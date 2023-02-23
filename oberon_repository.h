#ifndef __OBERON_REPOSITORY_H
#define __OBERON_REPOSITORY_H

#include <hiredis/hiredis.h>
#include "oberon_common_repository.h"

/**
 * Starts tracking a newly born task lifecycle to the repository
 *
 * @param ctx The oberon app context, which contains a reference to the Redis connection.
 * @param pid The process ID of the task to be tracked.
 * @param comm The name of the command associated with the task.
 * @param prio The priority of the task.
 *
 * Simply adds an entry for tracking task statistics.
 * The repository will track the task's total CPU time, total wait time, last seen task state,
 * and kernel time which indicates the time the task changes state.
 * It will be set to its initial values.
 * total_cpu_time : 0
 * total_wait_time : 0
 * last_seen_state : TASK_RUNNING
 * last_ktime_ns : current kernel time in nanoseconds
 *
 * @note This function does not execute the Redis command. To execute the pipeline, call `redisGetReply` repeatedly after pushing all desired commands.
 */
void repository_track_task(oberon_ctx *ctx, int pid, const char comm[16], int prio);

/**
 * Stops tracking a terminated task lifecycle from the repository
 *
 * @param ctx The oberon app context, which contains a reference to the Redis connection.
 * @param pid The process ID of the task to be tracked.
 *
 * Simply changes the last_seen_state of a task to __TASK_STOPPED
 * @note This function does not execute the Redis command. To execute the pipeline, call `redisGetReply` repeatedly after pushing all desired commands.
 */
void repository_untrack_task(oberon_ctx *ctx, int pid);

#endif