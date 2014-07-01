/*! \file fetch.c
 * Fetch: A DSL for Marionette interaction.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "gpio.h"

#include "fetch.h"

/************************************************************
BNF Outline for the Fetch Language Grammar

(N,∑,P,S)

N - Non-terminals: { <statement>, <command>, <datastr>, <byte>, <constant> }
∑ - Terminals:     { <command>, <gpio_subcommandA>, <port_subcommand>, <pin_subcommand>, <subcommandD>, <digit>, <EOL>, <whitespace>}
P - Production Rules:

<statement>        ::= <command> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> <EOL>
                     | <command> ":" <adc_subcommandA> ":" <subcommandB> ":" <subcommandC> ":" <subcommandD> <datastr> <EOL>
<command>          ::= "?"      | "help"     | "gpio"  | "adc"   | "spi"   | "adc" | "resetpins"
<adc_subcommandA>  ::= TBD
<spi_subcommandA>  ::= TBD
<i2c_subcommandA>  ::= TBD
<gpio_subcommandA> ::= "set"    | "clear"    | "configure"
<gpio_direction>   ::= "input"  | "output"
<gpio_sense>       ::= "pullup" | "pulldown" | "floating" | "analog"
<port_subcommand>  ::= "porta"  | "portb"    | "portc" | "portd" | "porte" | "portf" | "portg" | "porth" | "porti"
<pin_subcommand>   ::= "pin0"   | "pin1"     | "pin2"  | "pin3"  | "pin4"  | "pin5"  | "pin6"  | "pin7"
                                     | "pin8"   | "pin9"     | "pin10" | "pin11" | "pin12" | "pin13" | "pin14" | "pin15"
<subcommandD>      ::= TBD
<datastr>             ::= "(" <byte> ")"
<byte>             ::= <constant>
                     | <constant> <whitespace> <byte>
<constant>         ::= <digit><digit>
<digit>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e"
<EOL>              ::= "\n"
<whitespace>       ::= " " | "\t"

S - Start Symbol: { <statement> }

Comments:
A: This grammar is used to implement an imperitive language called 'Fetch'

B: This is a 'Right Regular Grammar'

Example:
        command:subcommand1:subcommand2:subcommand3:...:subcommandn(datastr1 datastr2 datastr3 ... datastrN)\n
        gpio:set:portd:pin7\n
        gpio:configure:portd:pin7:input:floating\n

***************************************************/

static HELP_command_dictionary          help_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = HELP_HELPSTRING};
static GPIO_command_dictionary          gpio_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = GPIO_HELPSTRING};
static RESETPINS_command_dictionary     resetpins_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = RESETPINS_HELPSTRING};

// All elements of the Terminal set (∑) have definitions here.
static const char * command[]          = {"?", "help", "gpio", "adc", "spi", "i2c", "resetpins"};
static bool (*cmd_fns[NELEMS(command)]) (BaseSequentialStream * chp, char * l1[], char * l2[]);

static const char * gpio_subcommandA[] = {"set", "clear", "configure"};
static const char * port_subcommand[]  = {"porta", "portb", "portc", "portd", "porte", "portf", "portg", "porth", "porti" };
static const char * pin_subcommand[]   = {"pin0", "pin1", "pin2", "pin3", "pin4", "pin5", "pin6", "pin7", "pin8", "pin9", "pin10", "pin11", "pin12", "pin13", "pin14", "pin15" };
static const char * subcommandD[]      = {};
static const char * digit[]            = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e"};
static const char * EOL[]              = {"\n"};
static const char * whitespace[]       = {" ", "\t"};

/*
   return index to command match in array
   return -1 on fail to match
*/

static bool fetch_not_yet(BaseSequentialStream  * chp, char * commandl[] UNUSED,
                          char * datal[] UNUSED)
{
	DBG_MSG(chp, "Not implemented");
	return false;
};
static int fetch_is_valid_command(BaseSequentialStream * chp, char * chkcommand)
{
	for(int i = 0; i < ((int) NELEMS(command)); ++i)
	{
		if (strncasecmp(command[i], chkcommand, strlen(command[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static int fetch_is_valid_gpio_subcommandA(BaseSequentialStream * chp,
                char * chkgpio_subcommandA)
{
	for(int i = 0; i < ((int) NELEMS(gpio_subcommandA)); ++i)
	{
		if (strncasecmp(gpio_subcommandA[i], chkgpio_subcommandA, strlen(gpio_subcommandA[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static int fetch_is_valid_port_subcommand(BaseSequentialStream * chp,
                char * chkport_subcommand)
{
	for(int i = 0; i < ((int) NELEMS(port_subcommand)); ++i)
	{
		if (strncasecmp(port_subcommand[i], chkport_subcommand, strlen(port_subcommand[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static size_t get_longest_str_length(const char * s1, const char * s2)
{
	size_t lens1  = strnlen(s1, FETCH_MAX_CMD_STRLEN );
	size_t lens2  = strnlen(s2, FETCH_MAX_CMD_STRLEN );

	return (max(lens1, lens2));
}

static int fetch_is_valid_pin_subcommand(BaseSequentialStream * chp, char * chkpin_subcommand)
{
	size_t maxlen = 1;
	for(int i = 0; i < ((int) NELEMS(pin_subcommand)); ++i)
	{
		maxlen = get_longest_str_length(pin_subcommand[i], chkpin_subcommand);
		if (strncasecmp(pin_subcommand[i], chkpin_subcommand, maxlen ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static int fetch_is_valid_digit(BaseSequentialStream * chp, char * chkdigit)
{
	for(int i = 0; i < ((int) NELEMS(digit)); ++i)
	{
		if (strncasecmp(digit[i], chkdigit, strlen(digit[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static int fetch_is_valid_EOL(BaseSequentialStream * chp, char * chkEOL)
{
	for(int i = 0; i < ((int) NELEMS(EOL)); ++i)
	{
		if (strncasecmp(EOL[i], chkEOL, strlen(EOL[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

static int fetch_is_valid_whitespace(BaseSequentialStream * chp, char * chkwhitespace)
{
	for(int i = 0; i < ((int) NELEMS(whitespace)); ++i)
	{
		if (strncasecmp(whitespace[i], chkwhitespace, strlen(whitespace[i]) ) == 0)
		{
			return i;
		}
	}
	return -1;
}

/* Help command implementation for fetch language */
static bool fetch_info(BaseSequentialStream * chp, char * cl[] UNUSED, char * dl[] UNUSED)
{
	util_infomsg(chp, "Fetch commandstr Help");
	chprintf(chp, "%s\r\n", gpio_lookup.helpstring);
	chprintf(chp, "%s\r\n", resetpins_lookup.helpstring);
	return true;
}

static bool fetch_gpio(BaseSequentialStream  * chp, char * commandl[], char * datal[])
{
	DBG_VMSG(chp, "GPIO ...%s...", commandl[1]);
	if(fetch_is_valid_gpio_subcommandA(chp, commandl[1]) >= 0)
	{
		if(fetch_is_valid_port_subcommand(chp, commandl[2]) >= 0)
		{
			if(fetch_is_valid_pin_subcommand(chp, commandl[3]) >= 0)
			{
				if (strncasecmp(commandl[1], "set", strlen("set") ) == 0)
				{
					gpio_set(chp, commandl);
				}
				else if (strncasecmp(commandl[1], "clear", strlen("clear") ) == 0)
				{
					gpio_clear(chp, commandl);
				}
				else if (strncasecmp(commandl[1], "config", strlen("config") ) == 0)
				{
					gpio_config(chp, commandl);
				}
				else
				{
					fetch_not_yet(chp, commandl, datal) ;
				}
				return true;
			}
		}
	}
	return false;
}

static bool fetch_resetpins(BaseSequentialStream * chp, char * commandl[] UNUSED, char * datal[] UNUSED) {
		DBG_MSG(chp, "Resetting pins");
  		palInit(&pal_default_config);
		return true;
};

/*! \brief register callbacks for command functions here
                \sa fetch_init
*/
static void fetch_init_cmd_fns(BaseSequentialStream * chp)
{
	for(int i = 0; i < ((int) NELEMS(command)); ++i)
	{
		if (strncasecmp(command[i], "?", strlen(command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_info;
		}
		else if (strncasecmp(command[i], "help", strlen(command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_info;
		}
		else if (strncasecmp(command[i], "gpio", strlen(command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_gpio;
		}
		else if (strncasecmp(command[i], "resetpins", strlen(command[i]) ) == 0)
		{
			cmd_fns[i] = fetch_resetpins;
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
	uint8_t n = 0;
	char localinput[FETCH_MAX_LINE_CHARS];
	char * lp, *tokp;
	char * colonpart;
	char commandstr[FETCH_MAX_LINE_CHARS];
	char * command_toks[FETCH_MAX_LINE_CHARS];
	char * parenpart;
	char datastr[FETCH_MAX_LINE_CHARS];
	char * data_toks[FETCH_MAX_LINE_CHARS];

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
		strncpy(datastr, parenpart, strlen(parenpart));
		datastr[strlen(parenpart)]    = '\0';
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
		command_toks[++n] = lp;
	}

	if(parenpart != NULL)
	{
		/* break data into array of strings */
		n   = 0;
		lp  = _strtok(datastr, " ", &tokp);
		data_toks[n] = lp;
		while ((lp = _strtok(NULL, " ", &tokp)) != NULL)
		{
			if (n >= FETCH_MAX_COMMANDS)
			{
				util_errormsg(chp, "Too many commands.");
				data_toks[0] = NULL;
				break;
			}
			data_toks[++n] = lp;
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


