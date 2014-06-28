/*! \file fetch.c
 */

#include <stdint.h>
#include <stdbool.h>

#include "fetch.h"


/* A very simple command language to configure the marionette. */

/*

  Commands are deliniated by ':' and '(' and ')'
  They are read from left to right.

  Let's just do lower case only for now.


command:subcommand1:subcommand2:subcommand3:...:subcommandn(data1 data2 data3 ... dataN)

examples :
spi1:configure:cr1:(0x123456)
spi1:configure:cpol:(0x3)
spi1:send:(<numbytes> <prefixbyte> 0x3 0x8 0x)
spi1:get:(<numbytes> <prefixbyte> 0x3 0x8 0x)
spi1:exchange:(<numbytes-write> <numbytes-read> <prefixbyte> <write-bytes>)

gpio:set:portd:pin7()
gpio:clear:portd:pin7()
gpio:configure:portd:pin7:input()
gpio:configure:portd:pin7:pullup()
gpio:configure:portd:pin7:floating()


adc:configure:....


*/
HELP_command_dictionary     help_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = HELP_HELPSTRING};
GPIO_command_dictionary     gpio_lookup = { .enabled = true, .max_data_bytes = 0, .helpstring = GPIO_HELPSTRING};


// GPIO subcommand A


// ADC subcommand A



