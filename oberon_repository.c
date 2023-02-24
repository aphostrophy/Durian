#include "oberon_repository.h"

char lua_script_update_stats_task_enters_cpu_sha1_hash[41] = "";
char lua_script_update_stats_task_exits_cpu_sha1_hash[41] = "";

const char *lua_script_update_stats_task_enters_cpu =
    "local ktime_ns = ARGV[1]\n"
    "local last_ktime_ns = redis.call('GET' ,KEYS[1])"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if last_seen_state == 0x0000 then -- TASK_RUNNING_RQ\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))"
    "   redis.call('SET', KEYS[2], 0x0001) -- TASK_RUNNING_CPU\n"
    "end\n";

const char *lua_script_update_stats_task_exits_cpu =
    "local ktime_ns = ARGV[1]\n"
    "local last_ktime_ns = redis.call('GET', KEYS[1])\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if last_seen_state == 0x0001 then -- TASK_RUNNING_CPU\n"
    "   local delta = tonumber(ktime_ns) - tonumber(last_ktime_ns)\n"
    "   redis.call('INCRBY', KEYS[3], delta)\n"
    "   redis.call('SET', KEYS[2], 0x0000) -- TASK_RUNNING_RQ\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))"
    "end\n";

void repository_track_task(oberon_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "MSET %d:comm %s "
                          "%d:prio %d "
                          "%d:total_cpu_time %d "
                          "%d:total_wait_time %d "
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

void repository_update_stats_task_enters_cpu(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%llu",
                          lua_script_update_stats_task_enters_cpu_sha1_hash, 2, pid, pid, ktime_ns);
}

void repository_update_stats_task_exits_cpu(oberon_ctx *ctx, int pid, unsigned long long ktime_ns)
{
    pipeline_push_command(ctx,
                          "EVALSHA %s "
                          "%d "
                          "%d:last_ktime_ns "
                          "%d:last_seen_state "
                          "%d:total_cpu_time "
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