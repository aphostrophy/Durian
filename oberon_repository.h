#ifndef __OBERON_REPOSITORY_H
#define __OBERON_REPOSITORY_H

#include <hiredis/hiredis.h>
#include "oberon_common_repository.h"

void register_new_task(oberon_ctx *ctx, int pid, char comm[16], int prio);

#endif