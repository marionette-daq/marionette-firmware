/*!
 * \file    mshell.c
 *
 * mshell is the Marionette shell. 
 * This is derived from the example ChibiOS-RT shell.
 *
 * @defgroup mshell Marionette Shell 
 * @{
 */

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


#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "chbsem.h"

#include "util_version.h"
#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"

#include "fetch.h"
#include "mshell.h"
#include "mshell_sync.h"

static          Mshell_status                   mshell_state;

static          VERSIONData                     version_data;
static          char                            prompt[MSHELL_MAX_PROMPT_LENGTH];
static          bool                            mshell_echo_chars = MSHELL_ECHO_INPUT_CHARS;

EventSource     mshell_terminated;
static void mshell_stream_putstr(BaseSequentialStream * chp, char * fmt, ...) ;

static void usage(BaseSequentialStream * chp, char * p)
{
	util_info(chp, "Usage: %s", p);
}

static void list_commands(BaseSequentialStream * chp, const MShellCommand * scp)
{
	while (scp->sc_name != NULL)
	{
		chprintf(chp, "+%s ", scp->sc_name);
		scp++;
	}
}

static void cmd_version(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	util_fwversion(&version_data);
	util_hwversion(&version_data);
	util_info(chp, "Firmware Version:%s", version_data.firmware);
	util_info(chp, "Hardware Version:0x%x-0x%x-0x%x", version_data.hardware.id_high, version_data.hardware.id_center, version_data.hardware.id_low);
	if(argc > 0)
	{
		usage(chp, "version");
		return;
	}
}

/*! \brief turn on the prompt
 */
static void cmd_prompt(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if(argc > 0)
	{
		usage(chp, "prompt");
		return;
	}
	strncpy(prompt, "m > ", MSHELL_MAX_PROMPT_LENGTH);
}

/*! \brief turn off the prompt
 */
static void cmd_noprompt(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if(argc > 0)
	{
		usage(chp, "noprompt");
		return;
	}
	prompt[0] = '\0';
}

/*! \brief toggle the echo of characters to the serial port
 */
static void cmd_echo(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if(argc > 0)
	{
		usage(chp, "echo");
		return;
	}
	mshell_echo_chars = true;
}

/*! \brief toggle the echo of characters to the serial port
 */
static void cmd_noecho(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if(argc > 0)
	{
		usage(chp, "noecho");
		return;
	}
	mshell_echo_chars = false;
}

/*! \brief information about firmware and hardware
 */
static void cmd_info(BaseSequentialStream * chp, int argc, char * argv[])
{
	(void)argv;

	util_fwversion(&version_data);
	util_hwversion(&version_data);

	if (argc > 0)
	{
		usage(chp, "info");
		return;
	}
	util_info(chp, "Firmware Version:%s", version_data.firmware);
	util_info(chp, "Hardware Version:0x%x-0x%x-0x%x", version_data.hardware.id_high, version_data.hardware.id_center, version_data.hardware.id_low);
	util_info(chp, "Kernel:%s", CH_KERNEL_VERSION);
#ifdef CH_COMPILER_NAME
	util_info(chp, "Compiler:%s", CH_COMPILER_NAME);
#endif
	util_info(chp, "Architecture:%s", CH_ARCHITECTURE_NAME);
#ifdef CH_CORE_VARIANT_NAME
	util_info(chp, "Core Variant:%s", CH_CORE_VARIANT_NAME);
#endif
#ifdef CH_PORT_INFO
	util_info(chp, "Port Info:%s", CH_PORT_INFO);
#endif
#ifdef PLATFORM_NAME
	util_info(chp, "Platform:%s", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
	util_info(chp, "Board:%s", BOARD_NAME);
#endif
#ifdef __DATE__
#ifdef __TIME__
	util_info(chp, "Build time:%s%s%s", __DATE__, " - ", __TIME__);
#endif
#endif
}

/*! \brief systicks (1mS default) since reboot
 */
static void cmd_systime(BaseSequentialStream * chp, int argc, char * argv[])
{
	(void)argv;
	if (argc > 0)
	{
		util_info(chp, "systime");
		return;
	}
	util_info(chp, "%lu", (unsigned long)chTimeNow());
}

/**
 * @brief   Array of the default commands.
 */
static MShellCommand local_commands[] =
{
	{"info", cmd_info},
	{"systime", cmd_systime},
	{"prompt", cmd_prompt},
	{"noprompt", cmd_noprompt},
  {"echo", cmd_echo},
  {"noecho", cmd_noecho},
	{"version", cmd_version},
	{NULL, NULL}
};


/*! \brief execute a given command in the shell
 */
static bool_t cmdexec(const MShellCommand * scp, BaseSequentialStream * chp,
                      char * name, int argc, char * argv[])
{
	while (scp->sc_name != NULL)
	{
		if (strcasecmp(scp->sc_name, name) == 0)
		{
			scp->sc_function(chp, argc, argv);
			return FALSE;
		}
		scp++;
	}
	return TRUE;
}

BaseSequentialStream * getMShellStreamPtr() {
		return(mshell_state.chp);
}

/*! \brief   MShell thread function.
 *
 * Marionette shell commands are escaped with a '+'
 * 
 * Fetch commands are parsed here through the call to fetch_parse()
 * \sa fetch.c
 *
 * @param[in] p         pointer to a @p BaseSequentialStream object
 * @return              Termination reason.
 * @retval RDY_OK       terminated by command.
 * @retval RDY_RESET    terminated by reset condition on the I/O channel.
 *
 */
static msg_t mshell_thread(void * p)
{
	int n;

	bool_t  ret; 
	BaseSequentialStream * chp   = ((MShellConfig *)p)->sc_channel;
	const MShellCommand  * scp   = ((MShellConfig *)p)->sc_commands;
	char * lp, *cmd, *tokp;

	char input_line[MSHELL_MAX_LINE_LENGTH];
	char command_line[MSHELL_MAX_LINE_LENGTH];
	char * args[MSHELL_MAX_ARGUMENTS + 1];

	mshell_state.chp = chp;
	strncpy(prompt, "m > ", MSHELL_MAX_PROMPT_LENGTH);
	chRegSetThreadName("mshell");
	chThdSleepMilliseconds(500);
	//! Initial Welcome Prompt
	chprintf(chp, "\r\nMarionette Shell (\"+help\" for shell commands)\r\n");

	//! initialize parser.
	fetch_init(chp) ;

	while (TRUE)
	{
		mshell_stream_putstr(chp, "%s", prompt);
		ret = mshellGetLine(chp, input_line, sizeof(input_line));
		if (ret)
		{
			util_info(chp, "logout");
			break;
		}
		if(input_line[0] == '+')    // use escape to process mshell commands
		{
			strncpy(command_line, &input_line[1], MSHELL_MAX_LINE_LENGTH);
			lp = _strtok(command_line, " \t", &tokp);
			cmd = lp;
			n = 0;
			while ((lp = _strtok(NULL, " \t", &tokp)) != NULL)
			{
				if (n >= MSHELL_MAX_ARGUMENTS)
				{
					util_error(chp, "too many arguments");
					cmd = NULL;
					break;
				}
				args[n++] = lp;
			}
			args[n] = NULL;
			if (cmd != NULL)
			{
				if (strcasecmp(cmd, "exit") == 0)
				{
					if (n > 0)
					{
						usage(chp, "exit");
						continue;
					}
					break;
				}
				else if (strcasecmp(cmd, "help") == 0)
				{
					if (n > 0)
					{
						usage(chp, "help");
						continue;
					}
					util_info(chp, "Marionette Shell Commands: +help +exit ");
					list_commands(chp, local_commands);
					if (scp != NULL)
					{
						list_commands(chp, scp);
					}
					chprintf(chp, "\r\n");
				}
				else if (cmdexec(local_commands, chp, cmd, n, args) &&
				                ((scp == NULL) || cmdexec(scp, chp, cmd, n, args)))
				{
					chprintf(chp, "%s", cmd);
					chprintf(chp, " ?\r\n");
				}
			}
		}
		else
		{
			strncpy(command_line, &input_line[0], MSHELL_MAX_LINE_LENGTH);
			if(!fetch_parse(chp, command_line))
			{
				DBG_MSG(chp, "Parse fail.");
				util_error(chp, "Unrecognized Fetch Command. Type \"?\" or \"help\" or \"+help\"");
			};
		}
	}
	mshellExit(RDY_OK);
	/* Never executed, silencing a warning.*/
	return 0;
}


/**
 * @brief   MShell manager initialization.
 *
 * @api
 */
void mshellInit()
{
	chEvtInit(&mshell_terminated);
	mshell_io_sem_init();
}

/**
 * @brief   Terminates the shell.
 * @note    Must be invoked from the command handlers.
 * @note    Does not return.
 *
 * @param[in] msg       shell exit code
 *
 * @api
 */
void mshellExit(msg_t msg)
{
	/* Atomically broadcasting the event source and terminating the thread,
	   there is not a chSysUnlock() because the thread terminates upon return.*/
	chSysLock();
	chEvtBroadcastI(&mshell_terminated);
	chThdExitS(msg);
}

/**
 * @brief   Spawns a new shell.
 * @pre     @p CH_USE_HEAP and @p CH_USE_DYNAMIC must be enabled.
 *
 * @param[in] scp       pointer to a @p MShellConfig object
 * @param[in] size      size of the shell working area to be allocated
 * @param[in] prio      priority level for the new shell
 * @return              A pointer to the shell thread.
 * @retval NULL         thread creation failed because memory allocation.
 *
 * @api
 */
#if CH_USE_HEAP && CH_USE_DYNAMIC
Thread * mshellCreate(const MShellConfig * scp, size_t size, tprio_t prio)
{
	return chThdCreateFromHeap(NULL, size, prio, mshell_thread, (void *)scp);
}
#endif

/*!
 * \brief   Create statically allocated shell thread.
 *
 * \param[in] scp       pointer to a \p MShellConfig object
 * \param[in] wsp       pointer to a working area dedicated to the shell thread stack
 * \param[in] size      size of the shell working area
 * \param[in] prio      priority level for the new shell
 * \return              A pointer to the shell thread.
 *
 */
Thread * shellCreateStatic(const MShellConfig * scp, void * wsp,
                           size_t size, tprio_t prio)
{
	return chThdCreateStatic(wsp, size, prio, mshell_thread, (void *)scp);
}

static bool_t mshell_stream_put(BaseSequentialStream * chp, uint8_t c) {
	int ret;
	chBSemWait( &mshell_io_sem );
    ret = chSequentialStreamPut(chp, c) ;
	chBSemSignal( &mshell_io_sem );
	return ret;
}	

static void mshell_stream_putstr(BaseSequentialStream * chp, char * fmt, ...) {
	va_list argList;
	va_start(argList, fmt);

	chBSemWait( &mshell_io_sem );
	chvprintf(chp, fmt, argList);
	chBSemSignal( &mshell_io_sem );
	va_end(argList);
}

/*!
 * \brief   Reads a whole line from the input channel.
 *
 * \param[in] chp       pointer to a \p BaseSequentialStream object
 * \param[in] line      pointer to the line buffer
 * \param[in] size      buffer maximum length
 * \return              The operation status.
 * \retval TRUE         the channel was reset or CTRL-D pressed.
 * \retval FALSE        operation successful.
 *
 */
#define        ASCII_EOT                        ((char) 0x4)
#define        ASCII_BACKSPACE                  ((char) 0x8)
#define        ASCII_DELETE                     ((char) 0x7F)
#define        ASCII_CTL_U                      ((char) 0x15)
#define        ASCII_SPACE                      ((char) 0x20)
bool_t mshellGetLine(BaseSequentialStream * chp, char * line, unsigned size)

{
	char * p = line;
	while (TRUE)
	{
		char c;
		if (chSequentialStreamRead(chp, (uint8_t *)&c, 1) == 0)
		{
			return TRUE;
		}
		if (c == ASCII_EOT)
		{
			if(mshell_echo_chars)
			{
				chprintf(chp, "^D");
			}
			return TRUE;
		}
		if ((c == ASCII_CTL_U))
		{
			while (p != line)
			{
				if(mshell_echo_chars)
				{
					mshell_stream_put(chp, ASCII_BACKSPACE);
					mshell_stream_put(chp, ASCII_SPACE);
					mshell_stream_put(chp, ASCII_BACKSPACE);
				}
				p--;
			}
			continue;
		}

		if ((c == ASCII_BACKSPACE) || (c == ASCII_DELETE))
		{
			if (p != line)
			{
				if(mshell_echo_chars)
				{
					mshell_stream_put(chp, c);
					mshell_stream_put(chp, ASCII_SPACE);
					mshell_stream_put(chp, c);
				}
				p--;
			}
			continue;
		}
		if (c == '\r')
		{
			if(mshell_echo_chars)
			{
				chprintf(chp, "\r\n");
			}
			*p = 0;
			return FALSE;
		}
		if (c == 0) {
			continue;
		}
		if (c < ASCII_SPACE)   // ignore all other special chars.....
		{
			continue;
		}
		if (p < line + size - 1)
		{
			if(mshell_echo_chars)
			{
				mshell_stream_put(chp, c);
			}
			*p++ = (char)c;
		}
	}
}


/** @} */

