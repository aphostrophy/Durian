#include "oberon_repository.h"

void repository_track_task(oberon_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "MSET %d:comm %s "
                          "%d:prio %d "
                          "%d:total_cpu_time_ns %d "
                          "%d:total_wait_time_ns %d "
                          "%d:last_seen_state %d "
                          "%d:last_ktime_ns %llu",
                          pid, comm,
                          pid, prio,
                          pid, 0,
                          pid, 0,
                          pid, TASK_RUNNING_RQ,
                          pid, ktime_ns);
}

void repository_untrack_task(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "MSET %d:last_seen_state %d "
                          "%d:last_ktime_ns %llu",
                          pid, __TASK_STOPPED,
                          pid, ktime_ns);
}

void repository_update_stats_task_enters_cpu(oberon_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns)
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
                          "%llu "
                          "%s "
                          "%d",
                          lua_script_update_stats_task_enters_cpu_sha1_hash,
                          6,
                          pid, pid, pid, pid, pid, pid,
                          ktime_ns,
                          comm,
                          prio);
}

void repository_update_stats_task_exits_cpu(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%d:total_cpu_time_ns "
                          "%llu",
                          lua_script_update_stats_task_exits_cpu_sha1_hash,
                          3,
                          pid, pid, pid,
                          ktime_ns);
}

void repository_update_stats_task_wait_starts(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
}

void repository_update_stats_task_wait_ends(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
}