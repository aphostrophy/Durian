#ifndef __DURIAN_MAPS_H
#define __DURIAN_MAPS_H

extern const char *pin_basedir;
extern const char *sched_event_map_file_path;

struct sched_event_data_t
{
    int pid;
    char comm[16];
    int prio;
    int prev_task_state;
    int next_task_state;
    unsigned long long ktime_ns;
    /**
     * Actual task state when a task is context switched out of the CPU that is mapped
     * into durian task state variant
     *
     * Durian will always set next_task_state to TASK_RUNNING_RQ when a task is context
     * switched out of the CPU so we need this field to trace the actual task state (it's
     * arguable that it's better to put this in next_task_state instead in the future).
     */
    int trace_sched_switch_state;
    int delta; // used to pass delay field contained in some sched event tracepoints
};

#endif