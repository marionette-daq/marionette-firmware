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

#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_serial.h"
#include "fetch_parser.h"

#define SERIAL_DRIVER_COUNT 3

SerialConfig serial_configs[SERIAL_DRIVER_COUNT];
event_listener_t serial_events[SERIAL_DRIVER_COUNT];
SerialDriver * serial_drivers[SERIAL_DRIVER_COUNT] = { &SD4, &SD3, &SD2 };

uint32_t tx_timeout_ms = 100;
uint32_t rx_timeout_ms = 100;

static SerialDriver * parse_serial_dev( char * str, uint32_t * dev )
{
  uint32_t dev_id = str[0] - '0';

  if( str[1] != '\0' )
  {
    return NULL;
  }

  if( str[0] < '0' || str[0] >= ('0' + SERIAL_DRIVER_COUNT) )
  {
    return NULL;
  }

  if( dev )
  {
    *dev = dev_id;
  }

  return serial_drivers[dev_id];
}

bool fetch_serial_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 3);
  FETCH_MIN_ARGS(chp, argc, 3);

  uint32_t dev;
  uint32_t speed;
  bool hwfc;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );
  
  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }

  if( !util_parse_uint32(argv[1], &speed) )
  {
    util_message_error(chp, "Invalid rate");
    return false;
  }

  if( !util_parse_bool(argv[2], &hwfc) )
  {
    util_message_error(chp, "Invalid hwfc setting");
    return false;
  }

  // FIXME implement turning on cts/rts flow control
  if( hwfc )
  {
    util_message_info(chp, "flow control not implemented");
  }

  serial_configs[dev].speed = speed;
  sdStart(serial_drv, &serial_configs[dev]);

  return true;
}

bool fetch_serial_timeout_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MOD_ARGS(chp, argc, 2);

  if( argc > 0 )
  {
    if( !util_parse_uint32(argv[0], &tx_timeout_ms) )
    {
      util_message_error(chp, "Invalid tx timeout value");
      return false;
    }
    if( !util_parse_uint32(argv[1], &rx_timeout_ms) )
    {
      util_message_error(chp, "Invalid rx timeout value");
      return false;
    }
  }

  util_message_uint32(chp, "tx_timeout_ms", tx_timeout_ms);
  util_message_uint32(chp, "rx_timeout_ms", rx_timeout_ms);

  return true;
}

bool fetch_serial_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MIN_ARGS(chp, argc, 2);

  uint32_t dev;
  uint8_t byte;
  uint32_t out_len = 0;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );
  
  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }
  if( serial_drv->state != SD_READY )
  {
    util_message_error(chp, "Serial device not ready");
    return false;
  }

  if( !fetch_parse_bytes(chp, argc-1, &argv[1], fetch_shared_buffer, sizeof(fetch_shared_buffer), &out_len) )
  {
    util_message_error(chp, "fetch_parse_bytes failed");
    return false;
  }

  for( uint32_t i = 0; i < out_len; i++ )
  {
    if( sdPutTimeout(serial_drv, fetch_shared_buffer[i], MS2ST(tx_timeout_ms)) == MSG_TIMEOUT )
    {
      util_message_error(chp, "timeout");
      return false;
    }
  }
  
  return true;
}

bool fetch_serial_read_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 2);

  uint32_t dev;
  uint8_t byte;
  uint32_t max_count;
  uint32_t rx_count;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );
  
  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }
  if( serial_drv->state != SD_READY )
  {
    util_message_error(chp, "Serial device not ready");
    return false;
  }
  
  if( !util_parse_uint32(argv[1], &max_count) || max_count == 0 || max_count >= FETCH_SHARED_BUFFER_SIZE)
  {
    util_message_error(chp, "Invalid byte count, min=1, max=%d", FETCH_SHARED_BUFFER_SIZE);
    return false;
  }

  for( rx_count=0; rx_count < max_count; rx_count++ )
  {
    if( sdReadTimeout(serial_drv, &byte, 1, MS2ST(rx_timeout_ms)) != 1 )
    {
      break;
    }
    fetch_shared_buffer[rx_count] = byte;
  }

  eventflags_t flags = chEvtGetAndClearFlags(&serial_events[dev]);

  util_message_bool(chp, "parity_error", flags & SD_PARITY_ERROR);
  util_message_bool(chp, "framing_error", flags & SD_FRAMING_ERROR);
  util_message_bool(chp, "overrun_error", flags & SD_OVERRUN_ERROR);
  util_message_bool(chp, "noise_error", flags & SD_NOISE_ERROR);
  util_message_bool(chp, "break_detected", flags & SD_BREAK_DETECTED);
  util_message_uint32(chp, "count", rx_count);
  util_message_string_escape(chp,"str",(char*)fetch_shared_buffer,rx_count);
  util_message_hex_uint8_array(chp,"hex",fetch_shared_buffer,rx_count);

  return true;
}

bool fetch_serial_break_cmd(BaseSequentialStream *chp, uint32_t argc, char *argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 2);
  
  util_message_error(chp, "not implemented"); // FIXME

  return true;
}

bool fetch_serial_status_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);
  
  uint32_t dev;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );

  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }

  util_message_bool(chp, "ready", serial_drv->state == SD_READY);

  eventflags_t flags = chEvtGetAndClearFlags(&serial_events[dev]);

  util_message_bool(chp, "chn_no_error", flags & CHN_NO_ERROR);
  util_message_bool(chp, "chn_connected", flags & CHN_CONNECTED);
  util_message_bool(chp, "chn_disconnected", flags & CHN_DISCONNECTED);
  util_message_bool(chp, "chn_input_available", flags & CHN_INPUT_AVAILABLE);
  util_message_bool(chp, "chn_output_empty", flags & CHN_OUTPUT_EMPTY);
  util_message_bool(chp, "chn_transmission_end", flags & CHN_TRANSMISSION_END);

  util_message_bool(chp, "sd_parity_error", flags & SD_PARITY_ERROR);
  util_message_bool(chp, "sd_framing_error", flags & SD_FRAMING_ERROR);
  util_message_bool(chp, "sd_overrun_error", flags & SD_OVERRUN_ERROR);
  util_message_bool(chp, "sd_noise_error", flags & SD_NOISE_ERROR);
  util_message_bool(chp, "sd_break_detected", flags & SD_BREAK_DETECTED);

  return true;
}

bool fetch_serial_flush_input_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);

  uint32_t dev;
  uint8_t byte;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );
  
  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }
  if( serial_drv->state != SD_READY )
  {
    util_message_error(chp, "Serial device not ready");
    return false;
  }
  
  chSysLock();
  iqResetI(&(serial_drv)->iqueue);
  chSysUnlock();

  return true;
}


bool fetch_serial_read_line_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 2);
  FETCH_MIN_ARGS(chp, argc, 1);

  uint32_t dev;
  uint8_t byte;
  uint32_t max_count;
  uint32_t rx_count;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );
  
  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }
  if( serial_drv->state != SD_READY )
  {
    util_message_error(chp, "Serial device not ready");
    return false;
  }
  
  if( argc == 1 )
  {
    max_count = FETCH_SHARED_BUFFER_SIZE;
  }
  else if( !util_parse_uint32(argv[1], &max_count) || max_count == 0 || max_count >= FETCH_SHARED_BUFFER_SIZE)
  {
    util_message_error(chp, "Invalid byte count, min=1, max=%d", FETCH_SHARED_BUFFER_SIZE);
    return false;
  }

  for( rx_count=0; rx_count < max_count; rx_count++ )
  {
    if( sdReadTimeout(serial_drv, &byte, 1, MS2ST(rx_timeout_ms)) != 1 )
    {
      break;
    }

    fetch_shared_buffer[rx_count] = byte;

    if( byte == '\r' || byte == '\n' )
    {
      break; // eol
    }
  }

  eventflags_t flags = chEvtGetAndClearFlags(&serial_events[dev]);

  util_message_bool(chp, "parity_error", flags & SD_PARITY_ERROR);
  util_message_bool(chp, "framing_error", flags & SD_FRAMING_ERROR);
  util_message_bool(chp, "overrun_error", flags & SD_OVERRUN_ERROR);
  util_message_bool(chp, "noise_error", flags & SD_NOISE_ERROR);
  util_message_bool(chp, "break_detected", flags & SD_BREAK_DETECTED);
  util_message_uint32(chp, "count", rx_count);
  util_message_string_escape(chp,"str",(char*)fetch_shared_buffer,rx_count);
  util_message_hex_uint8_array(chp,"hex",fetch_shared_buffer,rx_count);

  return true;
}

bool fetch_serial_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 1);
  FETCH_MIN_ARGS(chp, argc, 1);
  
  uint32_t dev;
  SerialDriver * serial_drv = parse_serial_dev( argv[0], &dev );

  if( serial_drv == NULL )
  {
    util_message_error(chp, "Invalid serial device");
    return false;
  }

  sdStop(serial_drv);

  return true;
}

bool fetch_serial_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[])
{
  FETCH_MAX_ARGS(chp, argc, 0);

  FETCH_HELP_BREAK(chp);
  FETCH_HELP_LEGEND(chp);
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_TITLE(chp,"Serial Help");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "config(<dev>,<rate>,<hwfc>)");
  FETCH_HELP_DES(chp, "Configure serial uart");
  FETCH_HELP_ARG(chp, "dev","Serial device number");
  FETCH_HELP_ARG(chp, "rate","Baudrate");
  FETCH_HELP_ARG(chp, "hwfc","hardware flow control, 0 | 1");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "timeout([<tx_ms>,<rx_ms>])");
  FETCH_HELP_DES(chp, "Get or set timeout for read/write");
  FETCH_HELP_ARG(chp, "tx_ms", "transmit timeout in milli-seconds");
  FETCH_HELP_ARG(chp, "rx_ms", "receive timeout in milli-seconds");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "write(<dev>,<data 0>[,<data_1 ...])");
  FETCH_HELP_DES(chp, "Write data to serial port");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_ARG(chp, "data", "List of bytes or strings");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "read(<dev>,<count>)");
  FETCH_HELP_DES(chp, "Read data from serial port");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_ARG(chp, "count", "Max number of bytes to read");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "break(<dev>,<ms>)");
  FETCH_HELP_DES(chp, "Signal break on tx line");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_ARG(chp, "ms", "time to assert break in milli-seconds");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "status(<dev>)");
  FETCH_HELP_DES(chp, "Query serial device status flags");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "reset(<dev>)");
  FETCH_HELP_DES(chp, "Reset serial device");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "flush_input(<dev>)");
  FETCH_HELP_DES(chp, "Flush input buffer");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_BREAK(chp);
  FETCH_HELP_CMD(chp, "read_line(<dev>[,<max>])");
  FETCH_HELP_DES(chp, "Read until CR or NL");
  FETCH_HELP_ARG(chp, "dev", "Serial device number");
  FETCH_HELP_ARG(chp, "count", "Optional maximum bytes to read");
  FETCH_HELP_BREAK(chp);

	return true;
}

void fetch_serial_init(void)
{
  for( uint32_t i = 0; i < SERIAL_DRIVER_COUNT; i++ )
  {
    chEvtRegister(chnGetEventSource(serial_drivers[i]), &serial_events[i], i);
  }
}

bool fetch_serial_reset(BaseSequentialStream * chp)
{
  for( uint32_t i = 0; i < SERIAL_DRIVER_COUNT; i++ )
  {
    sdStop(serial_drivers[i]);
  }
  return true;
}

/*! @} */

