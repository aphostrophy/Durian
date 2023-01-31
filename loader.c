#include "bpf_load.h"
#include "trace_helpers.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (load_bpf_file("oberon_probes/sched/sched_switch.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    if (load_bpf_file("oberon_probes/sched/sched_wakeup.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    if (load_bpf_file("oberon_probes/sched/sched_process_wait.o") != 0)
    {
        printf("The kernel didn't load the BPF program: %s\n", strerror(errno));
        return -1;
    }

    read_trace_pipe();

    return 0;
}