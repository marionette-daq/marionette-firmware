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

#ifndef MSHELL_WA_SIZE
#define MSHELL_WA_SIZE 16384
#endif

#ifndef MSHELL_PRIO
#define MSHELL_PRIO HIGHPRIO
#endif

static THD_WORKING_AREA(mshell_wa, MSHELL_WA_SIZE);

static thread_t * mshell_tp = NULL;

static mshell_config_t mshell_config;


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
  mshell_config.show_prompt = true;
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
	mshell_config.show_prompt = false;
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
	mshell_config.echo_chars = true;
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
	mshell_config.echo_chars = false;
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

	util_message_string_format(chp, "firmware_version", GIT_COMMIT_VERSION);
	util_message_hex_uint32_array(chp, "chip_id", chip_id, 3);
	util_message_string_format(chp, "kernel", CH_KERNEL_VERSION);



	util_message_string_format(chp, "compiler", PORT_COMPILER_NAME);
	util_message_string_format(chp, "architecture", PORT_ARCHITECTURE_NAME);
	util_message_string_format(chp, "core_variant", PORT_CORE_VARIANT_NAME);
	util_message_string_format(chp, "port_info", PORT_INFO);
	util_message_string_format(chp, "platform", PLATFORM_NAME);
	util_message_string_format(chp, "board", BOARD_NAME);
	util_message_string_format(chp, "build_date", __DATE__);
	util_message_string_format(chp, "build_time", __TIME__);
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
	util_message_uint32(chp, "systime", time_value);
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
  mshell_config.show_prompt = true;
	mshell_config.echo_chars = true;
  return true;
}

/**
 * @brief   Array of the default commands.
 */
static mshell_command_t local_commands[] =
{
	{cmd_info,      "info",       "Query system info"},
	{cmd_systime,   "systime",    "Query system time"},
	{cmd_prompt,    "prompt",     "Enable shell prompt"},
	{cmd_noprompt,  "noprompt",   "Disable shell prompt"},
	{cmd_noprompt,  "no_prompt",  NULL},
	{cmd_echo,      "echo",       "Enable shell echo"},
	{cmd_noecho,    "noecho",     "Disable shell echo"},
	{cmd_noecho,    "no_echo",    NULL},
  {cmd_reset,     "reset",      "Reset shell to defaults"},
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
 * Fetch commands are parsed here through the call to fetch_execute()
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
  BaseAsynchronousChannel * channel = mshell_config.channel;
	BaseSequentialStream * stream = (BaseSequentialStream*)mshell_config.channel;
  static char input_line[MSHELL_MAX_LINE_LENGTH];

	chRegSetThreadName("mshell");
	chThdSleepMilliseconds(500); // FIXME do we need this and does it need to be this long?

  chprintf(stream, "\r\n");
	util_message_info(stream, MSHELL_WELCOME_STR);

  while(!chThdShouldTerminateX())
	{
    if( mshell_config.show_prompt )
    {
      chprintf(stream, mshell_config.prompt);
    }

    // FIXME change this to handle each return code, eg. break, exit, etc.
		if( mshell_get_line(channel, input_line, sizeof(input_line), mshell_config.echo_chars) != MSHELL_MSG_OK )
    {
      chprintf(stream, "\r\n");
			util_message_warning(stream, "exit mshell thread");
			break; // exit function
		}

    // ignore empty input lines
    if( input_line[0] == '\0' )
    {
      continue;
    }

    util_message_begin(stream);

		if(input_line[0] == '+' || input_line[0] == '.')    // use escape to process mshell commands
		{
      util_message_end(stream, mshell_parse(stream, mshell_config.commands, &input_line[1]) );
		}
		else // all other commands are passed to fetch
		{
			util_message_end(stream, fetch_execute(stream, input_line) );
		}
	}

  chThdExit(MSG_OK);
}


/**
 * @brief   MShell manager initialization.
 *
 * @api
 */
void mshell_init()
{
  mshell_sync_init();
}


void mshell_start( const mshell_config_t * cfg )
{
  // start mshell thread if not already active
  if( mshell_tp == NULL || chThdTerminatedX(mshell_tp) )
  {
    mshell_config = *cfg;

	  mshell_tp = chThdCreateStatic(mshell_wa, sizeof(mshell_wa), MSHELL_PRIO, mshell_thread, NULL);
  }
}


void mshell_stop(void)
{
  if( mshell_tp )
  {
    chThdTerminate(mshell_tp);
    chThdWait(mshell_tp);
    mshell_tp = NULL;
  }
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
#define ASCII_CTRL_C      ((char) 0x03)
#define ASCII_CTRL_D      ((char) 0x04)
#define ASCII_BACKSPACE   ((char) 0x08)
#define ASCII_DELETE      ((char) 0x7F)
#define ASCII_CTRL_U      ((char) 0x15)
#define ASCII_SPACE       ((char) 0x20)

mshell_msg_t mshell_get_line(BaseAsynchronousChannel * channel, char * line, unsigned size, bool echo_chars )
{
	char * p = line;
  BaseSequentialStream * stream = (BaseSequentialStream *)channel;

	while( !chThdShouldTerminateX() )
	{
		char c;

    // Read a single character from the input stream, timeout so we can check if the thread should terminate
		if( chnReadTimeout(channel, (uint8_t *)&c, 1, MS2ST(100)) == 0 ) 
		{
			continue;
		}

    // DEBUG lets spit stuff out to see what is received at initial connection
    chprintf(DEBUG_CHP, "%c", c);

    // Ctrl+D: return true indicating we should exit mshell
		if( c == ASCII_CTRL_D )
		{
			return MSHELL_MSG_EXIT;
		}
    
    // Ctrl+C: this allows the user to signal a break at any time
    if( c == ASCII_CTRL_C )
    {
      return MSHELL_MSG_BREAK;
    }

    // Ctrl+U: clear entire line
		if( (c == ASCII_CTRL_U) )
		{
			while( p != line )
			{
				if( echo_chars )
				{
          // FIXME why is this sent the way it is?
					streamPut(stream, ASCII_BACKSPACE);
					streamPut(stream, ASCII_SPACE);
					streamPut(stream, ASCII_BACKSPACE);
				}
				p--;
			}
			continue;
		}

    // Backspase & Delete keys: delete previous character
		if( (c == ASCII_BACKSPACE) || (c == ASCII_DELETE) )
		{
			if( p != line )
			{
				if( echo_chars )
				{
          // FIXME why is this sent the way it is?
					streamPut(stream, c);
					streamPut(stream, ASCII_SPACE);
					streamPut(stream, c);
				}
				p--;
			}
			continue;
		}

    // NL & CR: terminate line and return false indicating no errors
    // Note: one side effect is that a \r\n sequence will trigger an empty line
		if( c == '\r' || c == '\n' )
		{
			if( echo_chars )
			{
				chprintf(stream, "\r\n");
			}
			*p = '\0';
			return MSHELL_MSG_OK;
		}

    // ignore all other special chars below value 32 (space)
		if( c < ASCII_SPACE )
		{
			continue;
		}

    // append character to line buffer if there is space
		if( p < (line + size - 1) )
		{
			if( echo_chars )
			{
        streamPut(stream, c);
			}
			*p++ = (char)c;
		}
	}

  // thread should terminate
  return MSHELL_MSG_EXIT;
}


/** @} */

