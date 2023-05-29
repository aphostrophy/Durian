#include "durian_sched_stats_transactions.h"

char lua_script_track_task_sha1_hash[41] = "";
char lua_script_untrack_task_sha1_hash[41] = "";
char lua_script_update_stats_task_enters_cpu_sha1_hash[41] = "";
char lua_script_update_stats_task_exits_cpu_sha1_hash[41] = "";
char lua_script_update_stats_task_wait_starts_sha1_hash[41] = "";
char lua_script_update_stats_task_wait_ends_sha1_hash[41] = "";

/**
 * @brief Lua transaction script for starting initial task tracking mechanism
 *
 * Initialize task statistics with initial values
 * Add task pid to RUNNING_PID_SET
 * If another task with the same pid has been marked with EXPIRE
 * but hasn't been removed , this will overwrite the values AND
 * undo the automatic removal (TTL) with PERSIST
 *
 * @param KEYS[1] pid:comm
 * @param KEYS[2] pid:prio
 * @param KEYS[3] pid:total_cpu_time_ns
 * @param KEYS[4] pid:total_wait_time_ns
 * @param KEYS[5] pid:last_seen_state
 * @param KEYS[6] pid:last_ktime_ns
 * @param KEYS[7] pid:sched_stats_start_time_ns
 * @param KEYS[8] pid:nr_switches
 * @param KEYS[9] RUNNING_PID_SET
 *
 * @param ARGV[1] ktime_ns
 * @param ARGV[2] comm
 * @param ARGV[3] prio
 * @param ARGV[4] pid
 * @param ARGV[5] last_seen_state
 */
const char *lua_script_track_task =
    "local ktime_ns = ARGV[1]\n"
    "local comm = ARGV[2]\n"
    "local prio = ARGV[3]\n"
    "local pid = ARGV[4]\n"
    "local last_seen_state = ARGV[5]\n"

    "redis.call('SET', KEYS[1], comm) -- pid:comm\n"
    "redis.call('PERSIST', KEYS[1])\n"
    "redis.call('SET', KEYS[2], prio) -- pid:prio\n"
    "redis.call('PERSIST', KEYS[2])\n"
    "redis.call('SET', KEYS[3], 0) -- pid:total_cpu_time_ns\n"
    "redis.call('PERSIST', KEYS[3])\n"
    "redis.call('SET', KEYS[4], 0) -- pid:total_wait_time_ns\n"
    "redis.call('PERSIST', KEYS[4])\n"
    "redis.call('SET', KEYS[5], last_seen_state) -- pid:last_seen_state\n"
    "redis.call('PERSIST', KEYS[5])\n"
    "redis.call('SET', KEYS[6], tonumber(ktime_ns)) -- pid:last_ktime_ns\n"
    "redis.call('PERSIST', KEYS[6])\n"
    "redis.call('SET', KEYS[7], tonumber(ktime_ns)) -- pid:sched_stats_start_time_ns\n"
    "redis.call('PERSIST', KEYS[7])\n"
    "redis.call('SET', KEYS[8], 0) -- pid:nr_switches\n"
    "redis.call('PERSIST', KEYS[8])\n"

    "redis.call('SADD', KEYS[9], pid)\n";

/**
 * @brief Lua transaction script for untracking a task
 *
 * Remove task pid from RUNNING_PID_SET
 * Add automatic deletion timeout for all keys that are related to the task pid
 * TTL for deletion is 300 seconds (5 minutes)
 *
 * @param KEYS[1] pid:comm
 * @param KEYS[2] pid:prio
 * @param KEYS[3] pid:total_cpu_time_ns
 * @param KEYS[4] pid:total_wait_time_ns
 * @param KEYS[5] pid:last_seen_state
 * @param KEYS[6] pid:last_ktime_ns
 * @param KEYS[7] pid:sched_stats_start_time_ns
 * @param KEYS[8] pid:nr_switches
 * @param KEYS[9] RUNNING_PID_SET
 *
 * @param ARGV[1] ktime_ns
 * @param ARGV[2] pid
 * @param ARGV[3] last_seen_state
 */
const char *lua_script_untrack_task =
    "local ktime_ns = ARGV[1]\n"
    "local pid = ARGV[2]\n"
    "local last_seen_state = ARGV[3]\n"
    "redis.call('SET', KEYS[5], last_seen_state) -- pid:last_seen_state\n"
    "redis.call('SET', KEYS[6], ktime_ns) -- pid:last_ktime_ns\n"

    "redis.call('EXPIRE', KEYS[1], 300)\n"
    "redis.call('EXPIRE', KEYS[2], 300)\n"
    "redis.call('EXPIRE', KEYS[3], 300)\n"
    "redis.call('EXPIRE', KEYS[4], 300)\n"
    "redis.call('EXPIRE', KEYS[5], 300)\n"
    "redis.call('EXPIRE', KEYS[6], 300)\n"
    "redis.call('EXPIRE', KEYS[7], 300)\n"
    "redis.call('EXPIRE', KEYS[8], 300)\n"

    "redis.call('SREM', KEYS[9], pid)\n";

/**
 * @brief Lua transaction script that will modify the time statistics for a task
 * on CPU state change (task enter).
 *
 * Will do the initial task tracking mechanism if task is not already tracked.
 *
 * @param KEYS[1] pid:last_ktime_ns
 * @param KEYS[2] pid:last_seen_state
 *
 * @note extra KEYS used for task tracking mechanism
 * @param KEYS[3] pid:total_cpu_time_ns
 * @param KEYS[4] pid:total_wait_time_ns
 * @param KEYS[5] pid:comm
 * @param KEYS[6] pid:prio
 * @param KEYS[7] pid:sched_stats_start_time_ns
 * @param KEYS[8] pid:nr_switches
 * @param KEYS[9] RUNNING_PID_SET
 *
 * @param ARGV[1] ktime_ns
 *
 * @note extra ARGV used for task tracking mechanism
 * @param ARGV[2] comm
 * @param ARGV[3] prio
 * @param ARGV[4] pid
 * @param ARGV[5] last_seen_state
 *
 * @note pid:comm is going to be keep updated in the repository due to possibility of
 * comm value change by the kernel. First value is a fork of the parent's process
 * name and will be changed only before the second context switch.
 */
const char *lua_script_update_stats_task_enters_cpu =
    "local ktime_ns = ARGV[1]\n"
    "local comm = ARGV[2]\n"
    "local prio = ARGV[3]\n"
    "local pid = ARGV[4]\n"
    "local last_seen_state = ARGV[5]\n"
    "local last_ktime_ns = redis.call('GET' ,KEYS[1])\n"
    "if not last_ktime_ns then\n"
    "   --["
    "   If last_ktime_ns is not found we start tracking the task"
    "   We initialize everything exactly as in repository_track_task"
    "   This durian observer will always assume that the task has just been started"
    "   Hence we put 0 as a value for every time statistics"
    "   ]\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns)) -- pid:last_ktime_ns\n"
    "   redis.call('PERSIST', KEYS[1])\n"
    "   redis.call('SET', KEYS[2], last_seen_state) -- pid:last_seen_state\n"
    "   redis.call('PERSIST', KEYS[2])\n"
    "   redis.call('SET', KEYS[3], 0) -- pid:total_cpu_time_ns\n"
    "   redis.call('PERSIST', KEYS[3])\n"
    "   redis.call('SET', KEYS[4], 0) -- pid:total_wait_time_ns\n"
    "   redis.call('PERSIST', KEYS[4])\n"
    "   redis.call('SET', KEYS[5], comm) -- pid:comm\n"
    "   redis.call('PERSIST', KEYS[5])\n"
    "   redis.call('SET', KEYS[6], prio) -- pid:prio\n"
    "   redis.call('PERSIST', KEYS[6])\n"
    "   redis.call('SET', KEYS[7], tonumber(ktime_ns)) -- pid:sched_stats_start_time_ns\n"
    "   redis.call('PERSIST', KEYS[7])\n"
    "   redis.call('SET', KEYS[8], 0) -- pid:nr_switches\n"
    "   redis.call('PERSIST', KEYS[8])\n"

    "   redis.call('SADD', KEYS[9], pid)\n"
    "   last_ktime_ns = ktime_ns\n"
    "end\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if (last_seen_state == 0x0000 and ktime_ns >= last_ktime_ns) then -- TASK_RUNNING_RQ\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
    "   redis.call('SET', KEYS[2], 0x0001) -- TASK_RUNNING_CPU\n"
    "   redis.call('SET', KEYS[5], comm) -- pid:comm\n"
    "   redis.call('INCRBY', KEYS[8], 1)\n"
    "end\n";

/**
 * @brief Lua transaction script that will modify the time statistics for a task
 * on CPU state change (task exited)
 *
 * @param KEYS[1] pid:last_ktime_ns
 * @param KEYS[2] pid:last_seen_state
 * @param KEYS[3] pid:total_cpu_time_ns
 *
 * @param ARGV[1] ktime_ns
 * @param ARGV[2] trace_sched_switch_state
 */
const char *lua_script_update_stats_task_exits_cpu =
    "local ktime_ns = ARGV[1]\n"
    "local trace_sched_switch_state = ARGV[2]\n"
    "local last_ktime_ns = redis.call('GET', KEYS[1])\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if (last_seen_state == 0x0001 and ktime_ns >= last_ktime_ns) then -- TASK_RUNNING_CPU\n"
    "   local delta = tonumber(ktime_ns) - tonumber(last_ktime_ns)\n"
    "   redis.call('INCRBY', KEYS[3], delta)\n"
    "   redis.call('SET', KEYS[2], trace_sched_switch_state) -- trace_sched_switch_state\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
    "end\n";

/**
 * @brief Lua transaction script for modifying the time statistics of a task
 * on I/O state change (entering a wait queue).
 *
 * When a task enters any wait queue from TASK_RUNNING_CPU state, increment
 * total_cpu_time_ns by delta, update task state to TASK_WAITING, modify last_ktime_ns.
 *
 * @param KEYS[1] pid:last_ktime_ns
 * @param KEYS[2] pid:last_seen_state
 * @param KEYS[3] pid:total_cpu_time_ns
 *
 * @param ARGV[1] ktime_ns
 *
 * @note DEPRECATED
 */
const char *lua_script_update_stats_task_wait_starts =
    "local ktime_ns = ARGV[1]\n"
    "local last_ktime_ns = redis.call('GET', KEYS[1])\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if (last_seen_state == 0x0001 and ktime_ns >= last_ktime_ns) then --TASK_RUNNING_CPU\n"
    "   local delta = tonumber(ktime_ns) - tonumber(last_ktime_ns)\n"
    "   redis.call('INCRBY', KEYS[3], delta)\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
    "   redis.call('SET', KEYS[2], 0x0002) -- TASK_WAITING\n"
    "end\n";

/**
 * @brief Lua transaction script for modifying the time statistics of a task
 * on I/O state change (exiting a wait queue).
 *
 * When a task exits any wait queue from TASK_WAITING state, increment
 * total_wait_time_ns by delta, update task state to TASK_RUNNING_RQ, modify last_ktime_ns.
 *
 * @param KEYS[1] pid:last_ktime_ns
 * @param KEYS[2] pid:last_seen_state
 * @param KEYS[3] pid:total_wait_time_ns
 *
 * @param ARGV[1] ktime_ns
 */
const char *lua_script_update_stats_task_wait_ends =
    "local ktime_ns = ARGV[1]\n"
    "local last_ktime_ns = redis.call('GET', KEYS[1])\n"
    "local last_seen_state = tonumber(redis.call('GET', KEYS[2]))\n"
    "\n"
    "if (ktime_ns >= last_ktime_ns) then -- TASK_RUNNING_RQ\n"
    "   local delta = tonumber(ktime_ns) - tonumber(last_ktime_ns)\n"
    "   redis.call('INCRBY', KEYS[3], delta)\n"
    "   redis.call('SET', KEYS[1], tonumber(ktime_ns))\n"
    "   redis.call('SET', KEYS[2], 0x0000) -- TASK_RUNNING_RQ\n"
    "end\n";
