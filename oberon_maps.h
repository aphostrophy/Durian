#ifndef __OBERON_MAPS_H
#define __OBERON_MAPS_H

extern const char *pin_basedir;
extern const char *task_time_stats_map_file_path;
extern const char *task_time_stats_graveyard_map_file_path;
extern const char *sched_event_map_file_path;

struct task_time_stats_entry
{
    int pid;
    char comm[16];
    int prio;
    int total_cpu_time;
    int total_wait_time;
    unsigned char current_state;
    unsigned long long last_timestamp;
};

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