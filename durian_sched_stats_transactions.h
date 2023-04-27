#ifndef __DURIAN_SCHED_STATS_TRANSACTIONS_H
#define __DURIAN_SCHED_STATS_TRANSACTIONS_H

extern const char *lua_script_track_task;
extern const char *lua_script_untrack_task;
extern const char *lua_script_update_stats_task_enters_cpu;
extern const char *lua_script_update_stats_task_exits_cpu;
extern const char *lua_script_update_stats_task_wait_starts;
extern const char *lua_script_update_stats_task_wait_ends;

extern char lua_script_track_task_sha1_hash[41]; // 40 characters for the hash plus one null terminator
extern char lua_script_untrack_task_sha1_hash[41];
extern char lua_script_update_stats_task_exits_cpu_sha1_hash[41];
extern char lua_script_update_stats_task_enters_cpu_sha1_hash[41];
extern char lua_script_update_stats_task_wait_starts_sha1_hash[41];
extern char lua_script_update_stats_task_wait_ends_sha1_hash[41];

#endif