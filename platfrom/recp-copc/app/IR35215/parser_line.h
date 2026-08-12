#ifndef __PARSER_LINE_H__
#define __PARSER_LINE_H__
#include "common.h"
#include "command.h"


#define CONFIG_SYS_CBSIZE 1024
#define CONFIG_SYS_MAXARGS  64
#define PROMPT_STR         "devctrl#:"

#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/


int readline_into_buffer(const char *const prompt, char *buffer, int timeout);
int readline (const char *const prompt);
int run_command(const char *cmd, int flag);
static int builtin_run_command(const char *cmd, int flag);
int parse_line (char *line, char *argv[]);

int make_argv(char *s, int argvsz, char *argv[]);

int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp);

int complete_cmdv(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]);

#endif