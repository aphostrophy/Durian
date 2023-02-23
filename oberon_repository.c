#include "oberon_repository.h"

void repository_track_task(oberon_ctx *ctx, int pid, const char comm[16], int prio)
{
    pipeline_push_command(ctx, "MSET %d %s", pid, comm);
}

void repository_untrack_task(oberon_ctx *ctx, int pid)
{
}