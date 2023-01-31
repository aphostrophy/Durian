#include <bpf/bpf.h>
#include "bpf_load.h"
#include "trace_helpers.h"
#include <stdio.h>
#include <errno.h>

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
    int fd, pinned;

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

    if (load_bpf_file("oberon_probes/sched/sched_switch.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    if (load_bpf_file("oberon_probes/sched/sched_wakeup.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    if (load_bpf_file("oberon_probes/sched/sched_process_wait.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    read_trace_pipe();

    return 0;
}