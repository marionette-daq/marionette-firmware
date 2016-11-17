/*! \file fetch_defs.h
 * Definitions for fetch grammar
 * @addtogroup fetch
 * @{
 */


#ifndef FETCH_DEFS_H_
#define FETCH_DEFS_H_

#include "hal.h"

#define 		FETCH_MAX_LINE_CHARS					1024
#define 		FETCH_MAX_COMMAND_TOKS 				8
#define 		FETCH_MAX_DATA_TOKS 					260
#define     FETCH_MAX_HELP_STRLEN         512
#define     FETCH_MAX_DATA_STRLEN         FETCH_MAX_LINE_CHARS
#define     FETCH_MAX_CMD_STRLEN          FETCH_MAX_LINE_CHARS

// depricated: remove once ragel command matching is done
#define     FETCH_COMMAND_NOT_FOUND       -1

typedef bool (*fetch_func_t)(BaseSequentialStream *chp, uint32_t argc, char * argv[]);

// depricated: remove once ragel command matching is done
typedef struct
{
  fetch_func_t function;
  const char * command;
  const char * help;
} fetch_command_t;

#endif
/*! @} */
