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

struct sched_wakeup_new_args
{
    unsigned long long pad;
    char comm[16];
    int pid;
    int prio;
    int success;
    int target_cpu;
};

SEC("tracepoint/sched/sched_wakeup_new")
int bpf_prog(struct sched_wakeup_new_args *ctx)
{
    if (ctx->success)
    {
        int key = ctx->pid;
        int prio = ctx->prio;
        int timestamp = bpf_ktime_get_ns();
        struct task_time_stats_entry e = {0};
        e.pid = key;
        e.prio = prio;
        e.last_timestamp = timestamp;
        bpf_map_update_elem(&task_time_stats, &key, &e, BPF_ANY);

        char msg[] = "task %d enters run queue for the first time\n";
        bpf_trace_printk(msg, sizeof(msg), ctx->pid);
    }
    return 0;
}

char _license[] SEC("license") = "GPL";