#include "oberon_common_user_debug.h"

char *get_task_state_name(int state)
{
    switch (state)
    {
    case 0:
        return "TASK_RUNNING";
    case 1:
        return "TASK_WAITING";
    case 2:
        return "__TASK_STOPPED";
    default:
        return "UNKNOWN";
    }
}