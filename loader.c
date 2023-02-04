#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <bpf/bpf.h>
#include "trace_helpers.h"
#include "src/oberon_loader.h"
#include "bpf_load.h"

#define PID_MAX 32768

static const char *task_time_stats_map_file_path = "/sys/fs/bpf/task_time_stats";

struct task_time_stats_entry
{
    int pid;
    int prio;
    int total_cpu_time;
    int total_wait_time;
    u8 current_state;
    int last_timestamp;
};

int main(int argc, char **argv)
{
    int err, fd, pinned, prog_fd;

    pinned = bpf_obj_get(task_time_stats_map_file_path);
    if (pinned < 0)
    {
        printf("Failed to find bpf object at %s: %s\n", task_time_stats_map_file_path, strerror(errno));
        fd = bpf_create_map(BPF_MAP_TYPE_HASH, sizeof(int), sizeof(struct task_time_stats_entry), PID_MAX + 1, BPF_F_NO_PREALLOC);
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

    err = link_oberon_tracepoint_probe("oberon_probes/sched/sched_switch.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = link_oberon_tracepoint_probe("oberon_probes/sched/sched_wakeup.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = link_oberon_tracepoint_probe("oberon_probes/sched/sched_process_wait.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    err = link_oberon_tracepoint_probe("oberon_probes/sched/sched_wakeup_new.o");
    if (err)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
    }

    read_trace_pipe();

    return 0;
}