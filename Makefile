CLANG = clang

EXECABLE = oberond

SCHED_WAKEUP_NEW_PROBE_BPF = oberon_probes/sched/sched_wakeup_new
SCHED_WAKEUP_PROBE_BPF = oberon_probes/sched/sched_wakeup
SCHED_SWITCH_PROBE_BPF = oberon_probes/sched/sched_switch
SCHED_PROCESS_WAIT_PROBE_BPF = oberon_probes/sched/sched_process_wait
SCHED_WAIT_TASK_PROBE_BPF = oberon_probes/sched/sched_wait_task
SCHED_PROCESS_EXIT_PROBE_BPF = oberon_probes/sched/sched_process_exit
SCHED_STAT_SLEEP_PROBE_BPF = oberon_probes/sched/sched_stat_sleep

OBERON_MAPS = oberon_maps

KERNEL_SRC = /lib/modules/5.10.102.1-custom-Jesson-Yo+/build/
BPFTOOLS = $(KERNEL_SRC)/samples/bpf
BPFLOADER = $(BPFTOOLS)/bpf_load.c

BPFTEST = $(KERNEL_SRC)/tools/testing/selftests/bpf/trace_helpers.c

CCINCLUDE += -I$(KERNEL_SRC)/tools/testing/selftests/bpf

LOADINCLUDE += -I$(KERNEL_SRC)/samples/bpf
LOADINCLUDE += -I$(KERNEL_SRC)/tools/lib
LOADINCLUDE += -I$(KERNEL_SRC)/tools/perf
LOADINCLUDE += -I$(KERNEL_SRC)/tools/include
LOADINCLUDE += -I$(KERNEL_SRC)/tools/testing/selftests/bpf

OBERONINCLUDE += oberon_common_user_bpf.c
OBERONINCLUDE += oberon_common_user_debug.c
OBERONINCLUDE += oberon_common_repository.c
OBERONINCLUDE += oberon_maps.c
OBERONINCLUDE += oberon_repository.c
OBERONINCLUDE += oberon_sched_stats_transactions.c

LIBRARY_PATH = -L/usr/local/lib64
BPFSO = -lbpf
HIREDISSO = -lhiredis

# Setting -DHAVE_ATTR_TEST=0 for the kernel containing below patch:
# 06f84d1989b7 perf tools: Make usage of test_attr__* optional for perf-sys.h
#
# The patch was included in Linus's tree starting v5.5-rc1, but was also included
# in stable kernel branch linux-5.4.y. So it's hard to determine whether a kernel
# is affected based on the kernel version alone:
# - for a v5.4 kernel from Linus's tree, no;
# - for a v5.4 kernel from the stable tree (used by many distros), yes.
#
# So let's look at the actual kernel source code to decide.
#
# See more context at:
# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=06f84d1989b7e58d56fa2e448664585749d41221
# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=fce9501aec6bdda45ef3a5e365a5e0de7de7fe2d
CFLAGS += $(shell grep -q "define HAVE_ATTR_TEST 1" $(KERNEL_SRC)/tools/perf/perf-sys.h \
                  && echo "-DHAVE_ATTR_TEST=0")

.PHONY: clean $(CLANG) bpfload build

clean:
	rm -f *.o *.so $(EXECABLE)
	rm -f oberon_probes/sched/*.o

build_sched_wakeup_new_probe: ${SCHED_WAKEUP_NEW_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_WAKEUP_NEW_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_WAKEUP_NEW_PROBE_BPF:=.o} 

build_sched_wakeup_probe: ${SCHED_WAKEUP_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_WAKEUP_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_WAKEUP_PROBE_BPF:=.o} 

build_sched_switch_probe: ${SCHED_SWITCH_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_SWITCH_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_SWITCH_PROBE_BPF:=.o}

build_sched_process_wait_probe: ${SCHED_PROCESS_WAIT_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_PROCESS_WAIT_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_PROCESS_WAIT_PROBE_BPF:=.o}

build_sched_wait_task_probe: ${SCHED_WAIT_TASK_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_WAIT_TASK_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_WAIT_TASK_PROBE_BPF:=.o}

build_sched_process_exit_probe: ${SCHED_PROCESS_EXIT_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_PROCESS_EXIT_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_PROCESS_EXIT_PROBE_BPF:=.o}

build_sched_stat_sleep_probe: ${SCHED_STAT_SLEEP_PROBE_BPF.c} ${OBERON_MAPS.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(SCHED_STAT_SLEEP_PROBE_BPF:=.c) $(OBERON_MAPS.c) $(CCINCLUDE) -o ${SCHED_STAT_SLEEP_PROBE_BPF:=.o}

bpfload: build_sched_wakeup_new_probe \
		 build_sched_switch_probe \
		 build_sched_wakeup_probe \
		 build_sched_process_exit_probe \
		 build_sched_stat_sleep_probe
	clang $(CFLAGS) -o $(EXECABLE) -lelf $(LOADINCLUDE) $(LIBRARY_PATH) $(BPFSO) $(HIREDISSO) \
        $(BPFLOADER) $(BPFTEST) $(OBERONINCLUDE) oberond.c -I KERNEL_SRC

$(EXECABLE): bpfload

.DEFAULT_GOAL := $(EXECABLE)