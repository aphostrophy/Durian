CLANG = clang
EXECABLE = io_bound

.PHONY: clean

clean:
	rm -f *.o *.so $(EXECABLE)
	rm -f /sys/fs/bpf/oberon/sched_events

$(EXECABLE): io_bound.c
	$(CLANG) -o $(EXECABLE) io_bound.c

.DEFAULT_GOAL := all

all: clean $(EXECABLE)