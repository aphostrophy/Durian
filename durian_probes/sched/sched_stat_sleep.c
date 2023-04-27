#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <stdint.h>
#include "../../durian_maps.h"
#include "../../durian_def.h"

struct bpf_map_def SEC("maps") sched_events = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = SCHED_EVENT_RINGBUF_SIZE,
};

struct sched_stat_sleep
{
    unsigned long long pad;
    char comm[16];
    int pid;
    uint64_t delay;
};

SEC("tracepoint/sched/sched_stat_sleep")
int bpf_prog(struct sched_stat_sleep *ctx)
{
    unsigned long long timestamp = bpf_ktime_get_ns();
    struct sched_event_data_t data = {};

    data.pid = ctx->pid;
    data.prio = -1; // Not given in ctx, mark as -1 to differentiate with prio 0
    data.ktime_ns = timestamp;
    data.prev_task_state = TASK_WAITING;
    data.next_task_state = TASK_RUNNING_RQ;
    data.delta = ctx->delay;

    bpf_probe_read_kernel_str(&data.comm, sizeof(data.comm), ctx->comm);

    int ret;

    char msg[] = "[sched_stat_sleep:data] ringbuf output err\n";
    bpf_trace_printk(msg, sizeof(msg));

    ret = bpf_ringbuf_output(&sched_events, &data, sizeof(struct sched_event_data_t), 0);
    if (ret != 0)
    {
        char msg[] = "[sched_stat_sleep:data] ringbuf output err %d\n";
        bpf_trace_printk(msg, sizeof(msg), ret);
    }

    return 0;
}

char _license[] SEC("license") = "GPL";