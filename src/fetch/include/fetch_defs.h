/*! \file fetch_defs.h
 * Definitions for fetch grammar
 * @addtogroup fetch
 * @{
 */


#ifndef FETCH_DEFS_H_
#define FETCH_DEFS_H_

#include "hal.h"

#define 		FETCH_MAX_LINE_CHARS					1024
#define 		FETCH_MAX_COMMANDS 						8
#define 		FETCH_MAX_DATA_ITEMS					260
#define 		FETCH_MAX_CMD_STRLEN					25
#define     FETCH_MAX_DATA_STRLEN         25
#define     FETCH_MAX_HELP_STRLEN         512

#define     FETCH_COMMAND_NOT_FOUND       -1

#define     FETCH_CMD_DELIM   ":."
#define     FETCH_DATA_DELIM  " ,"

typedef enum {
  FETCH_TOK_CMD = 0,
  FETCH_TOK_SUBCMD_0,
  FETCH_TOK_SUBCMD_1,
  FETCH_TOK_SUBCMD_2,
  FETCH_TOK_SUBCMD_3,
} fetch_token_t;

typedef bool (*fetch_func_t)(BaseSequentialStream *chp, char * cmd_list[], char * data_list[]);

typedef struct
{
  fetch_func_t function;
  const char * command;
  const char * help;
} fetch_command_t;

#endif
/*! @} */
