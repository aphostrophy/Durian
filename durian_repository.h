#ifndef __DURIAN_REPOSITORY_H
#define __DURIAN_REPOSITORY_H

#include <hiredis/hiredis.h>
#include "durian_def.h"
#include "durian_common_repository.h"
#include "durian_sched_stats_transactions.h"

#define RUNNING_PID_SET "RUNNING_PID_SET"

/**
 * Starts tracking a newly born task lifecycle to the repository
 *
 * @param ctx The durian app context, which contains a reference to the Redis connection.
 * @param pid The process ID of the task to be tracked.
 * @param comm The name of the command associated with the task.
 * @param prio The priority of the task.
 * @param task_state The most recent visible state of the task
 * @param ktime_ns The kernel time in nanoseconds in when the sched event happens
 *
 * Simply adds an entry for tracking task statistics.
 * The repository will track the task's total CPU time, total wait time, last seen task state,
 * and kernel time which indicates the time the task changes state.
 * It will be set to its initial values.
 * total_cpu_time_ns : 0
 * total_wait_time_ns : 0
 * last_seen_state : TASK_RUNNING_RQ
 * last_ktime_ns : kernel time in nanoseconds in when the sched event happens
 *
 * @note This function does not execute the Redis command. To execute the pipeline, call `redisGetReply` repeatedly after pushing all desired commands.
 */
void repository_track_task(durian_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns);

/**
 * Stops tracking a terminated task lifecycle from the repository
 *
 * @param ctx The durian app context, which contains a reference to the Redis connection.
 * @param pid The process ID of the task to be tracked.
 * @param task_state The most recent visible state of the task
 * @param ktime_ns The kernel time in nanoseconds in when the sched event happens
 *
 * Simply changes the last_seen_state of a task to __TASK_STOPPED
 * @note This function does not execute the Redis command. To execute the pipeline, call `redisGetReply` repeatedly after pushing all desired commands.
 */
void repository_untrack_task(durian_ctx *ctx, int pid, unsigned long long ktime_ns);

/**
 *
 */
void repository_update_stats_task_enters_cpu(durian_ctx *ctx, int pid, const char comm[16], int prio, unsigned long long ktime_ns);

/**
 *
 */
void repository_update_stats_task_exits_cpu(durian_ctx *ctx, int pid, unsigned long long ktime_ns);

/**
 *
 */
void repository_update_stats_task_wait_ends(durian_ctx *ctx, int pid, unsigned long long ktime_ns);

#endif