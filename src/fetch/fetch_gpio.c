/*! \file fetch_gpio.c
 * Marionette fetch_gpio routines
 * @defgroup fetch_gpio Fetch GPIO
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>

#include "chprintf.h"
#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"

#include "io_manage.h"

#include "fetch_defs.h"
#include "fetch_gpio.h"
#include "fetch.h"

// list all command function prototypes here 
static bool fetch_gpio_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_read_port_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_set_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_clear_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_info_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_gpio_force_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static const char gpio_config_help_string[] ="Configure pin as GPIO\n" \
                        "Usage: config(<port>,<pin>,<mode>)\n" \
                        "\tmode = INPUT_FLOATING, INPUT_PULLUP, INPUT_PULLDOWN,\n" \
                        "\t       OUTPUT_PUSHPULL, OUTPUT_OPENDRAIN";

static fetch_command_t fetch_gpio_commands[] = {
    { fetch_gpio_help_cmd,        "help",       "Display GPIO help"},
    { fetch_gpio_read_cmd,        "read",       "Read pin state\nUsage: read(<port>,<pin>)" },
    { fetch_gpio_read_port_cmd,   "readport",   "Read state of all pins on port\nUsage: readport(<port>)" },
    { fetch_gpio_write_cmd,       "write",      "Write state to pin\nUsage: write(<port>,<pin>,<state>)" },
    { fetch_gpio_set_cmd,         "set",        "Set pin to 1\nUsage: set(<port>,<pin>)" },
    { fetch_gpio_clear_cmd,       "clear",      "Clear pin to 0\nUsage: clear(<port>,<pin>)" },
    { fetch_gpio_config_cmd,      "config",     gpio_config_help_string },
    { fetch_gpio_info_cmd,        "info",       "Get pin info\nUsage: info(<port>,<pin>)" },
    { fetch_gpio_reset_cmd,       "reset",      "Reset GPIO pin to defaults\nUsage: reset(<port>,<pin>)" },
    { fetch_gpio_force_reset_cmd, "forcereset", NULL },
    { NULL, NULL, NULL } // null terminate list
  };

static const char * pin_state_tok[] = {"true","false","1","0"};
static const char * pin_mode_tok[] = {"INPUT_FLOATING","INPUT_PULLUP","INPUT_PULLDOWN",
                                      "OUTPUT_PUSHPULL","OUTPUT_OPENDRAIN"};

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
      palSetPad(port,pin);
      break;
    case 1: // false
    case 3: // 0
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

static bool fetch_gpio_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  iomode_t mode = PAL_STM32_PUDR_FLOATING | PAL_STM32_MODE_INPUT;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 3) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  switch( token_match( data_list[2], FETCH_MAX_DATA_STRLEN, pin_mode_tok, NELEMS(pin_mode_tok)) )
  {
    case 0: // input floating
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING;
      break;
    case 1: // input pullup
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_PULLUP;
      break;
    case 2: // input pulldown
      mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_PULLDOWN;
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

  if( !io_manage_set_mode( port, pin, mode, IO_GPIO) )
  {
    util_message_error(chp, "unable to allocate pin as GPIO");
    return false;
  }

  return true;
}

static bool fetch_gpio_info_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  io_alloc_t current_alloc;
  char * alloc_name;
  iomode_t current_mode;
  uint32_t available_alloc;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  current_alloc = io_manage_get_current_alloc(port, pin);
  current_mode = io_manage_get_current_mode(port, pin);
  available_alloc = io_manage_get_available_alloc(port, pin);

  alloc_name = (char*)io_manage_get_alloc_name(current_alloc);
  util_message_string(chp, "current", alloc_name);

  for( int i = 0; i < 32; i++ )
  {
    if( available_alloc & (1<<i) )
    {
      alloc_name = (char*)io_manage_get_alloc_name(available_alloc & (1<<i));
      util_message_string(chp, "available", alloc_name);
    }
  }

  switch( current_mode & PAL_STM32_MODE_MASK )
  {
    case PAL_STM32_MODE_INPUT:
      switch( current_mode & PAL_STM32_PUDR_MASK )
      {
        case PAL_STM32_PUDR_FLOATING:
          util_message_string(chp, "mode", "INPUT_FLOATING");
          break;
        case PAL_STM32_PUDR_PULLUP:
          util_message_string(chp, "mode", "INPUT_PULLUP");
          break;
        case PAL_STM32_PUDR_PULLDOWN:
          util_message_string(chp, "mode", "INPUT_PULLDOWN");
          break;
      }
      break;
    case PAL_STM32_MODE_OUTPUT:
      switch( current_mode & PAL_STM32_OTYPE_MASK )
      {
        case PAL_STM32_OTYPE_PUSHPULL:
          util_message_string(chp, "mode", "OUTPUT_PUSHPULL");
          break;
        case PAL_STM32_OTYPE_OPENDRAIN:
          util_message_string(chp, "mode", "OUTPUT_OPENDRAIN");
          break;
      }
      break;
    case PAL_STM32_MODE_ALTERNATE:
      util_message_string(chp, "mode", "ALTERNATE");
      break;
    case PAL_STM32_MODE_ANALOG:
      util_message_string(chp, "mode", "ANALOG");
      break;
  }

  return true;
}

static bool fetch_gpio_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  io_alloc_t current_alloc;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  current_alloc = io_manage_get_current_alloc(port, pin);

  if( current_alloc != IO_GPIO )
  {
    util_message_error(chp, "pin is not allocated as a GPIO");
    return false;
  }

  io_manage_set_default_mode(port, pin);

  return true;
}

static bool fetch_gpio_force_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  ioportid_t port = string_to_port(data_list[0]);
  uint32_t pin = string_to_pin(data_list[1]);
  io_alloc_t current_alloc;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

  if( port == NULL || pin == INVALID_PIN )
  {
    util_message_error(chp, "invalid port/pin");
    return false;
  }

  io_manage_set_default_mode(port, pin);

  return true;
}


/*! \brief dispatch a specific gpio command
 */
bool fetch_gpio_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_gpio_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}

/*! @} */

