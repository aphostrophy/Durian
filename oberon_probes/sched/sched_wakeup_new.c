#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include "../../oberon_maps.h"
#include "../../oberon_def.h"

struct bpf_map_def SEC("maps") task_time_stats = {
    .type = BPF_MAP_TYPE_RINGBUF,
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

struct bpf_map_def SEC("maps") sched_events = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = SCHED_EVENT_RINGBUF_SIZE,
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
        struct task_time_stats_entry e = {0};
        int key = ctx->pid;
        int prio = ctx->prio;
        unsigned long long timestamp = bpf_ktime_get_ns();

        e.pid = key;
        bpf_probe_read_kernel_str(&e.comm, sizeof(e.comm), ctx->comm);
        e.prio = prio;
        e.last_timestamp = timestamp;

        bpf_map_update_elem(&task_time_stats, &key, &e, BPF_ANY);

        struct sched_event_data_t data;
        data.pid = key;
        bpf_probe_read_kernel_str(&data.comm, sizeof(data.comm), ctx->comm);
        data.prio = prio;
        data.ktime_ns = timestamp;

        int ret;

        ret = bpf_ringbuf_output(&sched_events, &data, sizeof(struct sched_event_data_t), 0);
        if (ret != 0)
        {
            char msg[] = "Ringbuf output err %d\n";
            bpf_trace_printk(msg, sizeof(msg), ret);
        }
    }
    return 0;
}

char _license[] SEC("license") = "GPL";