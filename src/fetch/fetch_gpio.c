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
    // GPIO
    {GPIOG, GPIOG_PG0},               // D0
    {GPIOG, GPIOG_PG1},               // D1
    {GPIOG, GPIOG_PG2},               // D2
    {GPIOG, GPIOG_PG3},               // D3
    {GPIOG, GPIOG_PG4},               // D4
    {GPIOG, GPIOG_PG5},               // D5
    {GPIOG, GPIOG_PG6},               // D6
    {GPIOG, GPIOG_PG7},               // D7
    {GPIOE, GPIOE_PE0_TIM4_ETR},      // D8
    {GPIOE, GPIOE_PE7_TIM1_ETR},      // D9
    {GPIOE, GPIOE_PE8},               // D10
    {GPIOF, GPIOF_PF11},              // D11
    {GPIOF, GPIOF_PF12},              // D12
    {GPIOF, GPIOF_PF13},              // D13
    {GPIOF, GPIOF_PF14},              // D14
    {GPIOF, GPIOF_PF15},              // D15
    {GPIOH, GPIOH_PH2},               // D16
    {GPIOH, GPIOH_PH3},               // D17
    {GPIOH, GPIOH_PH5},               // D18
    {GPIOH, GPIOH_PH6},               // D19
    {GPIOH, GPIOH_PH9},               // D20
    {GPIOH, GPIOH_PH14},              // D21
    {GPIOB, GPIOB_PB8_TIM4_CH3},      // D22
    {GPIOB, GPIOB_PB9_TIM4_CH4},      // D23
    {GPIOE, GPIOE_PE5_TIM9_CH1},      // D24
    {GPIOE, GPIOE_PE6_TIM9_CH2},      // D25
    {GPIOE, GPIOE_PE9_TIM1_CH1},      // D26
    {GPIOE, GPIOE_PE13_TIM1_CH3},     // D27
    {GPIOH, GPIOH_PH10_TIM5_CH1},     // D28
    {GPIOH, GPIOH_PH11_TIM5_CH2},     // D29
    {GPIOH, GPIOH_PH12_TIM5_CH3},     // D30
    {GPIOA, GPIOA_PA15_TIM2_CH1_ETR}, // D31

    // SERIAL
    {GPIOI, GPIOI_PI1_SPI2_SCK},      // D32
    {GPIOI, GPIOI_PI0_SPI2_NSS},      // D33
    {GPIOI, GPIOI_PI2_SPI2_MISO},     // D34
    {GPIOI, GPIOI_PI3_SPI2_MOSI},     // D35
    {GPIOH, GPIOH_PH15},              // D36
    {GPIOD, GPIOD_PD7},               // D37
    {GPIOG, GPIOG_PG13_SPI6_SCK},     // D38
    {GPIOG, GPIOG_PG8_SPI6_NSS},      // D39
    {GPIOG, GPIOG_PG12_SPI6_MISO},    // D40
    {GPIOG, GPIOG_PG14_SPI6_MOSI},    // D41
    {GPIOG, GPIOG_PG9},               // D42
    {GPIOG, GPIOG_PG11},              // D43
    {GPIOF, GPIOF_PF0_I2C2_SDA},      // D44
    {GPIOF, GPIOF_PF1_I2C2_SCL},      // D45
    {GPIOE, GPIOE_PE1},               // D46
    {GPIOG, GPIOG_PG15},              // D47
    {GPIOA, GPIOA_PA0_UART4_TX},      // D48
    {GPIOA, GPIOA_PA1_UART4_RX},      // D49
    {GPIOE, GPIOE_PE2},               // D50
    {GPIOE, GPIOE_PE3},               // D51
    {GPIOD, GPIOD_PD8_USART3_TX},     // D52
    {GPIOD, GPIOD_PD9_USART3_RX},     // D53
    {GPIOD, GPIOD_PD11_USART3_CTS},   // D54
    {GPIOD, GPIOD_PD12_USART3_RTS},   // D55
    {GPIOD, GPIOD_PD5_USART2_TX},     // D56
    {GPIOD, GPIOD_PD6_USART2_RX},     // D57
    {GPIOD, GPIOD_PD3_USART2_CTS},    // D58
    {GPIOD, GPIOD_PD4_USART2_RTS},    // D59
    {GPIOI, GPIOI_PI8},               // D60
    {GPIOI, GPIOI_PI11},              // D61

    // ANALOG
    {GPIOD, GPIOD_PD1},               // D62
    {GPIOG, GPIOG_PG10},              // D63
    {GPIOB, GPIOB_PB15},              // D64
    {GPIOI, GPIOI_PI4},               // D65
    {GPIOD, GPIOD_PD0},               // D66
    {GPIOC, GPIOC_PC13},              // D67
    {GPIOI, GPIOI_PI10},              // D68
    {GPIOB, GPIOB_PB14}               // D69
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
static bool fetch_gpio_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_wait_cmd(BaseSequentialStream * chp, char *cmd_list[], char * data_list[]);
static bool fetch_gpio_heartbeat_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char gpio_config_help_string[] = "Configure pin as GPIO\n" \
                                              "Usage: config(<port>,<pin>,<mode>)\n" \
                                              "\tmode = INPUT_FLOATING, INPUT_PULLUP, INPUT_PULLDOWN,\n" \
                                              "\t       OUTPUT_PUSHPULL, OUTPUT_OPENDRAIN";
static const char gpio_wait_help_string[]   = "Wait for the given event on a gpio pin\n" \
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
    { fetch_gpio_reset_cmd,       "reset",      "Reset all GPIO pins to defaults" },
    { NULL, NULL, NULL } // null terminate list
  };

static const char * pin_state_tok[]   = {"true","false","1","0","high","low","set","clear"};
static const char * pin_mode_tok[]    = {"INPUT_FLOATING","INPUT_PULLUP","INPUT_PULLDOWN",
                                         "OUTPUT_PUSHPULL","OUTPUT_OPENDRAIN", "ALTERNATE",
                                         "ALT_0", "ALT_1", "ALT_2", "ALT_3",
                                         "ALT_4", "ALT_5", "ALT_6", "ALT_7",
                                         "ALT_8", "ALT_9", "ALT_10", "ALT_11",
                                         "ALT_12", "ALT_13", "ALT_14", "ALT_15"};
static const char * wait_event_tok[]  = {"HIGH","1","LOW","0","RISING","FALLING"};

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
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  util_message_info(chp, "Fetch GPIO Help:");
  fetch_display_help(chp, fetch_gpio_commands);
	return true;
}

static bool fetch_gpio_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 2, 2);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  bool pin_state;

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
  FETCH_PARAM_CHECK(chp, cmd_list, 1, 1);

  ioportid_t port = string_to_port(data_list[0]);
  uint16_t port_state;

  if( port == NULL )
  {
    util_message_error(chp, "invalid port");
    return false;
  }

  port_state = palReadPort(port);
  util_message_uint16(chp, "state", port_state);
  return true;
}

static bool fetch_gpio_read_all_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  uint16_t port_state;

  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  port_state = palReadPort(GPIOA);
  util_message_uint16(chp, "a", port_state);
  port_state = palReadPort(GPIOB);
  util_message_uint16(chp, "b", port_state);
  port_state = palReadPort(GPIOC);
  util_message_uint16(chp, "c", port_state);
  port_state = palReadPort(GPIOD);
  util_message_uint16(chp, "d", port_state);
  port_state = palReadPort(GPIOE);
  util_message_uint16(chp, "e", port_state);
  port_state = palReadPort(GPIOF);
  util_message_uint16(chp, "f", port_state);
  port_state = palReadPort(GPIOG);
  util_message_uint16(chp, "g", port_state);
  port_state = palReadPort(GPIOH);
  util_message_uint16(chp, "h", port_state);
  port_state = palReadPort(GPIOI);
  util_message_uint16(chp, "i", port_state);

  return true;
}

static bool fetch_gpio_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

  FETCH_PARAM_CHECK(chp, cmd_list, 3, 3);

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
  FETCH_PARAM_CHECK(chp, cmd_list, 2, 2);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

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
  FETCH_PARAM_CHECK(chp, cmd_list, 2, 2);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);

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
  FETCH_PARAM_CHECK(chp, cmd_list, 4, 4);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  char * endptr;
  int32_t timeout;
  wait_event_t event;
  systime_t start_time;
  bool state_next, state_prev;

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
  FETCH_PARAM_CHECK(chp, cmd_list, 3, 3);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  iomode_t mode = PAL_STM32_PUPDR_FLOATING | PAL_STM32_MODE_INPUT;


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

  uint32_t mode_sel = token_match( data_list[2], FETCH_MAX_DATA_STRLEN, pin_mode_tok, NELEMS(pin_mode_tok));
  switch( mode_sel )
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
    case 5: // alternate
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUPDR_FLOATING; // FIXME TODO
      break;
    default:
      if( mode_sel > 5 && mode_sel < (5+15))
      {
        util_message_info(chp, "alt=%U", mode_sel-6);
        mode = PAL_MODE_ALTERNATE(mode_sel-6);
      }
      else
      {
        util_message_error(chp, "invalid pin mode");
        return false;
      }
  }

  util_message_info(chp, "mode=%x (%U)", mode, mode);
  palSetPadMode(port, pin, mode);

  return true;
}

static bool fetch_gpio_info_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 2, 2);

  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  uint32_t alt_func;


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
      util_message_uint32(chp, "alt_func", alt_func);
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

static bool fetch_gpio_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

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
  return fetch_dispatch(chp, fetch_gpio_commands, cmd_list, data_list);
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

