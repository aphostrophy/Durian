#include <stdio.h>
#include <errno.h>
#include <bpf/bpf.h>
#include "trace_helpers.h"
#include "oberon_maps.h"
#include "oberon_common_user_bpf.h"
#include "bpf_load.h"

int main(int argc, char **argv)
{
    struct bpf_object *bpf_obj;
    int err, fd, pinned, prog_fd;

    pinned = bpf_obj_get(task_time_stats_map_file_path);
    if (pinned < 0)
    {
        printf("Failed to find bpf object at %s: %s\n", task_time_stats_map_file_path, strerror(errno));
        fd = bpf_create_map_name(BPF_MAP_TYPE_HASH, "task_time_stats", sizeof(int), sizeof(struct task_time_stats_entry), PID_MAX, BPF_F_NO_PREALLOC);
        if (fd < 0)
        {
            printf("Failed to create map: %d (%s)\n", fd, strerror(errno));
            return -1;
        }

        pinned = bpf_obj_pin(fd, task_time_stats_map_file_path);
        if (pinned < 0)
        {
            printf("Failed to pin map to the file system: %d (%s)\n", pinned, strerror(errno));
            return -1;
        }
    }

    pinned = bpf_obj_get(task_time_stats_graveyard_map_file_path);
    if (pinned < 0)
    {
        printf("Failed to find bpf object at %s: %s\n", task_time_stats_graveyard_map_file_path, strerror(errno));
        fd = bpf_create_map_name(BPF_MAP_TYPE_HASH, "time_stats_graveyard", sizeof(int), sizeof(struct task_time_stats_entry), PID_MAX, BPF_F_NO_PREALLOC);
        if (fd < 0)
        {
            printf("Failed to create map: %d (%s)\n", fd, strerror(errno));
            return -1;
        }

        pinned = bpf_obj_pin(fd, task_time_stats_graveyard_map_file_path);
        if (pinned < 0)
        {
            printf("Failed to pin map to the file system: %d (%s)\n", pinned, strerror(errno));
            return -1;
        }
    }

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

        printf("FD: %d\n", fd);

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

    // bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_switch.o", pin_basedir);
    // if (!bpf_obj)
    // {
    //     printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    //     return -1;
    // }

    // bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_wakeup.o", pin_basedir);
    // if (!bpf_obj)
    // {
    //     printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    //     return -1;
    // }

    // bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_process_wait.o", pin_basedir);
    // if (!bpf_obj)
    // {
    //     printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    //     return -1;
    // }

    // bpf_obj = load_bpf_and_tracepoint_attach("oberon_probes/sched/sched_process_exit.o", pin_basedir);
    // if (!bpf_obj)
    // {
    //     printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    //     return -1;
    // }

    struct ring_buffer *rb;

    pinned = bpf_obj_get(sched_event_map_file_path);

    printf("pinned: %d\n", pinned);

    struct bpf_map_info map_info = {};
    uint32_t map_info_len = sizeof(struct bpf_map_info);
    bpf_obj_get_info_by_fd(pinned, &map_info, &map_info_len);

    printf("map name: %s\n", map_info.name);

    // rb = ring_buffer__new(bpf_map__fd())

    read_trace_pipe();

    return 0;
}