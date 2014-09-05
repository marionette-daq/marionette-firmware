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
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "util_led.h"
#include "io_manage.h"
#include "fetch_gpio.h"
#include "fetch_adc.h"
#include "fetch_dac.h"
#include "fetch_spi.h"

#include "fetch_defs.h"
#include "fetch.h"

/*! \todo Mon 14 July 2014 11:17:26 (PDT)  Investigate FLEZ/BISON/YACC etc. when definition settles
 */

/*! \page BNF Fetch Language Grammar (BNF)

\verbatim

(N,∑,P,S)

N - Non-terminals: { <statement>, <command>, <datastr>, <byte>, <constant> }
∑ - Terminals:     { <command>, <gpio_subcommandA>, <gpio_direction>, <gpio_sense>, <adc_configure>, <adc_profile>, <dac_subcommandA>, <dac_configure>, <dac_channel>, <port_subcommand>, <pin_subcommand>, <subcommandD>, <digit>, <EOL>, <whitespace>}
P - Production Rules:

<statement>        ::= <command> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> ":" <gpio_direction> ":" <gpio_sense> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> ":" <gpio_direction> <EOL>
                     | <command> ":" <adc_subcommandA> <EOL>
                     | <command> ":" <adc_subcommandA>  ":" <adc_configure> <EOL>
                     | <command> ":" <adc_subcommandA>  ":" <adc_configure>   ":" <adc_profile> <EOL>
                     | <command> ":" <adc_subcommandA>  ":" <adc_configure>(<dec>) <EOL>
                     | <command> ":" <dac_subcommandA>  <EOL>
                     | <command> ":" <dac_subcommandA> (<dec>) <EOL>
                     | <command> ":" <dac_subcommandA>  ":" <dac_configure>(<dec>) <EOL>
<command>          ::= "?"      | "help"     | "gpio"  | "adc"   | "dac"   | "spi" | "i2c" | "resetpins" | "heartbeat_toggle" | "version"
<adc_subcommandA>  ::= "conf_adc1" | "start" | "stop"
<adc_configure>    ::= "profile" | "oneshot" | "continuous" | "reset" | "vref_mv"
<adc_profile>      ::= "default" | "PA"   | "PB"
<dac_subcommandA>  ::= "start"      | "stop" | "conf_ch1" | "conf_ch2" | "reset" | "release" | "vref_mv"
<dac_configure>    ::= "dc_mv"
<spi_subcommandA>  ::= TBD
<i2c_subcommandA>  ::= TBD
<gpio_subcommandA> ::= "get"    | "set"      | "clear"    | "configure" | "query"
<gpio_direction>   ::= "input"  | "output"
<gpio_sense>       ::= "pullup" | "pulldown" | "floating" | "analog"
<port_subcommand>  ::= "porta"  | "portb"    | "portc" | "portd" | "porte" | "portf" | "portg" | "porth" | "porti"
<pin_subcommand>   ::= "pin0"   | "pin1"     | "pin2"  | "pin3"  | "pin4"  | "pin5"  | "pin6"  | "pin7"
                   | "pin8"   | "pin9"     | "pin10" | "pin11" | "pin12" | "pin13" | "pin14" | "pin15"
<subcommandD>      ::= TBD
<datastr>          ::= "(" <byte> ")"
<byte>             ::= <hdigit><hdigit> <whitespace>
<dec>              ::= <ddigit>
					| <ddigit> <dec> 
<hdigit>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e"
<ddigit>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" 
<EOL>              ::= "\r\n" | "\n"
<whitespace_char>  ::= " " | "\t"
<whitespace>       ::= <whitespace_char> | <whitespace> <whitespace_char>

S - Start Symbol: { <statement> }

Comments:
A: This grammar is used to implement an imperitive language called 'Fetch'

B: This is a 'Right Regular Grammar' -- an attempt to 'Keep it Simple'

Example:
        command:subcommand1:subcommand2:subcommand3:...:subcommandn(datastr1 datastr2 datastr3 ... datastrN)\n
        gpio:set:portd:pin7\n
        gpio:configure:portd:pin7:input:floating\n

\endverbatim
*/

static bool fetch_help(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_reset_pins(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_version(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_chip_id(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_hbtoggle(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_adc(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_dac(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);
static bool fetch_test(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[]);

/*! \brief Function command array for fetch_dispatch() callbacks
 *  Commands with NULL function return as not implemented.
 *  Commands with NULL help are not displayed with the help command.
 *  List MUST be NULL terminated. List parsing stops at first NULL command string.
 *
 *  Command names will have all whitespace and '_' parsed out before comparison.
 */
static fetch_command_t fetch_commands[] = {
  /*  function              command string      help string */
    { fetch_help,           "?",                "Display command help" },
    { fetch_help,           "help",             "Display command help" },
    { fetch_reset_pins,      "resetpins",        "Reset pins to defaults" },
    { fetch_version,        "version",          "Version information" },
    { fetch_chip_id,        "chipid",           "Return unique cpu chip id" },
    { fetch_hbtoggle,       "heatbeattoggle",   "Toggle mode of led heartbeat" },
    { fetch_gpio_dispatch,  "gpio",             "GPIO command set (*)" },
    { fetch_adc_dispatch,   "adc",              "ADC command set (*)" },
    { fetch_dac_dispatch,   "dac",              "DAC command set (*)" },
    { fetch_spi_dispatch,   "spi",              "SPI command set (*)" },
    { NULL,                 "i2c",              "I2C command set (*)" },
    { fetch_test,           "test",             NULL },
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
static bool fetch_test(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
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
 * \todo: This could be turned into an iterator over all the dicts
 */
static bool fetch_help(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Fetch Help:");
  util_message_info(chp, " (*) use <command>:help for more details");
  fetch_display_help(chp, fetch_commands);
	return true;
}

/*! \brief VERSION command callback for fetch language
 */
static bool fetch_version(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                          char * data_list[] UNUSED)
{
	util_message_string(chp, "firmware_version", GIT_COMMIT_VERSION);
	return true;
}

/*! \brief CHIPID command callback for fetch language
 */
static bool fetch_chip_id(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                          char * data_list[] UNUSED)
{
  uint32_t chip_id[3] = {STM32F4_UNIQUE_ID_LOW, STM32F4_UNIQUE_ID_CENTER, STM32F4_UNIQUE_ID_HIGH};

	util_message_hex_uint32(chp, "chip_id", chip_id, 3);
	return true;
}


/*! \brief HBTOGGLE command callback for fetch language
 */
static bool fetch_hbtoggle(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                          char * data_list[] UNUSED)
{
  hbToggle();
  return true;
}


/*! \brief RESETPINS command callback for fetch language
 */
static bool fetch_reset_pins(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                            char * data_list[] UNUSED)
{
	palInit(&pal_default_config);
	io_manage_table_to_defaults();
	return true;
}

/*! \brief Fetch initialization 
 * 
 * This needs to be called before parsing fetch commands
 */
void fetch_init(BaseSequentialStream * chp)
{
	/* place holder for future use */
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

	static char * command_toks[ FETCH_MAX_COMMANDS ];
	static char * data_toks[ FETCH_MAX_DATA_ITEMS ];

	char * lp;
	char * tokp;
	char * colonpart;
	char * parenpart;

	uint8_t n = 0;
	int arrlen = 0;

  // NOTE: we use our version of strncpy that guarentees a null terminated string
  //       along with only copying up to n chars or the first NULL

	// don't mess up the string passed from outside this function
	_strncpy( localinput, inputline, FETCH_MAX_LINE_CHARS );


  DBG_VMSG(chp, "'%s'", localinput);

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
        DBG_VMSG(chp, "'%s'[%d]", datastr, datastr_end);
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
	n   = 0;
	arrlen = NELEMS(command_toks);
	lp  = _strtok(commandstr, FETCH_CMD_DELIM, &tokp);
	command_toks[n] = lp;
	while ((lp = _strtok(NULL, FETCH_CMD_DELIM, &tokp)) != NULL)
	{
		if (n >= FETCH_MAX_COMMANDS)
		{
			util_message_error(chp, "Too many commands. Limit: %u", FETCH_MAX_COMMANDS);
			command_toks[0] = NULL;
			break;
		}
		if(n < arrlen - 1)
		{
			command_toks[++n] = lp;
		}
		else
		{
			return false;
		}
	}
	if(n < arrlen - 1)
	{
		command_toks[++n] = NULL;
	}
	else
	{
		return false;
	}

	if(parenpart != NULL)
	{
		/* break data into array of strings */
		n   = 0;
		arrlen = NELEMS(data_toks);
		lp  = _strtok(datastr, FETCH_DATA_DELIM, &tokp);
		data_toks[n] = lp;
		while ((lp = _strtok(NULL, FETCH_DATA_DELIM, &tokp)) != NULL)
		{
			if (n >= FETCH_MAX_DATA_ITEMS)
			{
				util_message_error(chp, "Too many data items. Limit: %u", FETCH_MAX_DATA_ITEMS);
				data_toks[0] = NULL;
				break;
			}
			if(n < arrlen - 1)
			{
				data_toks[++n] = lp;
			}
			else
			{
				return false;
			}
		}
		if(n < arrlen - 1)
		{

			data_toks[++n] = NULL;
		}
		else
		{
			return false;
		}
	}
	else
	{
		data_toks[0] = NULL;
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
  for( int index = 0; cmd_fn[index].command != NULL; index++ )
  {
    if( cmd_fn[index].help != NULL )
    {
      util_message_info(chp, "%s", cmd_fn[index].command);
      util_message_info(chp, "\t%s", cmd_fn[index].help);
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


/*! @} */
