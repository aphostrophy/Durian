#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include "../../durian_maps.h"
#include "../../durian_def.h"

struct bpf_map_def SEC("maps") sched_events = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = SCHED_EVENT_RINGBUF_SIZE,
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
    unsigned long long timestamp = bpf_ktime_get_ns();

    struct sched_event_data_t data = {};
    data.pid = ctx->pid;
    data.prio = ctx->prio;
    data.ktime_ns = timestamp;
    data.prev_task_state = TASK_RUNNING_CPU;
    data.next_task_state = __TASK_STOPPED;
    bpf_probe_read_kernel_str(&data.comm, sizeof(data.comm), ctx->comm);

    int ret;

    ret = bpf_ringbuf_output(&sched_events, &data, sizeof(struct sched_event_data_t), 0);
    if (ret != 0)
    {
        char msg[] = "Ringbuf output err %d\n";
        bpf_trace_printk(msg, sizeof(msg), ret);
    }

    return 0;
}

char _license[] SEC("license") = "GPL";