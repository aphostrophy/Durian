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

struct sched_switch_args
{
    unsigned long long pad;
    char prev_comm[16];
    int prev_pid;
    int prev_prio;
    long long prev_state;
    char next_comm[16];
    int next_pid;
    int next_prio;
};

SEC("tracepoint/sched/sched_switch")
int bpf_prog(struct sched_switch_args *ctx)
{
    unsigned long long timestamp = bpf_ktime_get_ns();

    // prev_data for tracing task that is exiting the CPU
    struct sched_event_data_t prev_data = {};

    prev_data.pid = ctx->prev_pid;
    prev_data.prio = ctx->prev_prio;
    prev_data.ktime_ns = timestamp;
    // task goes back to the run queue
    prev_data.prev_task_state = TASK_RUNNING_CPU;
    prev_data.next_task_state = TASK_RUNNING_RQ;
    bpf_probe_read_kernel_str(&prev_data.comm, sizeof(prev_data.comm), ctx->prev_comm);

    // next_data for tracing task that is entering the CPU
    struct sched_event_data_t next_data = {};

    next_data.pid = ctx->next_pid;
    next_data.prio = ctx->next_prio;
    next_data.ktime_ns = timestamp;
    // task goes back to the cpu
    next_data.prev_task_state = TASK_RUNNING_RQ;
    next_data.next_task_state = TASK_RUNNING_CPU;
    bpf_probe_read_kernel_str(&next_data.comm, sizeof(next_data.comm), ctx->next_comm);

    int ret;

    ret = bpf_ringbuf_output(&sched_events, &prev_data, sizeof(struct sched_event_data_t), 0);
    if (ret != 0)
    {
        char msg[] = "[sched_switch:prev_data] ringbuf output err %d\n";
        bpf_trace_printk(msg, sizeof(msg), ret);
    }

    ret = bpf_ringbuf_output(&sched_events, &next_data, sizeof(struct sched_event_data_t), 0);
    if (ret != 0)
    {
        char msg[] = "[sched_switch:next_data] ringbuf output err %d\n";
        bpf_trace_printk(msg, sizeof(msg), ret);
    }

    return 0;
}

char _license[] SEC("license") = "GPL";