#include <linux/bpf.h>
#include <linux/sched.h>

#define SEC(NAME) __attribute__((section(NAME), used))

static int (*bpf_trace_printk)(const char *fmt, int fmt_size, ...) =
    (void *)BPF_FUNC_trace_printk;

SEC("kretprobe/pick_next_task_fair")
int bpf_prog(struct pt_regs *ctx)
{
    char msg[] = "Hello, return from pick_next_task_fair!";
    bpf_trace_printk(msg, sizeof(msg));
    return 0;
}

char _license[] SEC("license") = "GPL";