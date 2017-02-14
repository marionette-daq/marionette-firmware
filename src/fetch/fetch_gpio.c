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

static void read_all( port_states_t * states )
{
  states->a = palReadPort(GPIOA);
  states->b = palReadPort(GPIOB);
  states->c = palReadPort(GPIOC);
  states->d = palReadPort(GPIOD);
  states->e = palReadPort(GPIOE);
  states->f = palReadPort(GPIOF);
  states->g = palReadPort(GPIOG);
  states->h = palReadPort(GPIOH);
  states->i = palReadPort(GPIOI);
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
  FETCH_HELP_CMD(chp,"read_latch(<io>[, ...])");
  FETCH_HELP_DES(chp,"Read pin output latch(s)");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read_port_latch(<port>)");
  FETCH_HELP_DES(chp,"Read port output latch");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read_port(<port>)");
  FETCH_HELP_DES(chp,"Read port input values");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"read_all");
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
  FETCH_HELP_CMD(chp,"write_port(<port>, <value>)");
  FETCH_HELP_DES(chp,"Write to port");
  FETCH_HELP_ARG(chp,"port","A | B | C | D | E | F | G | H | I");
  FETCH_HELP_ARG(chp,"value","16bit value to write");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"write_all(<val_a>, ..., <val_i>)");
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
  FETCH_HELP_ARG(chp,"pull","*NONE | PULLUP | PULLDOWN");
  FETCH_HELP_ARG(chp,"otype","*PUSHPULL | OPENDRAIN");
  FETCH_HELP_ARG(chp,"ospeed","*0 ... 3");
  FETCH_HELP_ARG(chp,"*","gpio defaults, alternate defaults may differ");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"info(<io>)");
  FETCH_HELP_DES(chp,"Query pin configuration");
  FETCH_HELP_ARG(chp,"io", "io pin name");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp,"shiftout(<io>,<io_clk>,<rate>,<bits>,<data 0>[,<data 1> ...])");
  FETCH_HELP_DES(chp,"Shift out bits with optional clock");
  FETCH_HELP_ARG(chp,"io","data io pin name");
  FETCH_HELP_ARG(chp,"io_clk", "clock io pine name | NONE");
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

  if( !valid_gpio_port_pin(pp_io.port, pp_io.pin) )
  {
    util_message_error(chp, "restricted access data io pin");
    return false;
  }

  if( !fetch_gpio_parser(argv[1], FETCH_MAX_DATA_STRLEN, &pp_clk) )
  {
    if( strcasecmp(argv[1], "none") == 0 )
    {
      pp_clk.port = NULL;
    }
    else
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }
  }
  
  if( !valid_gpio_port_pin(pp_clk.port, pp_clk.pin) )
  {
    util_message_error(chp, "restricted access clock io pin");
    return false;
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

  if( bits > (FETCH_SHARED_BUFFER_SIZE * 8) )
  {
    util_message_error(chp, "invalid bit count, not enough buffer space");
    return false;
  }

  byte_count = 0;

  if( !fetch_parse_bytes(chp, argc-4, &argv[4], fetch_shared_buffer, FETCH_SHARED_BUFFER_SIZE, &byte_count) )
  {
    util_message_error(chp, "fetch_parse_bytes failed");
    return false;
  }
  
  if( bits > (byte_count * 8) )
  {
    util_message_error(chp, "invalid bit count, missing data");
    return false;
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
        if( fetch_shared_buffer[i] & (1<<(7-b)) )
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

  port_states_t states;
  
  read_all(&states);

  port_pin_t pp;

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }
    
    switch( (uint32_t)pp.port )
    {
      case GPIOA_BASE:
        util_message_bool(chp, *argv, (states.a >> pp.pin) & 1);
        break;
      case GPIOB_BASE:
        util_message_bool(chp, *argv, (states.b >> pp.pin) & 1);
        break;
      case GPIOC_BASE:
        util_message_bool(chp, *argv, (states.c >> pp.pin) & 1);
        break;
      case GPIOD_BASE:
        util_message_bool(chp, *argv, (states.d >> pp.pin) & 1);
        break;
      case GPIOE_BASE:
        util_message_bool(chp, *argv, (states.e >> pp.pin) & 1);
        break;
      case GPIOF_BASE:
        util_message_bool(chp, *argv, (states.f >> pp.pin) & 1);
        break;
      case GPIOG_BASE:
        util_message_bool(chp, *argv, (states.g >> pp.pin) & 1);
        break;
      case GPIOH_BASE:
        util_message_bool(chp, *argv, (states.h >> pp.pin) & 1);
        break;
      case GPIOI_BASE:
        util_message_bool(chp, *argv, (states.i >> pp.pin) & 1);
        break;
    }

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

  port_states_t states;
  
  read_all(&states);

  util_message_hex_uint16(chp, "a", states.a);
  util_message_hex_uint16(chp, "b", states.b);
  util_message_hex_uint16(chp, "c", states.c);
  util_message_hex_uint16(chp, "d", states.d);
  util_message_hex_uint16(chp, "e", states.e);
  util_message_hex_uint16(chp, "f", states.f);
  util_message_hex_uint16(chp, "g", states.g);
  util_message_hex_uint16(chp, "h", states.h);
  util_message_hex_uint16(chp, "i", states.i);

  return true;
}

bool fetch_gpio_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 2);
  FETCH_MOD_ARGS(chp, argc, 2);

  port_pin_t pp;
  bool state;

  port_states_t set_mask = {0,0,0,0,0,0,0,0,0};
  port_states_t clear_mask = {0,0,0,0,0,0,0,0,0};

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    if( !valid_gpio_port_pin(pp.port,pp.pin) )
    {
      util_message_error(chp, "restricted access io pin");
      return false;
    }

    argv++;

    if( *argv == NULL )
    {
      util_message_error(chp, "missing logic state");
      return false;
    }

    if( !util_parse_bool(*argv, &state) )
    {
      util_message_error(chp, "invalid logic state");
      return false;
    }
    
    if( state )
    {
      switch( (uint32_t)pp.port )
      {
        case GPIOA_BASE:
          set_mask.a |= (1<<pp.pin);
          break;
        case GPIOB_BASE:
          set_mask.b |= (1<<pp.pin);
          break;
        case GPIOC_BASE:
          set_mask.c |= (1<<pp.pin);
          break;
        case GPIOD_BASE:
          set_mask.d |= (1<<pp.pin);
          break;
        case GPIOE_BASE:
          set_mask.e |= (1<<pp.pin);
          break;
        case GPIOF_BASE:
          set_mask.f |= (1<<pp.pin);
          break;
        case GPIOG_BASE:
          set_mask.g |= (1<<pp.pin);
          break;
        case GPIOH_BASE:
          set_mask.h |= (1<<pp.pin);
          break;
        case GPIOI_BASE:
          set_mask.i |= (1<<pp.pin);
          break;
      }
    }
    else
    {
      switch( (uint32_t)pp.port )
      {
        case GPIOA_BASE:
          clear_mask.a |= (1<<pp.pin);
          break;
        case GPIOB_BASE:
          clear_mask.b |= (1<<pp.pin);
          break;
        case GPIOC_BASE:
          clear_mask.c |= (1<<pp.pin);
          break;
        case GPIOD_BASE:
          clear_mask.d |= (1<<pp.pin);
          break;
        case GPIOE_BASE:
          clear_mask.e |= (1<<pp.pin);
          break;
        case GPIOF_BASE:
          clear_mask.f |= (1<<pp.pin);
          break;
        case GPIOG_BASE:
          clear_mask.g |= (1<<pp.pin);
          break;
        case GPIOH_BASE:
          clear_mask.h |= (1<<pp.pin);
          break;
        case GPIOI_BASE:
          clear_mask.i |= (1<<pp.pin);
          break;
      }
    }
    
    argv++;
  }

  write_all(set_mask,clear_mask);
  
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

bool fetch_gpio_write_all_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 9);
  FETCH_MAX_ARGS(chp, argc, 9);

  port_states_t set_mask = {0,0,0,0,0,0,0,0,0};
  port_states_t clear_mask = {0,0,0,0,0,0,0,0,0};

  if( !util_parse_uint16(argv[0], &set_mask.a) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[1], &set_mask.b) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[2], &set_mask.c) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[3], &set_mask.d) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[4], &set_mask.e) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[5], &set_mask.f) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[6], &set_mask.g) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[7], &set_mask.h) )
  {
    util_message_error(chp, "invalid value");
  }
  if( !util_parse_uint16(argv[8], &set_mask.i) )
  {
    util_message_error(chp, "invalid value");
  }

  clear_mask.a = ~set_mask.a;
  clear_mask.b = ~set_mask.b;
  clear_mask.c = ~set_mask.c;
  clear_mask.d = ~set_mask.d;
  clear_mask.e = ~set_mask.e;
  clear_mask.f = ~set_mask.f;
  clear_mask.g = ~set_mask.g;
  clear_mask.h = ~set_mask.h;
  clear_mask.i = ~set_mask.i;

  write_all(set_mask,clear_mask);

  return true;
}

bool fetch_gpio_set_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;
  
  port_states_t set_mask = {0,0,0,0,0,0,0,0,0};
  port_states_t clear_mask = {0,0,0,0,0,0,0,0,0};

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    if( !valid_gpio_port_pin(pp.port,pp.pin) )
    {
      util_message_error(chp, "restricted access io pin");
    }
    
    switch((uint32_t)pp.port)
    {
      case GPIOA_BASE:
        set_mask.a |= (1<<pp.pin);
        break;
      case GPIOB_BASE:
        set_mask.b |= (1<<pp.pin);
        break;
      case GPIOC_BASE:
        set_mask.c |= (1<<pp.pin);
        break;
      case GPIOD_BASE:
        set_mask.d |= (1<<pp.pin);
        break;
      case GPIOE_BASE:
        set_mask.e |= (1<<pp.pin);
        break;
      case GPIOF_BASE:
        set_mask.f |= (1<<pp.pin);
        break;
      case GPIOG_BASE:
        set_mask.g |= (1<<pp.pin);
        break;
      case GPIOH_BASE:
        set_mask.h |= (1<<pp.pin);
        break;
      case GPIOI_BASE:
        set_mask.i |= (1<<pp.pin);
        break;
    }
    
    argv++;
  }

  write_all(set_mask, clear_mask);
  
  return true;
}

bool fetch_gpio_clear_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 1);

  port_pin_t pp;

  port_states_t set_mask = {0,0,0,0,0,0,0,0,0};
  port_states_t clear_mask = {0,0,0,0,0,0,0,0,0};

  while( *argv != NULL )
  {
    if( !fetch_gpio_parser(*argv, FETCH_MAX_DATA_STRLEN, &pp) )
    {
      util_message_error(chp, "invalid io pin");
      return false;
    }

    if( !valid_gpio_port_pin(pp.port,pp.pin) )
    {
      util_message_error(chp, "restricted access io pin");
    }
    
    switch((uint32_t)pp.port)
    {
      case GPIOA_BASE:
        clear_mask.a |= (1<<pp.pin);
        break;
      case GPIOB_BASE:
        clear_mask.b |= (1<<pp.pin);
        break;
      case GPIOC_BASE:
        clear_mask.c |= (1<<pp.pin);
        break;
      case GPIOD_BASE:
        clear_mask.d |= (1<<pp.pin);
        break;
      case GPIOE_BASE:
        clear_mask.e |= (1<<pp.pin);
        break;
      case GPIOF_BASE:
        clear_mask.f |= (1<<pp.pin);
        break;
      case GPIOG_BASE:
        clear_mask.g |= (1<<pp.pin);
        break;
      case GPIOH_BASE:
        clear_mask.h |= (1<<pp.pin);
        break;
      case GPIOI_BASE:
        clear_mask.i |= (1<<pp.pin);
        break;
    }
      
    argv++;
  }
  
  write_all(set_mask, clear_mask);
  
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
  uint32_t ospeed = PAL_STM32_OSPEED_LOWEST;

  const str_table_t mode_table[] = {
    {"DEFAULT", PAL_STM32_MODE_INPUT},
    {"INPUT", PAL_STM32_MODE_INPUT},
    {"OUTPUT", PAL_STM32_MODE_OUTPUT},
    {"ALTERNATE", PAL_STM32_MODE_ALTERNATE},
    {NULL, 0}
  };
  const str_table_t pupdr_table[] = {
    {"DEFAULT", PAL_STM32_PUPDR_FLOATING},
    {"NONE", PAL_STM32_PUPDR_FLOATING},
    {"PULLUP", PAL_STM32_PUPDR_PULLUP},
    {"PULLDOWN", PAL_STM32_PUPDR_PULLDOWN},
    {NULL, 0}
  };
  const str_table_t otype_table[] = {
    {"DEFAULT", PAL_STM32_OTYPE_PUSHPULL},
    {"PUSHPULL", PAL_STM32_OTYPE_PUSHPULL},
    {"OPENDRAIN", PAL_STM32_OTYPE_OPENDRAIN},
    {NULL, 0}
  };
  const str_table_t ospeed_table[] = {
    {"DEFAULT", PAL_STM32_OSPEED_LOWEST},
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
  if( argc > 2 && !util_match_str_table(argv[2], &pupdr, pupdr_table) )
  {
    util_message_error(chp, "invalid pull");
    return false;
  }
  if( argc > 3 && !util_match_str_table(argv[3], &otype, otype_table) )
  {
    util_message_error(chp, "invalid otype");
    return false;
  }
  if( argc > 4 && !util_match_str_table(argv[4], &ospeed, ospeed_table) )
  {
    util_message_error(chp, "invalid ospeed");
    return false;
  }

  if( mode == PAL_STM32_MODE_ALTERNATE )
  {
    if( argc > 2 && util_match_str(argv[2], "DEFAULT") )
    {
      pupdr = -1;
    }
    if( argc > 3 && util_match_str(argv[3], "DEFAULT") )
    {
      otype = -1;
    }
    if( argc > 4 && util_match_str(argv[4], "DEFAULT") )
    {
      ospeed = -1;
    }

    if( !set_alternate_mode_ext( pp.port, pp.pin, pupdr, otype, ospeed ) )
    {
      util_message_error(chp, "unable to set alternate mode for pin");
      return false;
    }
  }
  else
  {
    palSetPadMode(pp.port, pp.pin, mode | pupdr | otype | ospeed);
  }

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

  switch((pp.port->MODER >> (pp.pin*2)) & 3)
  {
    case 0:
      util_message_string_format(chp, "mode", "input");
      break;
    case 1:
      util_message_string_format(chp, "mode", "output");
      break;
    case 2:
      util_message_string_format(chp, "mode", "alternate");
      if( pp.pin < 8 )
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
      util_message_string_format(chp, "mode", "analog");
      break;
  }

  switch( (pp.port->PUPDR >> (pp.pin*2)) & 3)
  {
    case 0:
      util_message_string_format(chp, "pull", "none");
      break;
    case 1:
      util_message_string_format(chp, "pull", "pullup");
      break;
    case 2:
      util_message_string_format(chp, "pull", "pulldown");
      break;
    case 3:
      util_message_string_format(chp, "pull", "reserved");
      break;
  }
      
  if( pp.port->OTYPER & (1<<pp.pin) )
  {
    util_message_string_format(chp, "otype", "opendrain");
  }
  else
  {
    util_message_string_format(chp, "otype", "pushpull");
  }

  util_message_uint32(chp, "ospeed", (pp.port->OSPEEDR >> (pp.pin*2)) & 3);

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
      palSetPadMode(GPIOA, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_B_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOB, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_C_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOC, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_D_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOD, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_E_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOE, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_F_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOF, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_G_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOG, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_H_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOH, pin, FETCH_DEFAULT_PIN_MODE);
    if( PORT_I_GPIO_MASK & (1<<pin) )
      palSetPadMode(GPIOI, pin, FETCH_DEFAULT_PIN_MODE);
  }

  return true;
}

/*! @} */

