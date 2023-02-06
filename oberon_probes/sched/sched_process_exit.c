#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include "../../oberon_maps.h"

struct bpf_map_def SEC("maps") task_time_stats = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(int),
    .value_size = sizeof(struct task_time_stats_entry),
    .max_entries = 32768,
};

struct bpf_map_def SEC("maps") time_stats_graveyard = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(int),
    .value_size = sizeof(struct task_time_stats_entry),
    .max_entries = 32768,
};

struct sched_process_exit_args
{
    unsigned long long pad;
    char comm[16];
    int pid;
    int prio;
};

SEC("tracepoint/sched/sched_process_exit")
int bpf_prog(struct sched_process_exit_args *ctx)
{
    char msg[] = "task %d terminates\n";
    bpf_trace_printk(msg, sizeof(msg), ctx->pid);
    return 0;
}

char _license[] SEC("license") = "GPL";