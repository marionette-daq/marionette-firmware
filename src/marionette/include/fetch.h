/*! \file fetch.h
  * Support implementation of Fetch Language
  */

#ifndef FETCH_H_
#define FETCH_H_

/* BNF Outline for Marionette Fetch Grammar

(N,∑,P,S)

N - Non-terminals: { <statement>, <command>, <data>, <byte>, <constant> }
∑ - Terminals:     { <command>, <subcommandA>, <subcommandB>, <subcommandC>, <subcommandD>, <digit>, <EOL>, <whitespace>}
P - Production Rules:

<statement>   ::= <command> <EOL>
                | <command> ":" <subcommandA> <data> <EOL>  
                | <command> ":" <subcommandA> ":" <subcommandB> <data> <EOL>  
                | <command> ":" <subcommandA> ":" <subcommandB> ":" <subcommandC> <data> <EOL>
                | <command> ":" <subcommandA> ":" <subcommandB> ":" <subcommandC> ":" <subcommandD> <data> <EOL>  
<command>     ::= "?"    | "help"  | "gpio"      | "adc" | "spi" | "adc"
<subcommandA> ::= "set"  | "clear" | "configure" | TBD
<subcommandB> ::= "port" | TBD
<subcommandC> ::= "pin"  | TBD
<subcommandD> ::= TBD
<data>        ::= "(" <byte> ")"
<byte>        ::= <constant>
                | <constant> <whitespace> <byte>
<constant>    ::= <digit><digit>
<digit>       ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "a" | "b" | "c" | "d" | "e"
<EOL>         ::= "\n"
<whitespace>  ::= " " | "\t"

S - Start Symbol: { <command> }

Comments: 
A: This grammar is used to implement an imperitive language

B: This is probably a Right Regular Grammar    (We both stopped caring several lines ago...)

*/


// Defines
#define 		HELP_MAX_DATA_BYTES 					0
#define         HELP_HELPSTRING                         "\r\n"

#define 		GPIO_MAX_DATA_BYTES 					0
#define 		GPIO_HELPSTRING   					    ((const char *) "\r\n\tGPIO:<set,clear,configure>:port:pin")

#define 		GPIO_MAX_DATA_BYTES 					0
#define 		GPIO_HELPSTRING   					    ((const char *) "\r\n\tGPIO:<set,clear,configure>:port:pin")

// Commands
// These are STATIC string-arrays !there is NO modification of the command language by program
const char * fetch_commands[]   = {"?", "help", "gpio", "adc", "spi", "i2c"};                  // <command>
const char * subcommandA[]      = {"set", "clear", "configure"};
const char * subcommandB[]      = {"port"};
const char * subcommandC[]      = {"pin"};
const char * subcommandD[]      = {};

typedef struct help_command_dictionary
{
	bool     	   enabled;
	uint16_t 	   max_data_bytes;
	const char  *  helpstring;
} HELP_command_dictionary;


typedef struct gpio_command_dictionary
{
	bool     	   enabled;
	uint16_t 	   max_data_bytes;
	const char  *  helpstring;
} GPIO_command_dictionary;


// Functions




#endif
