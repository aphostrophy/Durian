CLANG = clang

EXECABLE = monitor-exec.o

PICK_NEXT_TASK_KRETPROBE_BPF = oberon_probes/pick_next_task_finishes
FINISH_TASK_SWITCH_KPROBE_BPF = oberon_probes/finish_task_switch_entry

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

LIBRARY_PATH = -L/usr/local/lib64
BPFSO = -lbpf

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

build_pick_next_task_kretprobe_bpf: ${PICK_NEXT_TASK_KRETPROBE_BPF.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(PICK_NEXT_TASK_KRETPROBE_BPF:=.c) $(CCINCLUDE) -o ${PICK_NEXT_TASK_KRETPROBE_BPF:=.o} 

build_finish_task_switch_kprobe_bpf: ${FINISH_TASK_SWITCH_KPROBE_BPF.c} ${BPFLOADER}
	$(CLANG) -O2 -target bpf -c $(FINISH_TASK_SWITCH_KPROBE_BPF:=.c) $(CCINCLUDE) -o ${FINISH_TASK_SWITCH_KPROBE_BPF:=.o} 

bpfload: build_pick_next_task_kretprobe_bpf build_finish_task_switch_kprobe_bpf
	clang $(CFLAGS) -o $(EXECABLE) -lelf $(LOADINCLUDE) $(LIBRARY_PATH) $(BPFSO) \
        $(BPFLOADER) $(BPFTEST) loader.c

$(EXECABLE): bpfload

.DEFAULT_GOAL := $(EXECABLE)