#include "oberon_common_repository.h"

void pipeline_push_command(oberon_ctx *ctx, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    redisAppendCommand(ctx->redis_context, format, args);
    va_end(args);
    ctx->redis_cmd_cnt += 1;
}