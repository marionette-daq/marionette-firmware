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
#include "util_io.h"
#include "fetch_gpio.h"
#include "fetch_adc.h"
#include "fetch_dac.h"
#include "fetch_spi.h"
#include "fetch_i2c.h"
#include "fetch_mbus.h"
#include "fetch_sd.h"
#include "fetch_timer.h"

#include "fetch_defs.h"
#include "fetch.h"

static bool fetch_help_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_reset_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_version_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_chip_id_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_test_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_clocks_cmd(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);

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
    { fetch_chip_id_cmd,        "chip_id",          "Return unique cpu chip id" },
    { fetch_chip_id_cmd,        "chipid",           NULL },
    { fetch_gpio_dispatch,      "gpio",             "GPIO command set\n(see gpio.help)" },
    { fetch_adc_dispatch,       "adc",              "ADC command set\n(see adc.help)" },
    { fetch_dac_dispatch,       "dac",              "DAC command set\n(see dac.help)" },
    { fetch_spi_dispatch,       "spi",              "SPI command set\n(see spi.help)" },
    { fetch_i2c_dispatch,       "i2c",              "I2C command set\n(see i2c.help)" },
    { fetch_mbus_dispatch,      "mbus",             "MBUS command set\n(see mbus.help)" },
    { fetch_sd_dispatch,        "sd",               "SD command set\n(see sd.help)" },
    { fetch_timer_dispatch,     "timer",            "TIMER command set\n(see timer.help)" },
    { fetch_test_cmd,           "test",             NULL },
    { fetch_clocks_cmd,         "clocks",           "display info about internal clocks"},
    { NULL, NULL, NULL }
  };

// used to store the number of data tokens so that we dont have to iterate the list to determine its size
// used in fetch_input_check()
static uint32_t fetch_cmd_count = 0;
static uint32_t fetch_data_count = 0;

/*!
 * Here is the theory on why using static inline here might be useful.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Inline.html
 *
 */

static bool fetch_clocks_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  util_message_info(chp, "SYSCLK  = %4u MHz", STM32_SYSCLK/1000000);
  util_message_info(chp, "PLLVCO  = %4u MHz", STM32_PLLVCO/1000000);
  util_message_info(chp, "HCLK    = %4u MHz", STM32_HCLK/1000000);
  
  util_message_info(chp, "PCLK1   = %4u MHz", STM32_PCLK1/1000000);
  util_message_info(chp, "PCLK2   = %4u MHz", STM32_PCLK2/1000000);

  util_message_info(chp, "TIMCLK1 = %4u MHz", STM32_TIMCLK1/1000000);
  util_message_info(chp, "TIMCLK2 = %4u MHz", STM32_TIMCLK2/1000000);
  util_message_info(chp, "ADCCLK  = %4u MHz", STM32_ADCCLK/1000000);

#if STM32_GPT_USE_TIM1
  util_message_info(chp, "GPTD1.clock  = %4u Mhz", GPTD1.clock/1000000);
#endif
#if STM32_GPT_USE_TIM2
  util_message_info(chp, "GPTD2.clock  = %4u Mhz", GPTD2.clock/1000000);
#endif
#if STM32_GPT_USE_TIM3
  util_message_info(chp, "GPTD3.clock  = %4u Mhz", GPTD3.clock/1000000);
#endif
#if STM32_GPT_USE_TIM4
  util_message_info(chp, "GPTD4.clock  = %4u Mhz", GPTD4.clock/1000000);
#endif
#if STM32_GPT_USE_TIM5
  util_message_info(chp, "GPTD5.clock  = %4u Mhz", GPTD5.clock/1000000);
#endif
#if STM32_GPT_USE_TIM6
  util_message_info(chp, "GPTD6.clock  = %4u Mhz", GPTD6.clock/1000000);
#endif
#if STM32_GPT_USE_TIM7
  util_message_info(chp, "GPTD7.clock  = %4u Mhz", GPTD7.clock/1000000);
#endif
#if STM32_GPT_USE_TIM8
  util_message_info(chp, "GPTD8.clock  = %4u Mhz", GPTD8.clock/1000000);
#endif
#if STM32_GPT_USE_TIM9
  util_message_info(chp, "GPTD9.clock  = %4u Mhz", GPTD9.clock/1000000);
#endif
#if STM32_GPT_USE_TIM10
  util_message_info(chp, "GPTD10.clock = %4u Mhz", GPTD10.clock/1000000);
#endif
#if STM32_GPT_USE_TIM11
  util_message_info(chp, "GPTD11.clock = %4u Mhz", GPTD11.clock/1000000);
#endif
#if STM32_GPT_USE_TIM12
  util_message_info(chp, "GPTD12.clock = %4u Mhz", GPTD12.clock/1000000);
#endif
#if STM32_GPT_USE_TIM13
  util_message_info(chp, "GPTD13.clock = %4u Mhz", GPTD13.clock/1000000);
#endif
#if STM32_GPT_USE_TIM14
  util_message_info(chp, "GPTD14.clock = %4u Mhz", GPTD14.clock/1000000);
#endif

  return true;
}

/*! test fetch command format by printing cmd_list and data_list
 */
static bool fetch_test_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "data_count %U", fetch_data_count);

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
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  util_message_info(chp, "+help\n\tDislay shell help");
  fetch_display_help(chp, fetch_commands);
	return true;
}

/*! \brief VERSION command callback for fetch language
 */
static bool fetch_version_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

	util_message_string(chp, "firmware_version", GIT_COMMIT_VERSION);

	return true;
}

/*! \brief CHIPID command callback for fetch language
 */
static bool fetch_chip_id_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  uint32_t chip_id[3] = {0,0,0};
  
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  chip_id[0] = *(uint32_t*)STM32F4_UNIQUE_ID_LOW;
  chip_id[1] = *(uint32_t*)STM32F4_UNIQUE_ID_CENTER;
  chip_id[2] = *(uint32_t*)STM32F4_UNIQUE_ID_HIGH;

	util_message_hex_uint32_array(chp, "chip_id", chip_id, 3);
	return true;
}

/*! \brief RESET command callback for fetch language
 */
static bool fetch_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // Add any new peripheral reset functions here
  fetch_adc_reset(chp);
  fetch_dac_reset(chp);
  fetch_spi_reset(chp);
  fetch_i2c_reset(chp);
  fetch_gpio_reset(chp);
  fetch_mbus_reset(chp);
  fetch_sd_reset(chp);
  fetch_timer_reset(chp);

  // make sure all pin assignments are set to defaults
  // ~not needed at the moment~
	//palInit(&pal_default_config);

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
  fetch_mbus_init(chp);
  fetch_sd_init(chp);
  fetch_timer_init(chp);
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
bool fetch_parse( BaseSequentialStream * chp, char * inputline )
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
  uint32_t cmd_count = 0;
  uint32_t data_count = 0;

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
		fetch_remove_whitespace( commandstr );
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

  cmd_count = 0;
  lp = _strtok(commandstr, FETCH_CMD_DELIM, &tokp);
  if( lp != NULL )
  {
    command_toks[cmd_count++] = lp;

    while ((lp = _strtok(NULL, FETCH_CMD_DELIM, &tokp)) != NULL)
    {
      if( cmd_count >= FETCH_MAX_COMMANDS )
      {
        util_message_error(chp, "Too many commands. Limit: %u", FETCH_MAX_COMMANDS);
        return false;
      }
      else
      {
        command_toks[cmd_count++] = lp;
      }
    }
  }
  command_toks[cmd_count] = NULL;

  data_count = 0;
	if(parenpart != NULL)
	{
		/* break data into array of strings */
		lp  = _strtok(datastr, FETCH_DATA_DELIM, &tokp);
    if( lp != NULL )
    {
      data_toks[data_count++] = lp;

      while ((lp = _strtok(NULL, FETCH_DATA_DELIM, &tokp)) != NULL)
      {
        if( data_count >= FETCH_MAX_DATA_ITEMS)
        {
          util_message_error(chp, "Too many data items. Limit: %u", FETCH_MAX_DATA_ITEMS);
          return false;
        }
        else
        {
          data_toks[data_count++] = lp;
        }
      }
    }
	}
  data_toks[data_count] = NULL;

  // ignore empty command string
  if( command_toks[FETCH_TOK_CMD] == NULL )
  {
    return true;
  }

  // save data count for use in fetch_input_check()
  fetch_cmd_count = cmd_count;
  fetch_data_count = data_count;

  // fetch_dispatch handles command/data we just parsed
	return fetch_dispatch(chp, fetch_commands, command_toks, data_toks);
}

/*! \brief dispatch the commands from here
 * \param[in] cmd_list A list of colon separated command strings is in the command_list
 * \param[in] data_list    A list of space separated data is in data_list
 *
 */
bool fetch_dispatch(BaseSequentialStream * chp, fetch_command_t cmd_lookup_table[], char * cmd_list[], char * data_list[])
{
  chDbgAssert(cmd_lookup_table, "cmd_lookup_table == NULL");
  chDbgAssert(cmd_list, "command_list == NULL");
  chDbgAssert(data_list, "data_list == NULL");

  int index = fetch_find_command(cmd_lookup_table, cmd_list[0]);

  if( index == FETCH_COMMAND_NOT_FOUND )
  {
    util_message_error(chp, "Invalid fetch command");
    return false;
  }
  else if( cmd_lookup_table[index].function == NULL )
  {
    util_message_error(chp, "Command not implemented");
    return false;
  }
  else
  {
    // head of cmd_list will now point to next cmd
    // no more sub commands exist if cmd_list[0] == NULL
    return ( (*cmd_lookup_table[index].function) ( chp, &cmd_list[1], data_list ) );
  }
}

void fetch_display_help_legend(BaseSequentialStream * chp)
{
  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");
}

void fetch_display_help(BaseSequentialStream * chp, fetch_command_t cmd_lookup_table[])
{
  static char help_str[FETCH_MAX_HELP_STRLEN];
  char * tokptr;
  char * saveptr;

  chDbgAssert(cmd_lookup_table, "cmd_lookup_table == NULL");

  for( int index = 0; cmd_lookup_table[index].command != NULL; index++ )
  {
    if( cmd_lookup_table[index].help != NULL )
    {
      // copy string then print each line
      _strncpy(help_str, cmd_lookup_table[index].help, FETCH_MAX_HELP_STRLEN);

      util_message_info(chp, "%s", cmd_lookup_table[index].command);

      tokptr = _strtok(help_str, "\n", &saveptr);
      do
      {
        util_message_info(chp, "\t%s", tokptr);
      }
      while( (tokptr = _strtok(NULL, "\n", &saveptr)) != NULL );
    }
  }
}

int fetch_find_command(fetch_command_t cmd_lookup_table[], char * command)
{
  chDbgAssert(cmd_lookup_table, "cmd_lookup_table == NULL");

  if( command == NULL )
  {
    return FETCH_COMMAND_NOT_FOUND;
  }

  /* loop until we encounter the null item in the list */
  for( int index = 0; cmd_lookup_table[index].command != NULL; index++ )
  {
    if( strncasecmp(command, cmd_lookup_table[index].command, FETCH_MAX_CMD_STRLEN) == 0 )
    {
      return index;
    }
  }

  return FETCH_COMMAND_NOT_FOUND;
}

bool fetch_param_check( BaseSequentialStream * chp, char * cmd_list[], uint32_t min_data, uint32_t max_data )
{
  if( cmd_list != NULL && cmd_list[0] != NULL)
  {
    util_message_error(chp, "invalid sub command");
    return false;
  }

  if( min_data > fetch_data_count )
  {
    util_message_error(chp, "missing arguments, %U > %U", min_data, fetch_data_count);
    return false;
  }

  if( max_data < fetch_data_count )
  {
    util_message_error(chp, "too many arguments, %U < %U", max_data, fetch_data_count);
    return false;
  }

  return true;
}

/*! @} */

