#include "oberon_sched_stats_transactions.h"

char lua_script_update_stats_task_enters_cpu_sha1_hash[41] = "";
char lua_script_update_stats_task_exits_cpu_sha1_hash[41] = "";

/**
 * Lua transaction script that will modify the time statistics for a task
 * Will do the initial task tracking mechanism if task is not already tracked
 * @param KEYS[1] pid:last_ktime_ns
 * @param KEYS[2] pid:last_seen_state
 *
 * @note extra KEYS used for task tracking mechanism
 * @param KEYS[3] pid:total_cpu_time
 * @param KEYS[4] pid:total_wait_time
 * @param KEYS[5] pid:comm
 * @param KEYS[6] pid:prio
 *
 * @param ARGV[1] ktime_ns
 *
 * @note extra ARGV used for task tracking mechanism
 * @param ARGV[2] comm
 * @param ARGV[3] prio
 */
const char *lua_script_update_stats_task_enters_cpu =
    "local ktime_ns = ARGV[1]\n"
    "local comm = ARGV[2]\n"
    "local prio = ARGV[3]\n"
    "local last_ktime_ns = redis.call('GET' ,KEYS[1])\n"
    "if not last_ktime_ns then\n"
    "   --["
    "   If last_ktime_ns is not found we start tracking the task"
    "   We initialize everything exactly as in repository_track_task"
    "   This oberon observer will always assume that the task has just been started"
    "   Hence we put 0 as a value for every time statistics"
    "   ]\n"
    "   redis.call('SET', KEYS[2], 0x0000)"
    "   redis.call('SET', KEYS[3], 0)\n"
    "   redis.call('SET', KEYS[4], 0)\n"
    "   redis.call('SET', KEYS[5], comm)\n"
    "   redis.call('SET', KEYS[6], prio)\n"
    "   last_ktime_ns = ktime_ns\n"
    "end\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if (last_seen_state == 0x0000 and ktime_ns >= last_ktime_ns) then -- TASK_RUNNING_RQ\n"
    "   if KEYS[1] == '0:last_ktime_ns' then\n"
    "       redis.log(redis.LOG_NOTICE, 'zeroo')\n"
    "   end\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
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
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
    "end\n";