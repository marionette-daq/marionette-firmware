/*! \file fetch.c
 *  Fetch: A DSL for Marionette interaction.
 * @addtogroup fetch_dsl
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
#include "fetch_gpio.h"
#include "fetch_adc.h"

#include "fetch_defs.h"
#include "fetch.h"

/* \todo Mon 14 July 2014 11:17:26 (PDT)  Investigate BISON/YACC if this gets too hoary. 
 * \warning BISON/YACC may need dynamic memory management?
 */

/************************************************************
BNF Outline for the Fetch Language Grammar

(N,∑,P,S)

N - Non-terminals: { <statement>, <command>, <datastr>, <byte>, <constant> }
∑ - Terminals:     { <command>, <gpio_subcommandA>, <gpio_direction>, <gpio_sense>, <port_subcommand>, <pin_subcommand>, <subcommandD>, <digit>, <EOL>, <whitespace>}
P - Production Rules:

<statement>        ::= <command> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> ":" <gpio_direction> ":" <gpio_sense> <EOL>
                     | <command> ":" <adc_subcommandA>  ":" <adc_sampletype>  ":" <EOL>
                     | <command> ":" <adc_subcommandA>  ":" <adc_configure>   ":" <EOL>
<command>          ::= "?"      | "help"     | "gpio"  | "adc"   | "spi"   | "adc" | "resetpins"
<adc_subcommandA>  ::= "configure" | "start" | "stop"
<adc_configure>    ::= TBD
<adc_sampletype>   ::= "oneshot"   | "continuous" 
<spi_subcommandA>  ::= TBD
<i2c_subcommandA>  ::= TBD
<gpio_subcommandA> ::= "get"    | "set"      | "clear"    | "configure"
<gpio_direction>   ::= "input"  | "output"
<gpio_sense>       ::= "pullup" | "pulldown" | "floating" | "analog"
<port_subcommand>  ::= "porta"  | "portb"    | "portc" | "portd" | "porte" | "portf" | "portg" | "porth" | "porti"
<pin_subcommand>   ::= "pin0"   | "pin1"     | "pin2"  | "pin3"  | "pin4"  | "pin5"  | "pin6"  | "pin7"
                   | "pin8"   | "pin9"     | "pin10" | "pin11" | "pin12" | "pin13" | "pin14" | "pin15"
<subcommandD>      ::= TBD
<datastr>          ::= "(" <byte> ")"
<byte>             ::= <constant>
                     | <constant> <whitespace> <byte>
<constant>         ::= <digit><digit>
<digit>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e"
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

***************************************************/

static Command_dictionary          help_dict      = { .enabled = true, .max_data_bytes = HELP_MAX_DATA_BYTES,      .helpstring = HELP_HELPSTRING};
static Command_dictionary          gpio_dict      = { .enabled = true, .max_data_bytes = GPIO_MAX_DATA_BYTES,      .helpstring = GPIO_HELPSTRING};
static Command_dictionary          adc_dict       = { .enabled = true, .max_data_bytes = ADC_MAX_DATA_BYTES,       .helpstring = ADC_HELPSTRING};
static Command_dictionary          version_dict   = { .enabled = true, .max_data_bytes = VERSION_MAX_DATA_BYTES,   .helpstring = VERSION_HELPSTRING};
static Command_dictionary          resetpins_dict = { .enabled = true, .max_data_bytes = RESETPINS_MAX_DATA_BYTES, .helpstring = RESETPINS_HELPSTRING};

static Fetch_terminals fetch_terms =
{
	.command          = {"?", "help", "version", "gpio", "adc", "spi", "i2c", "resetpins"},
	.gpio_subcommandA = {"get", "set", "clear", "configure"},
	.gpio_direction   = {"input", "output"},
	.gpio_sense       = {"pullup", "pulldown", "floating", "analog"},
	.adc_subcommandA  = {"configure", "start", "stop"},
	.adc_configure    = {},
	.adc_sampletype   = {"oneshot", "continuous"},
	.port_subcommand  = {"porta", "portb", "portc", "portd", "porte", "portf", "portg", "porth", "porti" },
	.pin_subcommand   = {"pin0", "pin1", "pin2", "pin3", "pin4", "pin5", "pin6", "pin7", "pin8", "pin9", "pin10", "pin11", "pin12", "pin13", "pin14", "pin15" },
	.subcommandD      = {},
	.digit            = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e"},
	.EOL              = {"\n"},
	.whitespace       = {" ", "\t"}
};

static bool (*cmd_fns[NELEMS(fetch_terms.command)]) (BaseSequentialStream * chp,
                char * l1[], char * l2[]);

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
 * *_is_valid_command...
 * return index to command match in array
 * return -1 on fail to match
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

/* Help command implementation for fetch language 
 * TODO: This could be turned into an iterator over all the dicts 
 */
static bool fetch_info(BaseSequentialStream * chp, char * cl[] UNUSED, char * dl[] UNUSED)
{
	util_infomsg(chp, "Help");
	chprintf(chp, "%s\r\n", version_dict.helpstring);
	chprintf(chp, "%s\r\n", resetpins_dict.helpstring);
	chprintf(chp, "%s\r\n", gpio_dict.helpstring);
	chprintf(chp, "%s\r\n", adc_dict.helpstring);
	return true;
}

static bool fetch_adc(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[])
{
		if(adc_dict.enabled) {
				DBG_MSG(chp, "adc call");
			return(fetch_adc_dispatch(chp, cmd_list, data_list, &fetch_terms));
		}
		
		DBG_MSG(chp, "Not yet implemented");
		return true;
}


static bool fetch_gpio(BaseSequentialStream  * chp, char * cmd_list[], char * data_list[])
{
		if(gpio_dict.enabled) {
			return(fetch_gpio_dispatch(chp, cmd_list, data_list, &fetch_terms));
		}
		return false;
}

static bool fetch_version(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                            char * data_list[] UNUSED)
{
	static          VERSIONData                     version_data;

	util_fwversion(&version_data);
	chprintf(chp, "Fetch Firmware Version:   %s\r\n", version_data.firmware);
	return true;
};

static bool fetch_resetpins(BaseSequentialStream * chp, char * cmd_list[] UNUSED,
                            char * data_list[] UNUSED)
{
	DBG_MSG(chp, "Resetting pins");
	palInit(&pal_default_config);
	return true;
};

/*! \brief register callbacks for command functions here
                \sa fetch_init
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
		else
		{
			cmd_fns[i] = fetch_not_yet;
		}
	}
}

/*! \brief catch all function for initialization */
void fetch_init(BaseSequentialStream * chp)
{
	fetch_init_cmd_fns(chp) ;
}

/*! \brief parse the Fetch Statement

        Create a commands list a data list
        Dispatch the commands \sa fetch_dispatch
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
		util_errormsg(chp, "No command-(only data?)");
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
			datastr[strlen(parenpart)] = '\0';
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
			util_errormsg(chp, "Too many commands.");
			command_toks[0] = NULL;
			break;
		}
		if(n < arrlen - 1)
		{
			command_toks[++n] = lp;
		}
		else
		{
			DBG_VMSG(chp, "Past array bounds: %d", arrlen);
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
			if (n >= FETCH_MAX_COMMANDS)
			{
				util_errormsg(chp, "Too many commands.");
				data_toks[0] = NULL;
				break;
			}
			if(n < arrlen - 1)
			{
				data_toks[++n] = lp;
			}
			else
			{
				DBG_VMSG(chp, "Too many tokens. Limit: %d", arrlen);
				return false;
			}
		}
		if(n < arrlen - 1)
		{

			data_toks[++n] = "\0";
		}
		else
		{
			DBG_VMSG(chp, "Too many tokens. Limit: %d", arrlen);
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
        a list of colon separated command strings is in the command_list
        a list of space separated data is in data_list

        \warning data_list[0] may be null.

        \sa fetch_init_cmd_fns
 */
bool fetch_dispatch(BaseSequentialStream * chp, char * command_list[], char * data_list[])
{
	int cindex = fetch_is_valid_command(chp, command_list[0]);

	if(cindex < 0)
	{
		DBG_MSG(chp, "Unrecognized command.");
		return false;
	}

	return ( (*cmd_fns[cindex]) (chp, command_list, data_list) );
}


/*! @} */
