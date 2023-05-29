#ifndef __DURIAN_DEF_H
#define __DURIAN_DEF_H

#define PATH_MAX 4096
#define PID_MAX 32768
#define SCHED_EVENT_RINGBUF_SIZE 4096 * 4096 // HAS TO BE MULTIPLE OF PAGE SIZE

// TODO: Add namespacing to avoid conflicts with kernel state definitons
/**
 * Used in sched_event_data_t->prev_task_state and sched_event_data_t->next_task_state
 * @def TASK_RUNNING_RQ is used to represent the running state of a task on the runqueue.
 * When a task is on the runqueue, it is in a "runnable" state,
 * meaning that it can be scheduled to run on a CPU core
 *
 * @def TASK_RUNNING_CPU is used to represent the running state of a task on a CPU core.
 * When a task is running on a CPU, it is said to be in the "running" state.
 *
 * @def TASK_WAITING is used to represent tasks that is in a wait queue
 *
 * @def __TASK_STOPPED is used to represent task that is terminated in any way
 *
 * @note TASK_WAITING is known as TASK_INTERRUPTIBLE or TASK_UNINTERRUPTIBLE in the kernel
 */
#define TASK_RUNNING_RQ 0x0000
#define TASK_RUNNING_CPU 0x0001
#define TASK_WAITING 0x0002
#define __TASK_STOPPED 0x0003

/* task state bitmask, copied from include/linux/sched.h */
#define KERNEL_TASK_RUNNING 0
#define KERNEL_TASK_INTERRUPTIBLE 1
#define KERNEL_TASK_UNINTERRUPTIBLE 2
#define KERNEL__TASK_STOPPED 4
#define KERNEL__TASK_TRACED 8

#endif
