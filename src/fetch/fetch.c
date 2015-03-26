/*! \file fetch.c
 *  \subpage BNF 
 *  Fetch: A DSL for Marionette interaction.
 * @defgroup fetch Fetch DSL
 * @{
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "io_manage.h"
#include "io_manage_defs.h"
#include "fetch_gpio.h"
#include "fetch_adc.h"
#include "fetch_dac.h"
#include "fetch_spi.h"
#include "fetch_i2c.h"
#include "fetch_can.h"

#include "fetch_defs.h"
#include "fetch.h"

static bool fetch_help_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_reset_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_version_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_chip_id_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_test_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
/* Testing can bus */
/*static bool fetch_can(BaseSequentialStream *chp, char * cmd_list[], char * data_list[]);*/

/*! \brief Function command array for fetch_dispatch() callbacks
 *  Commands with NULL function return as not implemented.
 *  Commands with NULL help are not displayed with the help command.
 *  List MUST be NULL terminated. List parsing stops at first NULL command string.
 *
 *  Command names will have all whitespace and '_' parsed out before comparison.
 */
static fetch_command_t fetch_commands[] = {
  /*  function                  command string      help string */
    { fetch_help_cmd,           "?",                NULL },
    { fetch_help_cmd,           "help",             "Display fetch command help" },
    { fetch_reset_cmd,          "reset",            "Reset all peripherals and pins" },
    { fetch_version_cmd,        "version",          "Version information" },
    { fetch_chip_id_cmd,        "chipid",           "Return unique cpu chip id" },
    { fetch_gpio_dispatch,      "gpio",             "GPIO command set\n(see gpio.help)" },
    { fetch_adc_dispatch,       "adc",              "ADC command set\n(see adc.help)" },
    { fetch_dac_dispatch,       "dac",              "DAC command set\n(see dac.help)" },
    { fetch_spi_dispatch,       "spi",              "SPI command set\n(see spi.help)" },
    { fetch_i2c_dispatch,       "i2c",              "I2C command set\n(see i2c.help)" },
    { fetch_can_dispatch,	"can",		    "CAN command set\n(see can.help)"},
    { fetch_test_cmd,           "test",             NULL },
    { NULL, NULL, NULL }
  };

/*!
 * Here is the theory on why using static inline here might be useful.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Inline.html
 *
 */

/*! test fetch command format by printing cmd_list and data_list
 */
static bool fetch_test_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  for( int i = 0; cmd_list[i] != NULL; i++ )
  {
    util_message_info(chp, "cmd '%s'", cmd_list[i]);
  }
  
  for( int i = 0; data_list[i] != NULL; i++ )
  {
    util_message_info(chp, "data '%s'", data_list[i]);
  }

	return true;
}

/*! Help command callback for fetch language
 */
static bool fetch_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_CMD, data_list, 0) )
  {
    return false;
  }

  util_message_info(chp, "+help\n\tDislay shell help");
  fetch_display_help(chp, fetch_commands);
	return true;
}

/*! \brief VERSION command callback for fetch language
 */
static bool fetch_version_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_CMD, data_list, 0) )
  {
    return false;
  }

	util_message_string(chp, "firmware_version", GIT_COMMIT_VERSION);
	return true;
}

/*! \brief CHIPID command callback for fetch language
 */
static bool fetch_chip_id_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  uint32_t chip_id[3] = {0,0,0};
  
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_CMD, data_list, 0) )
  {
    return false;
  }

  chip_id[0] = *(uint32_t*)STM32F4_UNIQUE_ID_LOW;
  chip_id[1] = *(uint32_t*)STM32F4_UNIQUE_ID_CENTER;
  chip_id[2] = *(uint32_t*)STM32F4_UNIQUE_ID_HIGH;

	util_message_hex_uint32(chp, "chip_id", chip_id, 3);
	return true;
}

/*! \brief RESET command callback for fetch language
 */
static bool fetch_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_CMD, data_list, 0) )
  {
    return false;
  }

  // Add any new peripheral reset functions here
  fetch_adc_reset(chp);
  fetch_dac_reset(chp);
  fetch_spi_reset(chp);
  fetch_i2c_reset(chp);
  fetch_gpio_reset(chp);
  fetch_can_reset(chp); 

  // make sure all pin assignments are set to defaults
	palInit(&pal_default_config);
	io_manage_set_all_to_defaults();

	return true;
}

/*! \brief Fetch initialization 
 * 
 * This needs to be called before parsing fetch commands
 */
void fetch_init(BaseSequentialStream * chp)
{
  fetch_gpio_init(chp);
	fetch_adc_init(chp);
  fetch_dac_init(chp);
  fetch_spi_init(chp);
  fetch_i2c_init(chp);
  fetch_can_init(chp); 
}

/*! \brief parse the Fetch Statement
 * 
 * \param[in] inputline string input 
 *
 * General idea:
 *
 * Break the input line into two parts:
 *
 * Everything to the left of a '(' is an instruction
 * Everything to the right of a '(' is data for that instruction
 * 
 * Two lists of string tokens are created
 * 		a command token array
 * 		a data token array
 * 		These are passed to the dispatch function
 *
 *  Dispatch the commands in \sa fetch_dispatch()
 */
bool fetch_parse(BaseSequentialStream * chp, char * inputline)
{
	static char   localinput[ FETCH_MAX_LINE_CHARS ];
	static char   commandstr[ FETCH_MAX_LINE_CHARS ];
	static char   datastr[ FETCH_MAX_LINE_CHARS ];

  // add one to guarentee space for null at end of list
	static char * command_toks[ FETCH_MAX_COMMANDS + 1 ];
	static char * data_toks[ FETCH_MAX_DATA_ITEMS + 1 ];

	char * lp;
	char * tokp;
	char * colonpart;
	char * parenpart;
	int n = 0;

  // NOTE: we use our version of strncpy that guarentees a null terminated string
  //       along with only copying up to n chars or the first NULL

	// don't mess up the string passed from outside this function
	_strncpy( localinput, inputline, FETCH_MAX_LINE_CHARS );


  //DBG_VMSG(chp, "'%s'", localinput);

	// break up into two strings the colons part and the parens part
  colonpart = _strtok(localinput, "(", &tokp);
  parenpart = _strtok(NULL, "(", &tokp);

  // copy parts to commandstr and datastr, cleaning up spaces
	if(colonpart != NULL)
	{
		_strncpy(commandstr, colonpart, FETCH_MAX_LINE_CHARS);
		fetch_remove_spaces( commandstr );
		if(parenpart != NULL)
		{
			_strncpy(datastr, parenpart, FETCH_MAX_LINE_CHARS);

      // remove trailing ')' if it exists
      int datastr_end = strlen(datastr) - 1;
      if( datastr_end >= 0 && datastr[datastr_end] == ')' )
      {
			  datastr[datastr_end] = '\0';
      }
      else
      {
        //DBG_VMSG(chp, "'%s'[%d]", datastr, datastr_end);
        util_message_error(chp, "Missing closing ')'");
        return false;
      }
		}
	}
	else
	{
		// empty line
		return true;
	}

	/* break commandstr into array of strings */

  n = 0;
  lp = _strtok(commandstr, FETCH_CMD_DELIM, &tokp);
  command_toks[n++] = lp;
  command_toks[n] = NULL;

	while ((lp = _strtok(NULL, FETCH_CMD_DELIM, &tokp)) != NULL)
  {
    if( n >= FETCH_MAX_COMMANDS )
    {
			util_message_error(chp, "Too many commands. Limit: %u", FETCH_MAX_COMMANDS);
      return false;
    }
    else
    {
      command_toks[n++] = lp;
      command_toks[n] = NULL;
    }
  }

	if(parenpart != NULL)
	{
		/* break data into array of strings */

		n   = 0;
		lp  = _strtok(datastr, FETCH_DATA_DELIM, &tokp);
		data_toks[n++] = lp;
    data_toks[n] = NULL;

		while ((lp = _strtok(NULL, FETCH_DATA_DELIM, &tokp)) != NULL)
		{
			if( n >= FETCH_MAX_DATA_ITEMS)
			{
				util_message_error(chp, "Too many data items. Limit: %u", FETCH_MAX_DATA_ITEMS);
        return false;
			}
      else
			{
				data_toks[n++] = lp;
        data_toks[n] = NULL;
			}
		}
	}
	else
	{
		data_toks[0] = NULL;
	}

  // ignore empty command string
  if( command_toks[FETCH_TOK_CMD] == NULL )
  {
    return true;
  }

  // fetch_dispatch handles command/data we just parsed
	return fetch_dispatch(chp, fetch_commands, command_toks[FETCH_TOK_CMD], command_toks, data_toks);
}

/*! \brief dispatch the commands from here
 * \param[in] command_list A list of colon separated command strings is in the command_list
 * \param[in] data_list    A list of space separated data is in data_list
 *
 * \warning data_list[0] may be null. Not all commands accept data.
 *
 */
bool fetch_dispatch(BaseSequentialStream * chp, fetch_command_t cmd_fn[], char * command, char * command_list[], char * data_list[])
{
  int index = fetch_find_command(cmd_fn, command);

  if( command == NULL || cmd_fn == NULL )
  {
    util_message_error(chp, "NULL pointer");
    return false;
  }
  else if( index == FETCH_COMMAND_NOT_FOUND )
  {
    util_message_error(chp, "Invalid fetch command");
    return false;
  }
  else if( cmd_fn[index].function == NULL )
  {
    util_message_error(chp, "Command not implemented");
    return false;
  }
  else
  {
    return ( (*cmd_fn[index].function) ( chp, command_list, data_list ) );
  }
}

void fetch_display_help(BaseSequentialStream * chp, fetch_command_t cmd_fn[])
{
  static char help_str[FETCH_MAX_HELP_STRLEN];
  char * tokptr;
  char * saveptr;


  for( int index = 0; cmd_fn[index].command != NULL; index++ )
  {
    if( cmd_fn[index].help != NULL )
    {
      // copy string then print each line
      _strncpy(help_str, cmd_fn[index].help, FETCH_MAX_HELP_STRLEN);

      util_message_info(chp, "%s", cmd_fn[index].command);

      tokptr = _strtok(help_str, "\n", &saveptr);
      do
      {
        util_message_info(chp, "\t%s", tokptr);
      }
      while( (tokptr = _strtok(NULL, "\n", &saveptr)) != NULL );
    }
  }
}

int fetch_find_command(fetch_command_t cmd_fn[], char * command)
{
  if( command == NULL )
  {
    return FETCH_COMMAND_NOT_FOUND;
  }

  /* loop until we encounter the null item in the list */
  for( int index = 0; cmd_fn[index].command != NULL; index++ )
  {
    if( strncasecmp(command, cmd_fn[index].command, FETCH_MAX_CMD_STRLEN) == 0 )
    {
      return index;
    }
  }

  return FETCH_COMMAND_NOT_FOUND;
}

bool fetch_input_check( BaseSequentialStream * chp, char * cmd_list[], uint32_t max_cmd, char * data_list[], uint32_t max_data )
{
  uint32_t count;

  for( count = 0; count < FETCH_MAX_COMMANDS && cmd_list[count] != NULL; count++ );

  // max_cmd is a zero based index not a count
  if( count > (max_cmd+1) )
  {
    util_message_error(chp, "invalid command");
    return false;
  }
  
  for( count = 0; count < FETCH_MAX_DATA_ITEMS && data_list[count] != NULL; count++ );

  // max_data is the count of data tokens
  if( count > max_data )
  {
    util_message_error(chp, "too many arguments");
    return false;
  }

  return true;
}

/*! @} */
