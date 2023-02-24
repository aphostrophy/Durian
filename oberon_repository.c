#include "oberon_repository.h"

void repository_track_task(oberon_ctx *ctx, int pid, const char comm[16], int prio, int task_state, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "MSET %d:comm %s "
                          "%d:prio %d "
                          "%d:total_cpu_time %d "
                          "%d:total_wait_time %d "
                          "%d:last_seen_state %d "
                          "%d:last_ktime_ns %d",
                          pid, comm,
                          pid, prio,
                          pid, 0,
                          pid, 0,
                          pid, task_state,
                          pid, ktime_ns);
}

void repository_untrack_task(oberon_ctx *ctx, int pid, int task_state, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "MSET %d:last_seen_state %d"
                          "%d:last_ktime_ns %d",
                          pid, task_state,
                          pid, ktime_ns);
}

void repository_update_stats_task_enters_cpu(oberon_ctx *ctx, int pid, int prev_task_state, int next_task_state, unsigned long long ktime_ns)
{
}

void repository_update_stats_task_exits_cpu(oberon_ctx *ctx, int pid, int prev_task_state, int next_task_state, unsigned long long ktime_ns)
{
}