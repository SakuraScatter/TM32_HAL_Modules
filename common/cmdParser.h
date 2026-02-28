#ifndef __CMD_PARSER_H
#define __CMD_PARSER_H

#include <stdint.h>
#include <stddef.h>

#include "log.h"

// ------------------- Configuration -------------------

// Case-insensitive command matching (0=no,1=yes)
#ifndef CMD_CASE_INSENSITIVE
#define CMD_CASE_INSENSITIVE 1
#endif

// Enable sub-command support (0=no,1=yes)
#ifndef CMD_ENABLE_SUBCMD
#define CMD_ENABLE_SUBCMD    0
#endif

// ------------------- Limits -------------------
#ifndef CMD_MAX_ARGS
#define CMD_MAX_ARGS        8
#endif

#ifndef CMD_LINE_BUF_SIZE
#define CMD_LINE_BUF_SIZE   64
#endif

#ifndef CMD_MAX_COMMANDS
#define CMD_MAX_COMMANDS    16
#endif

// ------------------- Command handler -------------------
typedef enum {
    CMD_OK = 0,
    CMD_ERR_UNKNOWN_CMD,
    CMD_ERR_PARAM_TOO_FEW,
} cmd_result_t;

typedef int (*cmd_handler_t)(int argc, char *argv[]);

// ------------------- Command structure -------------------
typedef struct {
    const char   *name;
#if CMD_ENABLE_SUBCMD
    const char   *subcmd;
#endif
    cmd_handler_t handler;
    const char   *help;
    int          min_args; // min args excluding cmd/subcmd
} cmd_item_t;

// ------------------- Register APIs -------------------

#if CMD_ENABLE_SUBCMD
int cmd_register_ext(const char *name, const char *subcmd, cmd_handler_t handler, const char *help, int min_args);
#else
int cmd_register_ext(const char *name, cmd_handler_t handler, const char *help, int min_args);
#endif

// Old-style registration (compatible)
static inline int cmd_register(const char *name, cmd_handler_t handler, const char *help)
{
#if CMD_ENABLE_SUBCMD
    return cmd_register_ext(name, NULL, handler, help, 0);
#else
    return cmd_register_ext(name, handler, help, 0);
#endif
}

// ------------------- Other APIs -------------------
void cmd_set_delimiters(const char *delimiters);
void cmdRxChar(char ch);
cmd_result_t cmdProcessLine(const char *line);
void cmdProcessPending(void);

uint8_t cmd_get_cmd_count(void);
const char *cmd_get_cmd_name(uint8_t index);
#if CMD_ENABLE_SUBCMD
const char *cmd_get_cmd_subcmd(uint8_t index);
#endif
const char *cmd_get_cmd_help(uint8_t index);

#endif

