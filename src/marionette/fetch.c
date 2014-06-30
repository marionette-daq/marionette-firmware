/*! \file fetch.c
 * Fetch: A very simple command language for the Marionette firmware.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "chprintf.h"

#include "util_strings.h"
#include "fetch.h"

/************************************************************
BNF Outline for the Fetch Language Grammar

(N,∑,P,S)

N - Non-terminals: { <statement>, <command>, <data>, <byte>, <constant> }
∑ - Terminals:     { <command>, <gpio_subcommandA>, <port_subcommand>, <pin_subcommand>, <subcommandD>, <digit>, <EOL>, <whitespace>}
P - Production Rules:

<statement>        ::= <command> <EOL>
                     | <command> ":" <gpio_subcommandA> ":" <port_subcommand> ":" <pin_subcommand> <EOL>
                     | <command> ":" <adc_subcommandA> ":" <subcommandB> ":" <subcommandC> ":" <subcommandD> <data> <EOL>  
<command>          ::= "?"     | "help"  | "gpio"  | "adc"   | "spi"   | "adc" | "resetpins"
<gpio_subcommandA> ::= "set"   | "clear" | "configure" 
<port_subcommand>  ::= "porta" | "portb" | "portc" | "portd" | "porte" | "portf" | "portg" | "porth" | "porti" |
<pin_subcommand>   ::= "pin0"  | "pin1"  | "pin2"  | "pin3"  | "pin4"  | "pin5"  | "pin6"  | "pin7"
				     | "pin8"  | "pin9"  | "pin10" | "pin11" | "pin12" | "pin13" | "pin14" | "pin15" | TBD
<subcommandD>      ::= TBD
<data>             ::= "(" <byte> ")"
<byte>             ::= <constant>
                     | <constant> <whitespace> <byte>
<constant>         ::= <digit><digit>
<digit>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e"
<EOL>              ::= "\n"
<whitespace>       ::= " " | "\t"

S - Start Symbol: { <statement> }

Comments: 
A: This grammar is used to implement an imperitive language

B: This is a "Right Regular Grammar" 

Example:
	command:subcommand1:subcommand2:subcommand3:...:subcommandn(data1 data2 data3 ... dataN)\n
	gpio:set:portd:pin7\n

***************************************************/

static HELP_command_dictionary     help_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = HELP_HELPSTRING};
static GPIO_command_dictionary     gpio_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = GPIO_HELPSTRING};

// All elements of the Terminal set (∑) have definitions here.
static const char * commands[]         = {"?", "help", "gpio", "adc", "spi", "i2c", "resetpins"};  
static const char * gpio_subcommandA[] = {"set", "clear", "configure"};
static const char * port_subcommand[]  = {"porta", "portb", "portc", "portd", "porte", "portf", "portg", "porth", "porti" };
static const char * pin_subcommand[]   = {"pin0", "pin1", "pin2", "pin3", "pin4", "pin5", "pin6", "pin7", "pin8", "pin9", "pin10", "pin11", "pin12", "pin13", "pin14", "pin15" };
static const char * subcommandD[]      = {};
static const char * digit[]            = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e"};
static const char * EOL[]              = {"\n"};
static const char * whitespace[]       = {" ", "\t"};

/* Help command implementation for fetch language */
void fetch_info(BaseSequentialStream * chp) {
	chprintf(chp, "\r\nFetch Commands Help:\r\n");
	chprintf(chp, "%s\r\n", gpio_lookup.helpstring);
}

bool fetch_exec(BaseSequentialStream* chp, char * inputline)
{
	int n = 0;
	char localinput[FETCH_MAX_LINE_CHARS];
	char * lp, *tokp;
	char * colonpart;
	char * parenpart;
	char * command;
	char * subcommands[FETCH_MAX_COMMANDS + 1];
	char * data[FETCH_MAX_DATA_ITEMS + 1];

	// don't mess up the string from outside the function
	strncpy(localinput, inputline, FETCH_MAX_LINE_CHARS);

	// break up into two strings the colons part and the parens part
	colonpart = _strtok(localinput, "(", &tokp);
	parenpart = _strtok(NULL, "(", &tokp);
	

	chprintf(chp, "colonpart: %s\r\nparenpart: %s\r\n", colonpart, parenpart);


/*
 *    lp           = _strtok(inputline, ":", &tokp);
 *    command      = lp;
 *    n            = 0;
 *    // Get all the ":" separated tokens
 *    while ((lp = _strtok(NULL, ":", &tokp)) != NULL)
 *    {
 *        // get all the tokens separated by ":"
 *        if (n >= STATEMENT_MAX_COMMANDS)
 *        {
 *            chprintf(chp, "too many commands\r\n");
 *            command = NULL;
 *            break;
 *        }
 *        statement[n++] = lp;
 *    }
 *    statement[n] = NULL;
 *
 *    // Test tokenizing
 *    chprintf(chp, "%s\t\r\n", command);
 *    for(n = 0; n < STATEMENT_MAX_COMMANDS + 1; ++n)
 *    {
 *        if(statement[n] == NULL)
 *        {
 *            break;
 *        }
 *        chprintf(chp, "\t%s\t ", statement[n]);
 *    }
 */

	chprintf(chp, "\r\n");
	// check if last token contains '('
	// if "(" then parse last token as data...
	return TRUE;
}









