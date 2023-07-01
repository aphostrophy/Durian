#include "durian_repository.h"

void repository_track_task(durian_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:comm "
                          "%d:prio "
                          "%d:total_cpu_time_ns "
                          "%d:total_wait_time_ns "
                          "%d:last_seen_state "
                          "%d:last_ktime_ns "
                          "%d:sched_stats_start_time_ns "
                          "%d:nr_switches "
                          "%d:nr_wait_switches "
                          "%s "
                          "%llu "
                          "%s "
                          "%d "
                          "%d "
                          "%d",
                          lua_script_track_task_sha1_hash,
                          10,
                          pid, pid, pid, pid, pid, pid, pid, pid, pid, RUNNING_PID_SET,
                          ktime_ns,
                          comm,
                          prio,
                          pid,
                          TASK_RUNNING_RQ);
}

void repository_untrack_task(durian_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:comm "
                          "%d:prio "
                          "%d:total_cpu_time_ns "
                          "%d:total_wait_time_ns "
                          "%d:last_seen_state "
                          "%d:last_ktime_ns "
                          "%d:sched_stats_start_time_ns "
                          "%d:nr_switches "
                          "%d:nr_wait_switches "
                          "%s "
                          "%s "
                          "%llu "
                          "%d "
                          "%d",
                          lua_script_untrack_task_sha1_hash,
                          11,
                          pid, pid, pid, pid, pid, pid, pid, pid, pid, RUNNING_PID_SET, EXPIRED_PID_SET,
                          ktime_ns,
                          pid,
                          __TASK_STOPPED);
}

void repository_update_stats_task_enters_cpu(durian_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%d:total_cpu_time_ns "
                          "%d:total_wait_time_ns "
                          "%d:comm "
                          "%d:prio "
                          "%d:sched_stats_start_time_ns "
                          "%d:nr_switches "
                          "%d:nr_wait_switches "
                          "%s "
                          "%llu "
                          "%s "
                          "%d "
                          "%d "
                          "%d ",
                          lua_script_update_stats_task_enters_cpu_sha1_hash,
                          10,
                          pid, pid, pid, pid, pid, pid, pid, pid, pid, RUNNING_PID_SET,
                          ktime_ns,
                          comm,
                          prio,
                          pid,
                          TASK_RUNNING_RQ);
}

void repository_update_stats_task_exits_cpu(durian_ctx *ctx, int pid, unsigned long long ktime_ns, int trace_sched_switch_state)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%d:total_cpu_time_ns "
                          "%d:nr_wait_switches "
                          "%llu "
                          "%d",
                          lua_script_update_stats_task_exits_cpu_sha1_hash,
                          4,
                          pid, pid, pid, pid,
                          ktime_ns,
                          trace_sched_switch_state);
}

void repository_update_stats_task_wait_ends(durian_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%d:total_wait_time_ns "
                          "%llu",
                          lua_script_update_stats_task_wait_ends_sha1_hash,
                          3,
                          pid, pid, pid,
                          ktime_ns);
}