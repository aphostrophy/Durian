#ifndef __OBERON_MAPS_H
#define __OBERON_MAPS_H

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
};

#endif