#include <stdio.h>
#include <errno.h>
#include <bpf/bpf.h>
#include "trace_helpers.h"
#include "oberon_maps.h"
#include "oberon_common_user_bpf.h"
#include "oberon_common_user_debug.h"
#include "bpf_load.h"

static int handle_rb_event(void *ctx, void *data, size_t data_size)
{
    const struct sched_event_data_t *e = data;

    printf("[%d:%s] prev: %s next: %s\n", e->pid, e->comm, get_task_state_name(e->prev_task_state), get_task_state_name(e->next_task_state));
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

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_wakeup.o", pin_basedir);
    if (!bpf_obj)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_process_wait.o", pin_basedir);
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
     * Start of RB Testing, will migrate to either Go or Rust in the future
     */
    struct ring_buffer *rb;

    fd = bpf_obj_get(sched_event_map_file_path);

    rb = ring_buffer__new(fd, handle_rb_event, NULL, NULL);
    if (!rb)
    {
        printf("Failed to create ring buffer %s\n", strerror(errno));
        return -1;
    }

    while (true)
    {
        err = ring_buffer__poll(rb, 100);
        if (err < 0)
        {
            printf("Error polling ring buffer: %d\n", err);
            break;
        }
    }
    /**
     * End of RB Testing
     */
    read_trace_pipe();

    return 0;
}