/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    mshell.h
 * @brief   Simple CLI shell header.
 *
 * @addtogroup MSHELL
 * @{
 */

#ifndef _MSHELL_H_
#define _MSHELL_H_

#include <stdbool.h>

/**
 * @brief   Shell echo chars enable.
 */
#if !defined(MSHELL_MAX_LINE_LENGTH) || defined(__DOXYGEN__)
#define MSHELL_ECHO_INPUT_CHARS       ((bool) false)
#endif


/**
 * @brief   Shell maximum input line length.
 */
#if !defined(MSHELL_MAX_LINE_LENGTH) || defined(__DOXYGEN__)
#define MSHELL_MAX_LINE_LENGTH       256
#endif

/**
 * @brief   Shell prompt maximum length.
 */
#if !defined(MSHELL_MAX_PROMPT_LENGTH) || defined(__DOXYGEN__)
#define MSHELL_MAX_PROMPT_LENGTH       10
#endif


/**
 * @brief   Shell maximum arguments per command.
 */
#if !defined(SHELL_MAX_ARGUMENTS) || defined(__DOXYGEN__)
#define MSHELL_MAX_ARGUMENTS         10
#endif

/**
 * @brief   Command handler function type.
 */
typedef void (*shellcmd_t)(BaseSequentialStream *chp, int argc, char *argv[]);

/**
 * @brief   Custom command entry type.
 */
typedef struct {
  const char            *sc_name;           /**< @brief Command name.       */
  shellcmd_t            sc_function;        /**< @brief Command function.   */
} MShellCommand;

/**
 * @brief   Shell descriptor type.
 */
typedef struct {
  BaseSequentialStream  *sc_channel;        /**< @brief I/O channel associated
                                                 to the shell.              */
  const MShellCommand    *sc_commands;       /**< @brief Shell extra commands
                                                 table.                     */
} MShellConfig;

#if !defined(__DOXYGEN__)
extern EventSource shell_terminated;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void mshellInit(void);
  void mshellExit(msg_t msg);
  Thread *mshellCreate(const MShellConfig *scp, size_t size, tprio_t prio);
  Thread *mshellCreateStatic(const MShellConfig *scp, void *wsp,
                            size_t size, tprio_t prio);
  bool_t mshellGetLine(BaseSequentialStream *chp, char *line, unsigned size);
#ifdef __cplusplus
}
#endif

#endif /* _SHELL_H_ */

/** @} */
