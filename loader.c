#include <stdio.h>
#include <errno.h>
#include <bpf/bpf.h>
#include "trace_helpers.h"
#include "oberon_maps.h"
#include "bpf_load.h"

#define PID_MAX 32768

int main(int argc, char **argv)
{
    int err, fd, pinned, prog_fd;

    pinned = bpf_obj_get(task_time_stats_map_file_path);
    if (pinned < 0)
    {
        printf("Failed to find bpf object at %s: %s\n", task_time_stats_map_file_path, strerror(errno));
        fd = bpf_create_map(BPF_MAP_TYPE_HASH, sizeof(int), sizeof(struct task_time_stats_entry), PID_MAX, BPF_F_NO_PREALLOC);
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
        fd = bpf_create_map(BPF_MAP_TYPE_HASH, sizeof(int), sizeof(struct task_time_stats_entry), PID_MAX, BPF_F_NO_PREALLOC);
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

    err = load_bpf_file("oberon_probes/sched/sched_switch.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = load_bpf_file("oberon_probes/sched/sched_wakeup.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = load_bpf_file("oberon_probes/sched/sched_process_wait.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = load_bpf_file("oberon_probes/sched/sched_wakeup_new.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    err = load_bpf_file("oberon_probes/sched/sched_process_exit.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    read_trace_pipe();

    return 0;
}