/*! \file fetch.c
 * Fetch: A very simple command language for the Marionette firmware.
 */

#include <stdint.h>
#include <stdbool.h>

#include "hal.h"
#include "chprintf.h"
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
<command>          ::= "gpio"  | "adc"   | "spi"   | "adc" | "resetpins"
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
static const char * commands[]         = {"?", "help", "gpio", "adc", "spi", "i2c"};  
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



