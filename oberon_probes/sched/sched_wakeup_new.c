#include <linux/bpf.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#define SEC(NAME) __attribute__((section(NAME), used))

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
        char msg[] = "task %d enters run queue for the first time\n";
        bpf_trace_printk(msg, sizeof(msg), ctx->pid);
    }
    return 0;
}

char _license[] SEC("license") = "GPL";