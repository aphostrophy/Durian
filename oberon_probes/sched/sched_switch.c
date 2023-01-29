#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#define SEC(NAME) __attribute__((section(NAME), used))

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
    char prev_comm[16];
    char next_comm[16];

    bpf_probe_read_str(prev_comm, sizeof(prev_comm), ctx->prev_comm);
    bpf_probe_read_str(next_comm, sizeof(next_comm), ctx->next_comm);
    char msg[] = "prev task: %d, next task: %d\n";
    bpf_trace_printk(msg, sizeof(msg), ctx->prev_pid, ctx->next_pid);
    return 0;
}

char _license[] SEC("license") = "GPL";