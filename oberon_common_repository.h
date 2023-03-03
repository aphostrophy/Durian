#ifndef __OBERON_COMMON_REPOSITORY_H
#define __OBERON_COMMON_REPOSITORY_H

#include <hiredis/hiredis.h>
#include <stdarg.h>
#include <string.h>

struct oberon_ctx
{
    struct redisContext *redis_context;
    int redis_cmd_cnt;
    int success;
};
typedef struct oberon_ctx oberon_ctx;

/**
 * Pushes a command onto the Redis pipeline using the given format and arguments.
 * Increments the `redis_cmd_cnt` field of the provided context by 1.
 *
 * @param ctx The oberon app context, which contains a reference to the Redis connection.
 * @param format A printf-style format string specifying the Redis command and any arguments.
 * @param ... Any additional arguments to be included in the Redis command.
 *
 * @note This function does not execute the Redis command. To execute the pipeline, call `redisGetReply` repeatedly after pushing all desired commands.
 */
void pipeline_push_command(oberon_ctx *ctx, const char *format, ...);

void pipeline_push_command_argv(oberon_ctx *ctx, int argc, const char **argv, const size_t *argvlen);

int load_transaction_script(oberon_ctx *ctx, const char *script, char script_hash[41]);

#endif