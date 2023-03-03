#include "oberon_sched_stats_transactions.h"

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