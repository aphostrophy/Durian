#ifndef __OBERON_COMMON_USER_DEBUG_H
#define __OBERON_COMMON_USER_DEBUG_H

/**
 * @brief Returns the macro name for the given task state.
 *
 * This function takes an integer representing a task state, and returns the corresponding macro name defined in the code.
 *
 * @param state The integer representing the task state.
 * @return The macro name of the given task state, or "UNKNOWN" if the state is not recognized.
 */
char *get_task_state_name(int state);

#endif