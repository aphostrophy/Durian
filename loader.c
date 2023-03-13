#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <bpf/bpf.h>
#include "trace_helpers.h"
#include <hiredis/hiredis.h>
#include "oberon_maps.h"
#include "oberon_common_user_bpf.h"
#include "oberon_common_user_debug.h"
#include "oberon_repository.h"
#include "bpf_load.h"

static int handle_rb_event(void *ctx, void *data, size_t data_size)
{
    struct oberon_ctx *ctx_data = (struct oberon_ctx *)ctx;
    const struct sched_event_data_t *e = data;

    if (e->prev_task_state == __TASK_STOPPED && e->next_task_state == TASK_RUNNING_RQ)
    {
        /* Task starts */
        repository_track_task(ctx_data, e->pid, e->comm, e->prio, e->ktime_ns);
    }
    else if (e->prev_task_state == TASK_RUNNING_CPU && e->next_task_state == __TASK_STOPPED)
    {
        /* Task terminates */
        repository_untrack_task(ctx_data, e->pid, e->ktime_ns);
    }
    else if (e->prev_task_state == TASK_RUNNING_CPU && e->next_task_state == TASK_RUNNING_RQ)
    {
        /* Task switches */
        repository_update_stats_task_exits_cpu(ctx_data, e->pid, e->ktime_ns);
    }
    else if (e->prev_task_state == TASK_RUNNING_RQ && e->next_task_state == TASK_RUNNING_CPU)
    {
        /* Task switches */
        repository_update_stats_task_enters_cpu(ctx_data, e->pid, e->comm, e->prio, e->ktime_ns);
    }
    else if (e->prev_task_state == TASK_WAITING && e->next_task_state == TASK_RUNNING_RQ)
    {
        /* Task exits a wait queue and enqueued to run queue */
        repository_update_stats_task_wait_ends(ctx_data, e->pid, e->ktime_ns);
    }
    else
    {
        printf("[%d:%s] task state change monitoring for prev: %s next: %s is not supported\n", e->pid, e->comm, get_task_state_name(e->prev_task_state), get_task_state_name(e->next_task_state));
    }
    return 0;
}

int main(int argc, char **argv)
{
    struct bpf_object *bpf_obj;
    int err, fd, pinned;

    pinned = bpf_obj_get(sched_event_map_file_path);
    if (pinned < 0)
    {
        printf("Failed to find bpf object at %s: %s\n", sched_event_map_file_path, strerror(errno));
        fd = bpf_create_map_name(BPF_MAP_TYPE_RINGBUF, "sched_events", 0, 0, SCHED_EVENT_RINGBUF_SIZE, 0);
        if (fd < 0)
        {
            printf("Failed to create map: %d (%s)\n", fd, strerror(errno));
            return -1;
        }

        pinned = bpf_obj_pin(fd, sched_event_map_file_path);
        if (pinned < 0)
        {
            printf("Failed to pin map to the file system: %d (%s)\n", pinned, strerror(errno));
            return -1;
        }
    }

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_wakeup_new.o", pin_basedir);
    if (!bpf_obj)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_switch.o", pin_basedir);
    if (!bpf_obj)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_stat_sleep.o", pin_basedir);
    if (!bpf_obj)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_process_exit.o", pin_basedir);
    if (!bpf_obj)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    /**
     * Connect to redis for storing persistent task statistics
     */
    redisContext *redis_context = redisConnect("localhost", 6379);
    if (redis_context == NULL || redis_context->err)
    {
        printf("Error: %s\n", redis_context == NULL ? "connection error" : redis_context->errstr);
        return -1;
    }

    oberon_ctx *ctx = malloc(sizeof(oberon_ctx));
    ctx->redis_context = redis_context;
    ctx->success = 1;
    ctx->redis_cmd_cnt = 0;

    /* Preload lua scripts */
    err = load_transaction_script(ctx, lua_script_track_task, lua_script_track_task_sha1_hash);
    if (err != 0)
    {
        return -1;
    }
    err = load_transaction_script(ctx, lua_script_untrack_task, lua_script_untrack_task_sha1_hash);
    if (err != 0)
    {
        return -1;
    }
    err = load_transaction_script(ctx, lua_script_update_stats_task_exits_cpu, lua_script_update_stats_task_exits_cpu_sha1_hash);
    if (err != 0)
    {
        return -1;
    }
    err = load_transaction_script(ctx, lua_script_update_stats_task_enters_cpu, lua_script_update_stats_task_enters_cpu_sha1_hash);
    if (err != 0)
    {
        return -1;
    }
    err = load_transaction_script(ctx, lua_script_update_stats_task_wait_ends, lua_script_update_stats_task_wait_ends_sha1_hash);
    if (err != 0)
    {
        return -1;
    }

    /**
     * Start of RB , will migrate to either Go or Rust in the future
     */
    struct ring_buffer *rb;

    fd = bpf_obj_get(sched_event_map_file_path);

    rb = ring_buffer__new(fd, handle_rb_event, ctx, NULL);
    if (!rb)
    {
        printf("Failed to create ring buffer %s\n", strerror(errno));
        return -1;
    }

    while (true)
    {
        err = ring_buffer__consume(rb);
        if (err < 0)
        {
            printf("Error polling ring buffer: %d\n", err);
            break;
        }
        /**
         * Submit single batch statistics to redis and read the replies
         */
        redisReply *reply;
        for (int i = 0; i < ctx->redis_cmd_cnt; i++)
        {
            int status = redisGetReply(redis_context, (void **)&reply);
            if (status != REDIS_OK)
            {
                // Handle error
                fprintf(stderr, "Error while reading redis reply: %d %s\n", status, ctx->redis_context->errstr);
            }
            if (reply != NULL)
            {
                // Process reply
                // ...
                if (reply->type == REDIS_REPLY_ERROR)
                {
                    printf("%s\n", reply->str);
                }
            }
            freeReplyObject(reply);
        }
        ctx->redis_cmd_cnt = 0;
    }
    /**
     * End of RB Testing
     */

    // read_trace_pipe();

    return 0;
}