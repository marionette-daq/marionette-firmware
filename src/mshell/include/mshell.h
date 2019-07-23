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

/*!
 * \file    mshell.h
 * \brief   Simple CLI shell header.
 *
 * \addtogroup mshell
 * @{
 */
#ifndef _MSHELL_H_
#define _MSHELL_H_

#include <stdbool.h>

/**
 * @brief   Shell maximum input line length.
 */
#if !defined(MSHELL_MAX_LINE_LENGTH) || defined(__DOXYGEN__)
#define MSHELL_MAX_LINE_LENGTH       1024
#endif

/**
 * @brief   Shell maximum arguments per command.
 */
#if !defined(MSHELL_MAX_ARGUMENTS) || defined(__DOXYGEN__)
#define MSHELL_MAX_ARGUMENTS         10
#endif


#if !defined(MSHELL_WELCOME_STR) || defined(__DOXYGEN__)
#define MSHELL_WELCOME_STR "Marionette Shell (\"help\" for fetch commands and  \"+help\" for shell commands)"
#endif

/**
 * @brief   Command handler function type.
 */
typedef bool (*shellcmd_t)(BaseSequentialStream *chp, int argc, char *argv[]);

/**
 * @brief   Custom command entry type.
 */
typedef struct {
  shellcmd_t            sc_function;        /**< @brief Command function.   */
  const char          * sc_name;            /**< @brief Command name.       */
  const char          * sc_help;            /**< @brief Command help string. */
} mshell_command_t;

/**
 * @brief   Shell descriptor type.
 */
typedef struct {
  BaseAsynchronousChannel * channel;        /**< @brief I/O channel associated to the shell. */
	const char * prompt;                      /**< @brief string to print for shell prompt. */
	bool show_prompt;                         /**< @brief print shell prompt. */
  bool echo_chars;                          /**< @brief print chars back to terminal. */
  const mshell_command_t * commands;        /**< @brief Shell extra commands table. */
} mshell_config_t;

typedef enum {
  MSHELL_MSG_OK = 0,
  MSHELL_MSG_ERROR,
  MSHELL_MSG_EXIT,
  MSHELL_MSG_BREAK,
  MSHELL_MSG_TIMEOUT
} mshell_msg_t;

#ifdef __cplusplus
extern "C" {
#endif

  void mshell_init(void);
  void mshell_start(const mshell_config_t *cfg);
  void mshell_stop(void);
  mshell_msg_t mshell_get_line(BaseAsynchronousChannel * channel, char * line, unsigned size, bool echo_chars );

#ifdef __cplusplus
}
#endif

#endif

/** @} */
