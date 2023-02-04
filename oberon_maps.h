#ifndef __OBERON_MAPS_H
#define __OBERON_MAPS_H

extern const char *task_time_stats_map_file_path;
extern const char *task_time_stats_graveyard_map_file_path;

struct task_time_stats_entry
{
    int pid;
    int prio;
    int total_cpu_time;
    int total_wait_time;
    unsigned char current_state;
    int last_timestamp;
};

#endif