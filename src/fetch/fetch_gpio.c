/*! \file fetch_gpio.c
 * Marionette fetch_gpio routines
 * @defgroup fetch_gpio Fetch GPIO
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"
#include "util_io.h"
#include "util_arg_parse.h"

#include "fetch_defs.h"
#include "fetch_gpio.h"
#include "fetch.h"
#include "fetch_parser.h"

#ifndef GPIO_BUFFER_SIZE
#define GPIO_BUFFER_SIZE 128
#endif

#ifndef DEFAULT_PIN_MODE
#define DEFAULT_PIN_MODE PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING
#endif

// this defines which pins we are allowed to manipulate through the gpio module
#define PORT_A_GPIO_MASK 0x8003
#define PORT_C_GPIO_MASK 0x2000
#define PORT_B_GPIO_MASK 0xc300
#define PORT_E_GPIO_MASK 0x23ef
#define PORT_D_GPIO_MASK 0x1bfb
#define PORT_G_GPIO_MASK 0xffff
#define PORT_F_GPIO_MASK 0xf803
#define PORT_I_GPIO_MASK 0x0d1f
#define PORT_H_GPIO_MASK 0xde6c

typedef struct {
  uint16_t a, b, c, d, e, f, g, h, i;
} port_states_t;

static uint8_t gpio_buffer[GPIO_BUFFER_SIZE];


static bool valid_gpio_port_pin( ioportid_t port, uint32_t pin )
{
  switch( (uint32_t)port )
  {
    case GPIOA_BASE:
      return (bool)(PORT_A_GPIO_MASK & (1<<pin));
    case GPIOB_BASE:
      return (bool)(PORT_B_GPIO_MASK & (1<<pin));
    case GPIOC_BASE:
      return (bool)(PORT_C_GPIO_MASK & (1<<pin));
    case GPIOD_BASE:
      return (bool)(PORT_D_GPIO_MASK & (1<<pin));
    case GPIOE_BASE:
      return (bool)(PORT_E_GPIO_MASK & (1<<pin));
    case GPIOF_BASE:
      return (bool)(PORT_G_GPIO_MASK & (1<<pin));
    case GPIOG_BASE:
      return (bool)(PORT_G_GPIO_MASK & (1<<pin));
    case GPIOH_BASE:
      return (bool)(PORT_H_GPIO_MASK & (1<<pin));
    case GPIOI_BASE:
      return (bool)(PORT_I_GPIO_MASK & (1<<pin));
  }
  return false;
}

static void write_all( port_states_t set_mask, port_states_t clear_mask )
{
  GPIOA->BSRR.W = (clear_mask.a << 16) | set_mask.a;
  GPIOB->BSRR.W = (clear_mask.b << 16) | set_mask.b;
  GPIOC->BSRR.W = (clear_mask.c << 16) | set_mask.c;
  GPIOD->BSRR.W = (clear_mask.d << 16) | set_mask.d;
  GPIOE->BSRR.W = (clear_mask.e << 16) | set_mask.e;
  GPIOF->BSRR.W = (clear_mask.f << 16) | set_mask.f;
  GPIOG->BSRR.W = (clear_mask.g << 16) | set_mask.g;
  GPIOH->BSRR.W = (clear_mask.h << 16) | set_mask.h;
  GPIOI->BSRR.W = (clear_mask.i << 16) | set_mask.i;
}

static port_states_t read_all( void )
{
  port_states_t states;
  states.a = palReadPort(GPIOA);
  states.b = palReadPort(GPIOB);
  states.c = palReadPort(GPIOC);
  states.d = palReadPort(GPIOD);
  states.e = palReadPort(GPIOE);
  states.f = palReadPort(GPIOF);
  states.g = palReadPort(GPIOG);
  states.h = palReadPort(GPIOH);
  states.i = palReadPort(GPIOI);
  return states;
}

bool fetch_gpio_help_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);

  FETCH_HELP_BREAK(chp);
  FETCH_HELP_LEGEND(chp);
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_TITLE(chp, "GPIO Help");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read(<io>[, ...])");
  FETCH_HELP_DES(chp,"Read pin state(s)");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read.latch(<port>)");
  FETCH_HELP_DES(chp,"Read port output latch");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read.port(<port>)");
  FETCH_HELP_DES(chp,"Read port input values");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read.all");
  FETCH_HELP_DES(chp,"Read all port input values");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"reset");
  FETCH_HELP_DES(chp,"Reset GPIO module");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"write(<io>,<value>[,<io>,<value> ...])");
  FETCH_HELP_DES(chp,"Write to io pin(s)");
  FETCH_HELP_ARG(chp,"io","io pin name");
  FETCH_HELP_ARG(chp,"value","0 | 1");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"write.port(<port>, <value>)");
  FETCH_HELP_DES(chp,"Write to port");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_ARG(chp,"value","16bit value to write");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"write.all(<val_a>, ..., <val_i>)");
  FETCH_HELP_DES(chp,"Write to all ports");
  FETCH_HELP_ARG(chp,"val", "16bit value to write")
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"set(<io>[, ...])");
  FETCH_HELP_DES(chp,"Set io pin(s) to 1");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"clear(<io[, ...])");
  FETCH_HELP_DES(chp,"Clear io pin(s) to 0");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"config(<io>,<mode>[,<pull>[,<otype>[,<ospeed>]]])");
  FETCH_HELP_DES(chp,"Configure pin");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_ARG(chp,"mode","*INPUT | OUTPUT | ALTERNATE");
  FETCH_HELP_ARG(chp,"pull","*FLOATING | PULLUP | PULLDOWN");
  FETCH_HELP_ARG(chp,"otype","*PUSHPULL | OPENDRAIN");
  FETCH_HELP_ARG(chp,"ospeed","2, 25, 50, *100");
  FETCH_HELP_ARG(chp,"*","defaults");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"info(<io>)");
  FETCH_HELP_DES(chp,"Query pin configuration");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"shiftout(<io>,<io_clk>,<rate>,<bits>,<data 0>[,<data 1> ...])");
  FETCH_HELP_DES(chp,"Shift out bits with optional clock");
  FETCH_HELP_ARG(chp,"io","data io pin name");
  FETCH_HELP_ARG(chp,"io_clk", "clock io pine name | none");
  FETCH_HELP_ARG(chp,"rate", "clock rate");
  FETCH_HELP_ARG(chp,"bits", "number of bits to output");
  FETCH_HELP_ARG(chp,"data", "list of bytes {*}");
  FETCH_HELP_ARG(chp,"*", "bytes are clocked out MSB first");
  FETCH_HELP_BREAK(chp);

	return true;
}

bool fetch_gpio_shift_out_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MIN_ARGS(chp, argc, 5);

  port_pin_t pp_io;
  port_pin_t pp_clk;
  uint32_t rate;
  uint32_t bits;
  uint32_t byte_count;

  if( !fetch_gpio_parser(argv[0], FETCH_MAX_DATA_STRLEN, &pp_io) )
  {
    util_message_error(chp, "invalid io pin");
    return false;
  }
  if( !fetch_gpio_parser(argv[1], FETCH_MAX_DATA_STRLEN, &pp_clk) )
  {
    if( strcasecmp(argv[1], "none") == 0 )
    {
      pp_clk.port = NULL;
    }
    else if( strcasecmp(argv[1], "null") == 0 )
    {
      pp_clk.port = NULL;
    }
    else
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }
  }
  if( !util_parse_uint32(argv[2], &rate) || rate == 0 || rate > 500 )
  {
    util_message_error(chp, "invalid clock rate");
    return false;
  }
  if( !util_parse_uint32(argv[3], &bits) || bits == 0 )
  {
    util_message_error(chp, "invalid bit count");
    return false;
  }

  if( bits > ((argc-4) * 8) )
  {
    util_message_error(chp, "invalid bit count, missing data");
    return false;
  }
  
  if( bits > (GPIO_BUFFER_SIZE * 8) )
  {
    util_message_error(chp, "invalid bit count, not enough buffer space");
    return false;
  }

  byte_count = 0;

  for( uint32_t i = 4; i < argc && byte_count < GPIO_BUFFER_SIZE; i++ )
  {
    if( !util_parse_uint8(argv[i], &gpio_buffer[byte_count]) )
    {
      util_message_error(chp, "invalid byte value, data[%d] = %s", byte_count, argv[i]);
      return false;
    }
    else
    {
      byte_count++;
    }
  }

  // this is half the clock period
  uint32_t ms_delay = 500 / rate;

  if( pp_clk.port )
  {
    palClearPad(pp_clk.port, pp_clk.pin);
  }
  palClearPad(pp_io.port, pp_io.pin);

  chThdSleepMilliseconds(ms_delay);

  for( uint32_t i = 0; i < byte_count && bits > 0; i++ )
  {
    for( uint32_t b = 0; b < 8; b++ )
    {
      if( bits > 0 )
      {
        if( gpio_buffer[i] & (1<<(7-b)) )
        {
          palSetPad(pp_io.port, pp_io.pin);
        }
        else
        {
          palClearPad(pp_io.port, pp_io.pin);
        }
        if( pp_clk.port )
        {
          chThdSleepMilliseconds(ms_delay);
          palSetPad(pp_clk.port, pp_clk.pin);
          chThdSleepMilliseconds(ms_delay);
          palClearPad(pp_clk.port, pp_clk.pin);
        }
        else
        {
          chThdSleepMilliseconds(ms_delay * 2);
        }
        bits--;
      }
    }
  }

  return true;
}

bool fetch_gpio_read_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }
    
    util_message_bool(chp, *argv, palReadPad(pp.port, pp.pin));

    argv++;
  }
  
  return true;
}

bool fetch_gpio_read_latch_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }
    
    util_message_bool(chp, *argv, palReadLatch(pp.port) & (1<<pp.pin));

    argv++;
  }
  return true;
}

bool fetch_gpio_read_port_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  ioportid_t port;
  uint16_t port_state;

  if( !fetch_gpio_port_parser(argv[0], FETCH_MAX_DATA_STRLEN, &port) )
  {
    util_message_error(chp, "invalid port");
    return false;
  }

  port_state = palReadPort(port);
  util_message_hex_uint16(chp, "state", port_state);
  return true;
}

bool fetch_gpio_read_port_latch_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  ioportid_t port;
  uint16_t port_state;

  if( !fetch_gpio_port_parser(argv[0], FETCH_MAX_DATA_STRLEN, &port) )
  {
    util_message_error(chp, "invalid port");
    return false;
  }

  port_state = palReadLatch(port);
  util_message_hex_uint16(chp, "state", port_state);
  return true;
}


bool fetch_gpio_read_all_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  uint16_t port_state;

  port_state = palReadPort(GPIOA);
  util_message_hex_uint16(chp, "a", port_state);
  port_state = palReadPort(GPIOB);
  util_message_hex_uint16(chp, "b", port_state);
  port_state = palReadPort(GPIOC);
  util_message_hex_uint16(chp, "c", port_state);
  port_state = palReadPort(GPIOD);
  util_message_hex_uint16(chp, "d", port_state);
  port_state = palReadPort(GPIOE);
  util_message_hex_uint16(chp, "e", port_state);
  port_state = palReadPort(GPIOF);
  util_message_hex_uint16(chp, "f", port_state);
  port_state = palReadPort(GPIOG);
  util_message_hex_uint16(chp, "g", port_state);
  port_state = palReadPort(GPIOH);
  util_message_hex_uint16(chp, "h", port_state);
  port_state = palReadPort(GPIOI);
  util_message_hex_uint16(chp, "i", port_state);

  return true;
}

bool fetch_gpio_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 2);
  FETCH_MOD_ARGS(chp, argc, 2);

  port_pin_t pp;
  bool state;

  // FIXME change this to output all values at the same time

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    argv++;

    if( *argv == NULL )
    {
      util_message_error(chp, "missing logic state");
      return false;
    }

    if( !util_parse_bool(argv[1], &state) )
    {
      util_message_error(chp, "invalid logic state");
      return false;
    }
    
    palWritePad(pp.port, pp.pin, state);
    
    argv++;
  }
  
  return true;
}

bool fetch_gpio_write_port_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 2);

  ioportid_t port;
  uint16_t port_state;
  uint16_t set_mask, clear_mask;

  if( !fetch_gpio_port_parser(argv[0], FETCH_MAX_DATA_STRLEN, &port) )
  {
    util_message_error(chp, "invalid port");
    return false;
  }

  if( !util_parse_uint16(argv[1], &port_state) )
  {
    util_message_error(chp, "invalid port output value");
    return false;
  }

  set_mask = port_state;
  clear_mask = ~port_state;

  switch( (uint32_t)port )
  {
    case GPIOA_BASE:
      set_mask &= PORT_A_GPIO_MASK;
      clear_mask &= PORT_A_GPIO_MASK;
      GPIOA->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOB_BASE:
      set_mask &= PORT_B_GPIO_MASK;
      clear_mask &= PORT_B_GPIO_MASK;
      GPIOB->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOC_BASE:
      set_mask &= PORT_C_GPIO_MASK;
      clear_mask &= PORT_C_GPIO_MASK;
      GPIOC->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOD_BASE:
      set_mask &= PORT_D_GPIO_MASK;
      clear_mask &= PORT_D_GPIO_MASK;
      GPIOD->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOE_BASE:
      set_mask &= PORT_E_GPIO_MASK;
      clear_mask &= PORT_E_GPIO_MASK;
      GPIOE->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOF_BASE:
      set_mask &= PORT_F_GPIO_MASK;
      clear_mask &= PORT_F_GPIO_MASK;
      GPIOF->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOG_BASE:
      set_mask &= PORT_G_GPIO_MASK;
      clear_mask &= PORT_G_GPIO_MASK;
      GPIOG->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOH_BASE:
      set_mask &= PORT_H_GPIO_MASK;
      clear_mask &= PORT_H_GPIO_MASK;
      GPIOH->BSRR.W = (clear_mask << 16) | set_mask;
      break;
    case GPIOI_BASE:
      set_mask &= PORT_I_GPIO_MASK;
      clear_mask &= PORT_I_GPIO_MASK;
      GPIOI->BSRR.W = (clear_mask << 16) | set_mask;
      break;
  }

  return true;
}

bool fetch_gpio_set_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;

  // FIXME change this to output all values at the same time

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    palSetPad(pp.port, pp.pin);
    
    argv++;
  }
  
  return true;
}

bool fetch_gpio_clear_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;

  // FIXME change this to output all values at the same time

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    palClearPad(pp.port, pp.pin);
    
    argv++;
  }
  
  return true;
}

bool fetch_gpio_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 5);
  FETCH_MIN_ARGS(chp, argc, 2);

  port_pin_t pp;

  if( !fetch_gpio_parser(argv[0], FETCH_MAX_DATA_STRLEN, &pp) )
  {
    util_message_error(chp, "invalid io pin");
    return false;
  }

  if( !valid_gpio_port_pin(pp.port, pp.pin) )
  {
    util_message_error(chp, "port/pin not available as gpio");
    return false;
  }

  uint32_t mode;
  uint32_t pupdr = PAL_STM32_PUPDR_FLOATING;
  uint32_t otype = PAL_STM32_OTYPE_PUSHPULL;
  uint32_t ospeed = PAL_STM32_OSPEED_HIGHEST;

  const str_table_t mode_table[] = {
    {"INPUT", PAL_STM32_MODE_INPUT},
    {"OUTPUT", PAL_STM32_MODE_OUTPUT},
    {NULL, 0}
  };
  const str_table_t pupdr_table[] = {
    {"FLOATING", PAL_STM32_PUPDR_FLOATING},
    {"PULLUP", PAL_STM32_PUPDR_PULLUP},
    {"PULLDOWN", PAL_STM32_PUPDR_PULLDOWN},
    {NULL, 0}
  };
  const str_table_t otype_table[] = {
    {"PUSHPULL", PAL_STM32_OTYPE_PUSHPULL},
    {"OPENDRAIN", PAL_STM32_OTYPE_OPENDRAIN},
    {NULL, 0}
  };
  const str_table_t ospeed_table[] = {
    {"0", PAL_STM32_OSPEED_LOWEST},
    {"1", PAL_STM32_OSPEED_MID1},
    {"2", PAL_STM32_OSPEED_MID2},
    {"3", PAL_STM32_OSPEED_HIGHEST},
    {NULL, 0}
  };

  if( !util_match_str_table(argv[1], &mode, mode_table) )
  {
    util_message_error(chp, "invalid mode");
    return false;
  }
  if( argv[2] != NULL )
  {
    if( !util_match_str_table(argv[2], &pupdr, pupdr_table) )
    {
      util_message_error(chp, "invalid pull");
      return false;
    }
    if( argv[3] != NULL )
    {
      if( !util_match_str_table(argv[3], &otype, otype_table) )
      {
        util_message_error(chp, "invalid otype");
        return false;
      }
      if( argv[4] != NULL )
      {
        if( !util_match_str_table(argv[4], &ospeed, ospeed_table) )
        {
          util_message_error(chp, "invalid ospeed");
          return false;
        }
      }
    }
  }

  palSetPadMode(pp.port, pp.pin, mode | pupdr | otype | ospeed);

  return true;
}

bool fetch_gpio_info_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  uint32_t alt_func;
  port_pin_t pp;
  bool pin_state;

  if( !fetch_gpio_parser(argv[0], FETCH_MAX_DATA_STRLEN, &pp) )
  {
    util_message_error(chp, "invalid io pin");
    return false;
  }

  // FIXME change this to print out mode, pull, otype, ospeed, and alternate function

  switch((pp.port->MODER >> (pp.pin*2)) & 3)
  {
    case 0:
      util_message_string(chp, "mode", "INPUT");
      break;
    case 1:
      if( pp.port->OTYPER & (1<<pp.pin) )
      {
        util_message_string(chp, "mode", "OUTPUT_OPENDRAIN");
      }
      else
      {
        util_message_string(chp, "mode", "OUTPUT_PUSHPULL");
      }
      break;
    case 2:
      util_message_string(chp, "mode", "ALTERNATE");
      if( pp.pin >= 8 )
      {
        alt_func = (pp.port->AFRL >> (pp.pin*4)) & 0xf;
      }
      else
      {
        alt_func = (pp.port->AFRH >> ((pp.pin-8)*4)) & 0xf;
      }
      util_message_uint32(chp, "alt_func", alt_func);
      break;
    case 3:
      util_message_string(chp, "mode", "ANALOG");
      break;
  }

  switch( (pp.port->PUPDR >> (pp.pin*2)) & 3)
  {
    case 0:
      util_message_string(chp, "pull", "NONE");
      break;
    case 1:
      util_message_string(chp, "pull", "UP");
      break;
    case 2:
      util_message_string(chp, "pull", "DOWN");
      break;
    case 3:
      util_message_string(chp, "pull", "RESERVED");
      break;
  }

  pin_state = palReadLatch(pp.port) & (1<<pp.pin);

  util_message_bool(chp, "latch_state", pin_state);

  pin_state = palReadPort(pp.port) & (1<<pp.pin);
  
  util_message_bool(chp, "pin_state", pin_state);

  return true;
}

bool fetch_gpio_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  return fetch_gpio_reset(chp);
}

void fetch_gpio_init(void)
{
  // Init stuff goes here
  // ...
}

bool fetch_gpio_reset( BaseSequentialStream * chp )
{
  // reset all gpio pins
  for(uint32_t pin = 0; pin < 16; pin++ )
  {
    if( PORT_A_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOA, pin, DEFAULT_PIN_MODE);
    if( PORT_B_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOB, pin, DEFAULT_PIN_MODE);
    if( PORT_C_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOC, pin, DEFAULT_PIN_MODE);
    if( PORT_D_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOD, pin, DEFAULT_PIN_MODE);
    if( PORT_E_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOE, pin, DEFAULT_PIN_MODE);
    if( PORT_F_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOF, pin, DEFAULT_PIN_MODE);
    if( PORT_G_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOG, pin, DEFAULT_PIN_MODE);
    if( PORT_H_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOH, pin, DEFAULT_PIN_MODE);
    if( PORT_I_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOI, pin, DEFAULT_PIN_MODE);
  }

  return true;
}

/*! @} */

