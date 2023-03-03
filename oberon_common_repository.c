#include "oberon_common_repository.h"

void pipeline_push_command(oberon_ctx *ctx, const char *format, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    redisAppendCommand(ctx->redis_context, buf);
    ctx->redis_cmd_cnt += 1;
}

void pipeline_push_command_argv(oberon_ctx *ctx, int argc, const char **argv, const size_t *argvlen)
{
    redisAppendCommandArgv(ctx->redis_context, argc, argv, argvlen);
    ctx->redis_cmd_cnt += 1;
}

int load_transaction_script(oberon_ctx *ctx, const char *script, char script_hash[41])
{
    redisReply *reply = redisCommand(ctx->redis_context, "SCRIPT LOAD %s", script);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
    {
        fprintf(stderr, "Failed to load Lua script: %s\n", reply ? reply->str : "unknown error");
        return -1;
    }
    strcpy(script_hash, reply->str);
    printf("Successfully preloaded script %s\n", script_hash);
    freeReplyObject(reply);
    return 0;
}