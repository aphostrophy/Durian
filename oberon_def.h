#ifndef __OBERON_DEF_H
#define __OBERON_DEF_H

#define PATH_MAX 4096
#define PID_MAX 32768
#define SCHED_EVENT_RINGBUF_SIZE 4096 * 256 // HAS TO BE MULTIPLE OF PAGE SIZE

/**
 * Used in sched_event_data_t->prev_task_state and sched_event_data_t->next_task_state
 * TASK_WAITING is used to represent tasks that is in a wait queue
 * TASK_WAITING is known as TASK_INTERRUPTIBLE or TASK_UNINTERRUPTIBLE in the kernel
 */

#define TASK_RUNNING 0x0000
#define TASK_WAITING 0x0001
#define __TASK_STOPPED 0x0002

#endif
