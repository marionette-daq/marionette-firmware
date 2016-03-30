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

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"

#include "fetch.h"
#include "mshell.h"
#include "mshell_sync.h"
#include "mshell_state.h"

static bool mshell_echo_chars = MSHELL_ECHO_INPUT_CHARS;

event_source_t mshell_terminated;

static void list_commands(BaseSequentialStream * chp, const mshell_command_t * scp)
{
	while (scp->sc_name != NULL)
	{
		util_message_info(chp, "+%s", scp->sc_name);
		util_message_info(chp, "\t%s", scp->sc_help);
		scp++;
	}
}

/*! \brief turn on the prompt
 */
static bool cmd_prompt(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if(argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'prompt'");
		return false;
	}
	setMShellVisiblePrompt(true);
	setMShellPrompt("m > ");
  return true;
}

/*! \brief turn off the prompt
 */
static bool cmd_noprompt(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'noprompt'");
		return false;
	}
	setMShellVisiblePrompt(false);
	setMShellPrompt("");
  return true;
}

/*! \brief toggle the echo of characters to the serial port
 */
static bool cmd_echo(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'echo'");
		return false;
	}
	mshell_echo_chars = true;
  return true;
}

/*! \brief toggle the echo of characters to the serial port
 */
static bool cmd_noecho(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'noecho'");
		return false;
	}
	mshell_echo_chars = false;
  return true;
}

/*! \brief information about firmware and hardware
 */
static bool cmd_info(BaseSequentialStream * chp, int argc, char * argv[])
{
	(void)argv;
  uint32_t chip_id[3] = {0,0,0};
  
  chip_id[0] = *(uint32_t*)STM32F4_UNIQUE_ID_LOW;
  chip_id[1] = *(uint32_t*)STM32F4_UNIQUE_ID_CENTER;
  chip_id[2] = *(uint32_t*)STM32F4_UNIQUE_ID_HIGH;

	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'info'");
		return false;
	}

	util_message_string(chp, "firmware_version", GIT_COMMIT_VERSION);
	util_message_hex_uint32(chp, "chip_id", chip_id, 3);
	util_message_string(chp, "kernel", CH_KERNEL_VERSION);



	util_message_string(chp, "compiler", PORT_COMPILER_NAME);
	util_message_string(chp, "architecture", PORT_ARCHITECTURE_NAME);
	util_message_string(chp, "core_variant", PORT_CORE_VARIANT_NAME);
	util_message_string(chp, "port_info", PORT_INFO);
	util_message_string(chp, "platform", PLATFORM_NAME);
	util_message_string(chp, "board", BOARD_NAME);
	util_message_string(chp, "build_date", __DATE__);
	util_message_string(chp, "build_time", __TIME__);
  return true;
}

/*! \brief systicks (1mS default) since reboot
 */
static bool cmd_systime(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'systime'");
		return false;
	}

  uint32_t time_value = chVTGetSystemTime();
	util_message_uint32(chp, "systime", &time_value, 1);
  return true;
}

/*! \brief reset shell options to defaults
 */
static bool cmd_reset(BaseSequentialStream * chp, int argc, char * argv[] UNUSED)
{
	if (argc > 0)
	{
		util_message_error(chp, "extra arguments for command 'reset'");
		return false;
	}
	setMShellVisiblePrompt(true);
	setMShellPrompt("m > ");
	mshell_echo_chars = true;
  return true;
}

/**
 * @brief   Array of the default commands.
 */
static mshell_command_t local_commands[] =
{
	{cmd_info,      "info",     "Query system info"},
	{cmd_systime,   "systime",  "Query system time"},
	{cmd_prompt,    "prompt",   "Enable shell prompt"},
	{cmd_noprompt,  "noprompt", "Disable shell prompt"},
	{cmd_echo,      "echo",     "Enable shell echo"},
	{cmd_noecho,    "noecho",   "Disable shell echo"},
  {cmd_reset,     "reset",    "Reset shell to defaults"},
	{NULL, NULL, NULL}
};

/*! \brief find comand in list
 */
static int mshell_find_cmd(const mshell_command_t * scp, char * name)
{
  for( int i = 0; scp[i].sc_name != NULL; i++ )
  {
		if (strcasecmp(scp[i].sc_name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

static bool mshell_parse(BaseSequentialStream* chp, const mshell_command_t * scp, char * inputline)
{
	int argc, index;
	char command_line[MSHELL_MAX_LINE_LENGTH];
	char * argv[MSHELL_MAX_ARGUMENTS + 1];
	char * lp, *cmd, *tokp;

  if( inputline == NULL )
  {
    return false;
  }

  // copy to a local buffer to modify
  _strncpy(command_line, inputline, MSHELL_MAX_LINE_LENGTH);

  lp = _strtok(command_line, " ", &tokp);
  cmd = lp;
  argc = 0;
  while ((lp = _strtok(NULL, " ", &tokp)) != NULL)
  {
    if (argc >= MSHELL_MAX_ARGUMENTS)
    {
      util_message_error(chp, "too many arguments");
      return false;
    }
    argv[argc++] = lp;
  }
  argv[argc] = NULL;

  if(strcasecmp(cmd, "exit") == 0)
  {
    util_message_error(chp, "exit");
    return true;
  }
  else if(strcasecmp(cmd, "help") == 0)
  {
    util_message_info(chp, "Marionette Shell Commands:");
    util_message_info(chp, "+help");
    util_message_info(chp, "\tList shell commands");
    list_commands(chp, local_commands);
    if (scp != NULL)
    {
      list_commands(chp, scp);
    }
    return true;
  }
  else if( (index = mshell_find_cmd(local_commands, cmd)) >= 0 )
  {
    return (*local_commands[index].sc_function) (chp, argc, argv);
  }
  else if( scp != NULL && (index = mshell_find_cmd(scp, cmd)) >= 0 )
  {
    return (*scp[index].sc_function) (chp, argc, argv);
  }
  else
  {
    util_message_error(chp, "Invalid shell command '%s'", cmd);
    return false;
  }
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
 * @retval MSG_OK       terminated by command.
 * @retval MSG_RESET    terminated by reset condition on the I/O channel.
 *
 */
static void mshell_thread(void * p)
{
	bool  ret; 
	BaseSequentialStream * chp   = ((MShellConfig *)p)->sc_channel;
	const mshell_command_t  * scp   = ((MShellConfig *)p)->sc_commands;
	static char input_line[MSHELL_MAX_LINE_LENGTH];

	setMShellStreamPtr(chp);
	setMShellPrompt("m > ");
	setMShellVisiblePrompt(true);
	chRegSetThreadName("mshell");
	chThdSleepMilliseconds(500);

	//! Initial Welcome Prompt
  chprintf(chp, "\r\n");
	util_message_info(getMShellStreamPtr(), "Marionette Shell (\"help\" or \"+help\" for commands)");

	//! initialize parser.
	fetch_init(chp) ;

	while (true)
	{
		mshell_putprompt();
		ret = mshellGetLine(chp, input_line, sizeof(input_line));
		if (ret)
		{
      chprintf(chp, "\r\n");
			util_message_error(chp, "exit");
			break; // exit function
		}

    if( input_line[0] == '\0' )
    {
      continue;
    }

    util_message_begin(chp);

		if(input_line[0] == '+' || input_line[0] == '.')    // use escape to process mshell commands
		{
      util_message_end(chp, mshell_parse(chp, scp, &input_line[1]) );
		}
		else
		{
			util_message_end(chp, fetch_parse(chp, input_line) );
		}
	}

	mshellExit(MSG_OK);
}


/**
 * @brief   MShell manager initialization.
 *
 * @api
 */
void mshellInit()
{
	chEvtObjectInit(&mshell_terminated);
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
#if CH_CFG_USE_HEAP && CH_CFG_USE_DYNAMIC
thread_t * mshellCreate(const MShellConfig * scp, size_t size, tprio_t prio)
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
thread_t * shellCreateStatic(const MShellConfig * scp, void * wsp,
                           size_t size, tprio_t prio)
{
	return chThdCreateStatic(wsp, size, prio, mshell_thread, (void *)scp);
}

/*!
 * \brief   Reads a whole line from the input channel.
 *
 * \param[in] chp       pointer to a \p BaseSequentialStream object
 * \param[in] line      pointer to the line buffer
 * \param[in] size      buffer maximum length
 * \return              The operation status.
 * \retval true         the channel was reset or CTRL-D pressed.
 * \retval false        operation successful.
 *
 */
#define        ASCII_EOT                        ((char) 0x4)
#define        ASCII_BACKSPACE                  ((char) 0x8)
#define        ASCII_DELETE                     ((char) 0x7F)
#define        ASCII_CTL_U                      ((char) 0x15)
#define        ASCII_SPACE                      ((char) 0x20)
bool mshellGetLine(BaseSequentialStream * chp, char * line, unsigned size)

{
	char * p = line;
	while (true)
	{
		char c;
		if (chSequentialStreamRead(chp, (uint8_t *)&c, 1) == 0)
		{
			return true;
		}
		if (c == ASCII_EOT)
		{
			if(mshell_echo_chars)
			{
				chprintf(chp, "^D");
			}
			return true;
		}
		if ((c == ASCII_CTL_U))
		{
			while (p != line)
			{
				if(mshell_echo_chars)
				{
          // FIXME why is this sent the way it is?
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
          // FIXME why is this sent the way it is?
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
			*p = '\0';
			return false;
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

