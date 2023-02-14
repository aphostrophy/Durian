#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include "../../oberon_maps.h"
#include "../../oberon_def.h"

struct bpf_map_def SEC("maps") sched_events = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = SCHED_EVENT_RINGBUF_SIZE,
};

struct sched_wakeup_args
{
    unsigned long long pad;
    char comm[16];
    int pid;
    int prio;
    int success;
    int target_cpu;
};

SEC("tracepoint/sched/sched_wakeup")
int bpf_prog(struct sched_wakeup_args *ctx)
{
    char msg[] = "task %d wakeup\n";
    bpf_trace_printk(msg, sizeof(msg), ctx->pid);
    return 0;
}

char _license[] SEC("license") = "GPL";