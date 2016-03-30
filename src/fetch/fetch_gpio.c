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

#include "fetch_defs.h"
#include "fetch_gpio.h"
#include "fetch.h"


static port_pin_t gpio_pins[] = {
    {GPIOA, GPIOA_PIN15}, // TIM2_CH1
    {GPIOB, GPIOB_PIN8},  // TIM4_CH3, TIM10_CH1
    {GPIOB, GPIOB_PIN9},  // TIM4_CH4, TIM11_CH1
    {GPIOB, GPIOB_PIN14}, // TIM1_CH2N, TIM8_CH2N
    {GPIOB, GPIOB_PIN15}, // TIM1_CH3N, TIM8_CH3N
    {GPIOC, GPIOC_PIN6},  // TIM3_CH1, TIM8_CH1
    {GPIOC, GPIOC_PIN13}, // 
    {GPIOD, GPIOD_PIN0},  // 
    {GPIOD, GPIOD_PIN1},  //
    {GPIOD, GPIOD_PIN3},  //
    {GPIOD, GPIOD_PIN4},  //
    {GPIOD, GPIOD_PIN5},  //
    {GPIOD, GPIOD_PIN6},  //
    {GPIOD, GPIOD_PIN7},  //
    {GPIOD, GPIOD_PIN8},  //
    {GPIOD, GPIOD_PIN9},  //
    {GPIOD, GPIOD_PIN10}, //
    {GPIOD, GPIOD_PIN11}, //
    {GPIOD, GPIOD_PIN12}, // TIM4_CH1
    {GPIOE, GPIOE_PIN0},  // 
    {GPIOE, GPIOE_PIN1},  //
    {GPIOE, GPIOE_PIN2},  //
    {GPIOE, GPIOE_PIN3},  //
    {GPIOE, GPIOE_PIN4},  //
    {GPIOE, GPIOE_PIN5},  // TIM9_CH1
    {GPIOE, GPIOE_PIN6},  // TIM9_CH2
    {GPIOE, GPIOE_PIN7},  //
    {GPIOE, GPIOE_PIN8},  // TIM1_CH1N
    {GPIOE, GPIOE_PIN9},  // TIM1_CH1
    {GPIOE, GPIOE_PIN10}, // TIM1_CH2N
    {GPIOE, GPIOE_PIN15}, //
    {GPIOF, GPIOF_PIN11}, //
    {GPIOF, GPIOF_PIN12}, //
    {GPIOF, GPIOF_PIN13}, //
    {GPIOF, GPIOF_PIN14}, //
    {GPIOF, GPIOF_PIN15}, //
    {GPIOG, GPIOG_PIN0},  //
    {GPIOG, GPIOG_PIN1},  //
    {GPIOG, GPIOG_PIN2},  //
    {GPIOG, GPIOG_PIN3},  //
    {GPIOG, GPIOG_PIN4},  //
    {GPIOG, GPIOG_PIN5},  //
    {GPIOG, GPIOG_PIN6},  //
    {GPIOG, GPIOG_PIN7},  //
    {GPIOG, GPIOG_PIN8},  //
    {GPIOH, GPIOH_PIN2},  //
    {GPIOH, GPIOH_PIN3},  //
    {GPIOH, GPIOH_PIN5},  //
    {GPIOH, GPIOH_PIN6},  //
    {GPIOH, GPIOH_PIN9},  //
    {GPIOH, GPIOH_PIN10}, // TIM5_CH1
    {GPIOH, GPIOH_PIN11}, // TIM5_CH2
    {GPIOH, GPIOH_PIN12}, // TIM5_CH3
    {GPIOH, GPIOH_PIN14}, // TIM8_CH2N
    {GPIOH, GPIOH_PIN15}, // TIM8_CH3N
    {GPIOI, GPIOI_PIN0},  // TIM5_CH4
    {GPIOI, GPIOI_PIN4},  //
    {GPIOI, GPIOI_PIN8},  //
    {GPIOI, GPIOI_PIN10}, //
    {GPIOI, GPIOI_PIN11}, //
    {GPIOI, GPIOI_PIN12}, //
    {GPIOI, GPIOI_PIN13}, //
    {GPIOI, GPIOI_PIN14}, //
    {GPIOI, GPIOI_PIN15}  //
  };

typedef enum {
  WAIT_EVENT_HIGH = 0,
  WAIT_EVENT_LOW,
  WAIT_EVENT_RISING,
  WAIT_EVENT_FALLING
} wait_event_t;

// list all command function prototypes here 
static bool fetch_gpio_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_read_port_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_read_all_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_set_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_clear_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_info_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_reset_all_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_wait_cmd(BaseSequentialStream * chp, char *cmd_list[], char * data_list[]);
static bool fetch_gpio_heartbeat_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char gpio_config_help_string[] = "Configure pin as GPIO\n" \
                                              "Usage: config(<port>,<pin>,<mode>)\n" \
                                              "\tmode = INPUT_FLOATING, INPUT_PULLUP, INPUT_PULLDOWN,\n" \
                                              "\t       OUTPUT_PUSHPULL, OUTPUT_OPENDRAIN";
static const char gpio_wait_help_string[] = "Wait for the given event on a gpio pin\n" \
                                            "Usage: wait(<port>,<pin>,<event>,<timeout>)\n" \
                                            "\tevent = HIGH, LOW, RISING, FALLING\n" \
                                            "\ttimeout = <milliseconds>\n";

static fetch_command_t fetch_gpio_commands[] = {
    { fetch_gpio_help_cmd,        "help",       "Display GPIO help"},
    { fetch_gpio_read_cmd,        "read",       "Read pin state\nUsage: read(<port>,<pin>)" },
    { fetch_gpio_read_port_cmd,   "readport",   "Read state of all pins on port\nUsage: readport(<port>)" },
    { fetch_gpio_read_all_cmd,    "readall",    "Read state of all pins on all ports" },
    { fetch_gpio_write_cmd,       "write",      "Write state to pin\nUsage: write(<port>,<pin>,<state>)" },
    { fetch_gpio_set_cmd,         "set",        "Set pin to 1\nUsage: set(<port>,<pin>)" },
    { fetch_gpio_clear_cmd,       "clear",      "Clear pin to 0\nUsage: clear(<port>,<pin>)" },
    { fetch_gpio_wait_cmd,        "wait",       gpio_wait_help_string },
    { fetch_gpio_config_cmd,      "config",     gpio_config_help_string },
    { fetch_gpio_info_cmd,        "info",       "Get pin info\nUsage: info(<port>,<pin>)" },
    { fetch_gpio_reset_all_cmd,   "resetall",   "Reset all GPIO pins to defaults" },
    { NULL, NULL, NULL } // null terminate list
  };

static const char * pin_state_tok[] = {"true","false","1","0","high","low","set","clear"};
static const char * pin_mode_tok[] = {"INPUT_FLOATING","INPUT_PULLUP","INPUT_PULLDOWN",
                                      "OUTPUT_PUSHPULL","OUTPUT_OPENDRAIN"};
static const char * wait_event_tok[] = {"HIGH","1","LOW","0","RISING","FALLING"};

static bool valid_gpio_port_pin( ioportid_t port, uint32_t pin )
{
  for(uint32_t i = 0; i < NELEMS(gpio_pins); i++ )
  {
    if( gpio_pins[i].port == port && gpio_pins[i].pin == pin )
    {
      return true;
    }
  }
  return false;
}

static bool fetch_gpio_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Fetch GPIO Help:");
  fetch_display_help(chp, fetch_gpio_commands);
	return true;
}

static bool fetch_gpio_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  bool pin_state;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  pin_state = palReadPad(port, pin);
  util_message_bool(chp, "state", pin_state);
  return true;
}

static bool fetch_gpio_read_port_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint16_t port_state;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  if( port == NULL )
  {
    util_message_error(chp, "invalid port");
    return false;
  }

  port_state = palReadPort(port);
  util_message_uint16(chp, "state", &port_state, 1);
  return true;
}

static bool fetch_gpio_read_all_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  uint16_t port_state;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  port_state = palReadPort(GPIOA);
  util_message_uint16(chp, "a", &port_state, 1);
  port_state = palReadPort(GPIOB);
  util_message_uint16(chp, "b", &port_state, 1);
  port_state = palReadPort(GPIOC);
  util_message_uint16(chp, "c", &port_state, 1);
  port_state = palReadPort(GPIOD);
  util_message_uint16(chp, "d", &port_state, 1);
  port_state = palReadPort(GPIOE);
  util_message_uint16(chp, "e", &port_state, 1);
  port_state = palReadPort(GPIOF);
  util_message_uint16(chp, "f", &port_state, 1);
  port_state = palReadPort(GPIOG);
  util_message_uint16(chp, "g", &port_state, 1);
  port_state = palReadPort(GPIOH);
  util_message_uint16(chp, "h", &port_state, 1);
  port_state = palReadPort(GPIOI);
  util_message_uint16(chp, "i", &port_state, 1);

  return true;
}

static bool fetch_gpio_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  switch( token_match( data_list[2], FETCH_MAX_DATA_STRLEN, pin_state_tok, NELEMS(pin_state_tok)) )
  {
    case 0: // true
    case 2: // 1
    case 4: // high
    case 6: // set
      palSetPad(port,pin);
      break;
    case 1: // false
    case 3: // 0
    case 5: // low
    case 7: // clear
      palClearPad(port,pin);
      break;
    default:
      util_message_error(chp, "invalid logic state");
      return false;
  }
  return true;
}

static bool fetch_gpio_set_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  palSetPad(port,pin);

  return true;
}

static bool fetch_gpio_clear_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  palClearPad(port,pin);

  return true;
}

static bool fetch_gpio_wait_cmd(BaseSequentialStream * chp, char *cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  char * endptr;
  int32_t timeout;
  wait_event_t event;
  systime_t start_time;
  bool state_next, state_prev;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 4) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  switch( token_match( data_list[2], FETCH_MAX_DATA_STRLEN, wait_event_tok, NELEMS(wait_event_tok)) )
  {
    case 0: // HIGH
    case 1: // 1
      event = WAIT_EVENT_HIGH;
      break;
    case 2: // LOW
    case 3: // 0
      event = WAIT_EVENT_LOW;
      break;
    case 4: // RISING
      event = WAIT_EVENT_RISING;
    case 5: // FALLING
      event = WAIT_EVENT_FALLING;
      break;
    default:
      util_message_error(chp, "invalid wait event");
      return false;
  }

  timeout = strtol(data_list[3], &endptr, 0);

  if( *endptr != '\0' || timeout <= 0 )
  {
    util_message_error(chp, "invalid wait timeout");
    return false;
  }

  start_time = chVTGetSystemTime();
  state_next = palReadPad(port,pin);

  while( chVTTimeElapsedSinceX(start_time) < MS2ST(timeout) )
  {
    state_prev = state_next;
    state_next = palReadPad(port,pin);
    switch( event )
    {
      case WAIT_EVENT_HIGH:
        if( state_prev || state_next )
        {
          util_message_bool(chp, "event", true);
          return true;
        }
        break;
      case WAIT_EVENT_LOW:
        if( !state_prev || !state_next )
        {
          util_message_bool(chp, "event", true);
          return true;
        }
        break;
      case WAIT_EVENT_RISING:
        if( !state_prev && state_next )
        {
          util_message_bool(chp, "event", true);
          return true;
        }
        break;
      case WAIT_EVENT_FALLING:
        if( state_prev && !state_next )
        {
          util_message_bool(chp, "event", true);
          return true;
        }
        break;
    }
  }
  util_message_bool(chp, "event", false);
  return true;
}

static bool fetch_gpio_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  iomode_t mode = PAL_STM32_PUPDR_FLOATING | PAL_STM32_MODE_INPUT;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  if( !valid_gpio_port_pin(port, pin) )
  {
    util_message_error(chp, "port/pin not available as gpio");
    return false;
  }

  switch( token_match( data_list[2], FETCH_MAX_DATA_STRLEN, pin_mode_tok, NELEMS(pin_mode_tok)) )
  {
    case 0: // input floating
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING;
      break;
    case 1: // input pullup
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_PULLUP;
      break;
    case 2: // input pulldown
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_PULLDOWN;
      break;
    case 3: // output pushpull
      mode = PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_PUSHPULL;
      break;
    case 4: // output opendrian
      mode = PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_OPENDRAIN;
      break;
    default:
      util_message_error(chp, "invalid pin mode");
      return false;
  }

  palSetPadMode(port, pin, mode);

  return true;
}

static bool fetch_gpio_info_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  uint32_t alt_func;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  switch((port->MODER >> (pin*2)) & 3)
  {
    case 0:
      util_message_string(chp, "mode", "INPUT");
      break;
    case 1:
      if( port->OTYPER & (1<<pin) )
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
      if( pin >= 8 )
      {
        alt_func = (port->AFRL >> (pin*4)) & 0xf;
      }
      else
      {
        alt_func = (port->AFRH >> ((pin-8)*4)) & 0xf;
      }
      util_message_uint32(chp, "alt_func", &alt_func, 1);
      break;
    case 3:
      util_message_string(chp, "mode", "ANALOG");
      break;
  }

  switch( (port->PUPDR >> (pin*2)) & 3)
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

  return true;
}

static bool fetch_gpio_reset_all_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 0) )
  {
    return false;
  }

  return fetch_gpio_reset(chp);
}

void fetch_gpio_init(BaseSequentialStream * chp)
{
  static bool gpio_init_flag = false;

  if( gpio_init_flag )
    return;

  // Init stuff goes here
  // ...

  gpio_init_flag = true;
}

/*! \brief dispatch a specific gpio command
 */
bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_gpio_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}

bool fetch_gpio_reset(BaseSequentialStream * chp)
{
  // reset all gpio pins
  for(uint32_t i = 0; i < NELEMS(gpio_pins); i++ )
  {
    palSetPadMode(gpio_pins[i].port, gpio_pins[i].pin, PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING);
  }

  return true;
}

/*! @} */

