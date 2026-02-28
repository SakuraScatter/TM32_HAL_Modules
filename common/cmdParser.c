#include "cmdParser.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static cmd_item_t cmd_table[CMD_MAX_COMMANDS];
static uint8_t cmd_count = 0;

static char cmd_line_buf[CMD_LINE_BUF_SIZE];
static uint8_t cmd_line_index = 0;
static volatile uint8_t cmd_ready_flag = 0;

static const char *cmd_delimiters = " \t=,";


// ------------------- string compare -------------------
static int cmd_strcmp(const char *s1, const char *s2)
{
#if CMD_CASE_INSENSITIVE
    while (*s1 && *s2) {
        char c1 = (*s1 >= 'A' && *s1 <= 'Z') ? *s1 + 32 : *s1;
        char c2 = (*s2 >= 'A' && *s2 <= 'Z') ? *s2 + 32 : *s2;
        if (c1 != c2) return c1 - c2;
        s1++; s2++;
    }
    return *s1 - *s2;
#else
    return strcmp(s1, s2);
#endif
}

#if CMD_ENABLE_SUBCMD
int cmd_register_ext(const char *name, const char *subcmd, cmd_handler_t handler, const char *help, int min_args)
{
    if (cmd_count >= CMD_MAX_COMMANDS || !name || !handler) return 0;
    cmd_table[cmd_count].name = name;
    cmd_table[cmd_count].subcmd = subcmd;
    cmd_table[cmd_count].handler = handler;
    cmd_table[cmd_count].help = help;
    cmd_table[cmd_count].min_args = min_args;
    cmd_count++;
    return 1;
}
#else
int cmd_register_ext(const char *name, cmd_handler_t handler, const char *help, int min_args)
{
    if (cmd_count >= CMD_MAX_COMMANDS || !name || !handler) return 0;
    cmd_table[cmd_count].name = name;
    cmd_table[cmd_count].handler = handler;
    cmd_table[cmd_count].help = help;
    cmd_table[cmd_count].min_args = min_args;
    cmd_count++;
    return 1;
}
#endif

void cmd_set_delimiters(const char *delimiters)
{
    if (delimiters) cmd_delimiters = delimiters;
}

void cmdRxChar(char ch)
{
    if (ch == '\r' || ch == '\n') {
        if (cmd_line_index > 0) {
            cmd_line_buf[cmd_line_index] = '\0';
            cmd_ready_flag = 1;
            cmd_line_index = 0;
        }
    } else {
        if (cmd_line_index < CMD_LINE_BUF_SIZE - 1)
            cmd_line_buf[cmd_line_index++] = ch;
        else
            cmd_line_index = 0;
    }
}

cmd_result_t cmdProcessLine(const char *line)
{
    char buffer[CMD_LINE_BUF_SIZE];
    char *argv[CMD_MAX_ARGS];
    int argc = 0;
    char *token;

    if (!line) return CMD_ERR_UNKNOWN_CMD;

    strncpy(buffer, line, CMD_LINE_BUF_SIZE - 1);
    buffer[CMD_LINE_BUF_SIZE - 1] = '\0';

    token = strtok(buffer, cmd_delimiters);
    while (token && argc < CMD_MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, cmd_delimiters);
    }

    if (argc == 0)
        return CMD_ERR_UNKNOWN_CMD;

    for (uint8_t i = 0; i < cmd_count; i++) {

        if (cmd_strcmp(argv[0], cmd_table[i].name) != 0)
            continue;

#if CMD_ENABLE_SUBCMD
        if (cmd_table[i].subcmd) {
            if (argc < 2)
                continue;
            if (cmd_strcmp(argv[1], cmd_table[i].subcmd) != 0)
                continue;
        }
#endif

        int arg_count = argc - 1;

#if CMD_ENABLE_SUBCMD
        if (cmd_table[i].subcmd)
            arg_count--;
#endif

        if (arg_count < cmd_table[i].min_args)
            return CMD_ERR_PARAM_TOO_FEW;

        cmd_table[i].handler(argc, argv);
        return CMD_OK;
    }

    return CMD_ERR_UNKNOWN_CMD;
}


static char line_copy[CMD_LINE_BUF_SIZE];
void cmdProcessPending(void)
{
    if (cmd_ready_flag) {
        __disable_irq();                     // 关中断
        cmd_ready_flag = 0;
        strncpy(line_copy, cmd_line_buf, CMD_LINE_BUF_SIZE);
        __enable_irq();                      // 开中断

        cmd_result_t res = cmdProcessLine(line_copy);

        if (CMD_ERR_UNKNOWN_CMD == res) {
            LOG_ERROR("Unknown Cmd>>>%s", line_copy);
        }
    }
}

// ------------------- getters -------------------
uint8_t cmd_get_cmd_count(void) { return cmd_count; }
const char *cmd_get_cmd_name(uint8_t index) { return index<cmd_count ? cmd_table[index].name : NULL; }
#if CMD_ENABLE_SUBCMD
const char *cmd_get_cmd_subcmd(uint8_t index) { return index<cmd_count ? cmd_table[index].subcmd : NULL; }
#endif
const char *cmd_get_cmd_help(uint8_t index) { return index<cmd_count ? cmd_table[index].help : NULL; }

