CLANG = clang
EXECABLE = cpu_bound

.PHONY: clean

clean:
	rm -f *.o *.so $(EXECABLE)
	rm -f /sys/fs/bpf/oberon/sched_events

$(EXECABLE): cpu_bound.c
	$(CLANG) -o $(EXECABLE) cpu_bound.c

.DEFAULT_GOAL := all

all: clean $(EXECABLE)