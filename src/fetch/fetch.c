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
                     | <command> ":" <dac_subcommandA>  ":" <dac_configure>(<dec>) <EOL>
<command>          ::= "?"      | "help"     | "gpio"  | "adc"   | "dac"   | "spi" | "i2c" | "resetpins" | "heartbeat_toggle" | "version"
<adc_subcommandA>  ::= "conf_adc1" | "start" | "stop"
<adc_configure>    ::= "profile" | "oneshot" | "continuous" | "reset" | "vref_mv"
<adc_profile>      ::= "default" | "PA"   | "PB"
<dac_subcommandA>  ::= "on"      | "off" | "conf_ch1" | "conf_ch2" | "reset" | "release" 
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

/*! \todo put this into iterable structure
  * \sa fetch_info()
  */
static Command_dictionary          help_dict               = { .enabled = true, .max_data_bytes = HELP_MAX_DATA_BYTES,      .helpstring = HELP_HELPSTRING};
static Command_dictionary          gpio_dict               = { .enabled = true, .max_data_bytes = GPIO_MAX_DATA_BYTES,      .helpstring = GPIO_HELPSTRING};
static Command_dictionary          adc_dict                = { .enabled = true, .max_data_bytes = ADC_MAX_DATA_BYTES,       .helpstring = ADC_HELPSTRING};
static Command_dictionary          dac_dict                = { .enabled = true, .max_data_bytes = DAC_MAX_DATA_BYTES,       .helpstring = DAC_HELPSTRING};
static Command_dictionary          version_dict            = { .enabled = true, .max_data_bytes = VERSION_MAX_DATA_BYTES,   .helpstring = VERSION_HELPSTRING};
static Command_dictionary          heartbeat_toggle_dict   = { .enabled = true, .max_data_bytes = HEARTBEAT_TOGGLE_MAX_DATA_BYTES,   .helpstring = HEARTBEAT_TOGGLE_HELPSTRING};
static Command_dictionary          resetpins_dict          = { .enabled = true, .max_data_bytes = RESETPINS_MAX_DATA_BYTES, .helpstring = RESETPINS_HELPSTRING};

/*! \brief Terminals for the Fetch grammar
 */
static Fetch_terminals fetch_terms =
{
	.command          = {"?", "help", "version", "chipid", "gpio", "adc", "dac", "spi", "i2c", "resetpins", "heartbeat_toggle"},
	.gpio_subcommandA = {"get", "set", "clear", "configure", "query"},
	.gpio_direction   = {"input", "output"},
	.gpio_sense       = {"pullup", "pulldown", "floating", "analog"},
	.adc_subcommandA  = {"conf_adc1", "start", "stop"},
	.adc_configure    = {"profile", "oneshot", "continuous", "reset", "vref_mv"},
	.adc_profile      = {"default", "PA", "PB"},
	.dac_subcommandA  = {"on", "off", "conf_ch1", "conf_ch2", "reset", "release"},
    .dac_configure    = {"dc_mv"},  
	.port_subcommand  = {"porta", "portb", "portc", "portd", "porte", "portf", "portg", "porth", "porti" },
	.pin_subcommand   = {"pin0", "pin1", "pin2", "pin3", "pin4", "pin5", "pin6", "pin7", "pin8", "pin9", "pin10", "pin11", "pin12", "pin13", "pin14", "pin15" },
	.subcommandD      = {},
	.digit            = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e"},
	.EOL              = {"\n"},
	.whitespace       = {" ", "\t"}
};

/*! \brief Function pointer array for \sa fetch_dispatch() callbacks
 */
static bool (*cmd_fns[NELEMS(fetch_terms.command)]) (BaseSequentialStream * chp,
                char * l1[], char * l2[]);

/*! \brief Placeholder function for TBD callbacks
 */
static bool fetch_not_yet(BaseSequentialStream  * chp, char * cmd_list[] UNUSED,
                          char * data_list[] UNUSED)
{
	DBG_MSG(chp, "Not implemented");
	return false;
};

/*!
 * Here is the theory on why using static inline here might be useful.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Inline.html
 *
 */

/*!
 * fetch_is_valid_* functions
 * \param[in] chkcommand      Input String to match

 * \return index to command match in array
 * \return -1 on fail to match
 *
 * \sa token_match()
 */
static inline int fetch_is_valid_command(BaseSequentialStream * chp, char * chkcommand)
{
	return(token_match(chp, fetch_terms.command, chkcommand,
	                   ((int) NELEMS(fetch_terms.command)) ));
}

inline int fetch_is_valid_digit(BaseSequentialStream * chp, char * chkdigit)
{
	return(token_match(chp, fetch_terms.digit, chkdigit,
	                   ((int) NELEMS(fetch_terms.digit))) );
}

inline int fetch_is_valid_EOL(BaseSequentialStream * chp, char * chkEOL)
{
	return(token_match(chp, fetch_terms.EOL, chkEOL,
	                   ((int) NELEMS(fetch_terms.EOL))) );
}

inline int fetch_is_valid_whitespace(BaseSequentialStream * chp, char * chkwhitespace)
{
	return(token_match(chp, fetch_terms.whitespace, chkwhitespace,
	                   ((int) NELEMS(fetch_terms.whitespace))) );
}

/*! Help command callback for fetch language
 * \todo: This could be turned into an iterator over all the dicts
 */
static bool fetch_info(BaseSequentialStream * chp, char * cl[] UNUSED, char * dl[] UNUSED)
{
	util_message_comment(chp, "Help");
	util_message_comment(chp, "\t%s", heartbeat_toggle_dict.helpstring);
	util_message_comment(chp, "\t%s", version_dict.helpstring);
	util_message_comment(chp, "\t%s", resetpins_dict.helpstring);
	util_message_comment(chp, "\t%s", gpio_dict.helpstring);
	util_message_comment(chp, "\t%s", adc_dict.helpstring);
	util_message_comment(chp, "\t%s", dac_dict.helpstring);
	return true;
}

/*! \brief ADC command callback for fetch language
 */
static bool fetch_adc(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[])
{
	if(adc_dict.enabled)
	{
		return(fetch_adc_dispatch(chp, cmd_list, data_list, &fetch_terms));
	}
	util_message_info(chp, "Command not enabled");
	return false;
}

/*! \brief DAC command callback for fetch language
 */
static bool fetch_dac(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[])
{
	if(dac_dict.enabled)
	{
		return(fetch_dac_dispatch(chp, cmd_list, data_list, &fetch_terms));
	}
	util_message_info(chp, "Command not enabled");
	return false;
}


/*! \brief GPIO command callback for fetch language
 */
static bool fetch_gpio(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[])
{
	if(gpio_dict.enabled)
	{
		return(fetch_gpio_dispatch(chp, cmd_list, data_list, &fetch_terms));
	}
	util_message_info(chp, "Command not enabled");
	return false;
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
	if(heartbeat_toggle_dict.enabled)
	{
		hbToggle();
		return true;
	}

	util_message_info(chp, "Command not enabled");
	return false;
}


/*! \brief RESETPINS command callback for fetch language
 */
static bool fetch_resetpins(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                            char * data_list[] UNUSED)
{
	palInit(&pal_default_config);
	io_manage_table_to_defaults();
	return true;
}

/*! \brief register callbacks for command functions here
 *               \sa fetch_init()
 */
static void fetch_init_cmd_fns(BaseSequentialStream * chp)
{
	for(int i = 0; i < ((int) NELEMS(fetch_terms.command)); ++i)
	{
		if (strncasecmp(fetch_terms.command[i], "?", strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_info;
		}
		else if (strncasecmp(fetch_terms.command[i], "help",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_info;
		}
		else if (strncasecmp(fetch_terms.command[i], "resetpins",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_resetpins;
		}
		else if (strncasecmp(fetch_terms.command[i], "version",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_version;
		}
		else if (strncasecmp(fetch_terms.command[i], "chipid",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_chip_id;
		}
		else if (strncasecmp(fetch_terms.command[i], "heartbeat_toggle",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_hbtoggle;
		}
		else if (strncasecmp(fetch_terms.command[i], "gpio",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_gpio;
		}
		else if (strncasecmp(fetch_terms.command[i], "adc",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_adc;
		}
		else if (strncasecmp(fetch_terms.command[i], "dac",
		                     strlen(fetch_terms.command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_dac;
		}
		else
		{
			cmd_fns[i] = fetch_not_yet;
		}
	}
}

/*! \brief Fetch initialization 
 * 
 * This needs to be called before parsing fetch commands
 */
void fetch_init(BaseSequentialStream * chp)
{
	fetch_init_cmd_fns(chp) ;
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
	static char     localinput[FETCH_MAX_LINE_CHARS];
	static char     commandstr[FETCH_MAX_LINE_CHARS];
	static char     datastr[FETCH_MAX_LINE_CHARS];

	static char      *     command_toks[FETCH_MAX_LINE_CHARS];
	static char      *     data_toks[FETCH_MAX_LINE_CHARS];

	char      *      lp;
	char      *      tokp;
	char      *      colonpart;
	char      *      parenpart;

	uint8_t          n = 0;

	int              arrlen = NELEMS(command_toks);

	// don't mess up the string passed from outside this function
	strncpy(localinput, inputline, FETCH_MAX_LINE_CHARS);

	// break up into two strings the colons part and the parens part
	if(localinput[0] != '(' || localinput[0] == ')')
	{
		colonpart = _strtok(localinput, "(", &tokp);
		parenpart = _strtok(NULL, "(", &tokp);
	}
	else
	{
		util_message_error(chp, "No command-(only data?)");
		return false;
	}

	if(colonpart != NULL)
	{
		strncpy(commandstr, colonpart, strlen(colonpart));
		commandstr[strlen(colonpart)] = '\0';
		fetch_remove_spaces(commandstr);
		if(parenpart != NULL)
		{
			strncpy(datastr, parenpart, strlen(parenpart));
			datastr[strlen(parenpart) - 1] = '\0';
			//DBG_VMSG(chp, "datastr : %s\r\n", datastr);
		}
	}
	else
	{
		// empty line
		return true;
	}

	/* break commandstr into array of strings */
	n   = 0;
	lp  = _strtok(commandstr, ":", &tokp);
	command_toks[n] = lp;
	while ((lp = _strtok(NULL, ":", &tokp)) != NULL)
	{
		if (n >= FETCH_MAX_COMMANDS)
		{
			util_message_error(chp, "Too many data. Limit: %u", FETCH_MAX_COMMANDS);
			command_toks[0] = NULL;
			break;
		}
		if(n < arrlen - 1)
		{
			command_toks[++n] = lp;
		}
		else
		{
			//DBG_VMSG(chp, "Past array bounds: %d", arrlen);
			return false;
		}
	}
	if(n < arrlen - 1)
	{
		command_toks[++n] = "\0";
	}
	else
	{
		DBG_VMSG(chp, "Past array bounds: %d", arrlen);
		return false;
	}

	if(parenpart != NULL)
	{
		/* break data into array of strings */
		n   = 0;
		lp  = _strtok(datastr, " ", &tokp);
		data_toks[n] = lp;
		arrlen = NELEMS(data_toks);
		while ((lp = _strtok(NULL, " ", &tokp)) != NULL)
		{
			if (n >= FETCH_MAX_DATA_ITEMS)
			{
				util_message_error(chp, "Too many data. Limit: %u", FETCH_MAX_DATA_ITEMS);
				data_toks[0] = NULL;
				break;
			}
			if(n < arrlen - 1)
			{
				data_toks[++n] = lp;
			}
			else
			{
				//DBG_VMSG(chp, "Too many tokens. Limit: %d", arrlen);
				return false;
			}
		}
		if(n < arrlen - 1)
		{

			data_toks[++n] = "\0";
		}
		else
		{
			//DBG_VMSG(chp, "Too many tokens. Limit: %d", arrlen);
			return false;
		}
	}
	else
	{
		data_toks[0] = NULL;
	}
	return(fetch_dispatch(chp, command_toks, data_toks));
}

/*! \brief dispatch the commands from here
 * \param[in] command_list A list of colon separated command strings is in the command_list
 * \param[in] data_list    A list of space separated data is in data_list
 *
 * \warning data_list[0] may be null. Not all commands accept data.
 *
 * \sa fetch_init_cmd_fns()
 */
bool fetch_dispatch(BaseSequentialStream * chp, char * command_list[], char * data_list[])
{
	int cindex = fetch_is_valid_command(chp, command_list[0]);

	if(cindex < 0)
	{
		return false;
	}

	return ( (*cmd_fns[cindex]) (chp, command_list, data_list) );
}
/*! @} */
