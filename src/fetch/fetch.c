/*! \file fetch.c
 *  \subpage BNF 
 *  Fetch: A DSL for Marionette interaction.
 * @defgroup fetch Fetch DSL
 * @{
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "util_io.h"
#include "util_arg_parse.h"

#include "fetch_defs.h"
#include "fetch_commands.h"
#include "fetch_parser.h"
#include "fetch.h"

// shared buffer for use in IO buffering and parsing strings
uint8_t fetch_shared_buffer[FETCH_SHARED_BUFFER_SIZE];

bool fetch_parse_bytes( BaseSequentialStream * chp, uint32_t argc, char * argv[], uint8_t * output_str, uint32_t max_output_len, uint32_t * count )
{
  uint8_t byte;
  uint32_t parser_count;

  chDbgAssert(output_str != NULL, "fetch_parse_bytes(), #1, output_str is NULL");
  chDbgAssert(count != NULL, "fetch_parse_bytes(), #2, count is NULL");

  *count = 0;

  for( uint32_t i = 0; i < argc && argv[i] != NULL; i++ )
  {
    if( argv[i][0] == '\'' || argv[i][0] == '\"' )
    {
      parser_count = 0;
      // parse as quoted string
      if( !fetch_string_parser(argv[i], FETCH_MAX_DATA_STRLEN, (char*)&output_str[*count], max_output_len-(*count), &parser_count) )
      {
        util_message_error(chp, "error parsing string");
        util_message_error(chp, "error_msg: %s", fetch_parser_info.error_msg);
        return false;
      }
      *count += parser_count;
    }
    else if( argv[i][0] == 'h' || argv[i][0] == 'H' )
    {
      // parse as hex string
      if( !fetch_hex_string_parser(argv[i], FETCH_MAX_DATA_STRLEN, (char*)&output_str[*count], max_output_len-(*count), &parser_count) )
      {
        util_message_error(chp, "error parsing hex string");
        util_message_error(chp, "error_msg: %s", fetch_parser_info.error_msg);
        return false;
      }
      *count += parser_count;
    }
    else
    {
      // parse as numeric byte value
      if( !util_parse_uint8(argv[i], &byte) )
      {
        util_message_error(chp, "error parsing numeric byte");
        return false;
      }
      else if( *count < max_output_len )
      {
        output_str[*count] = byte;
        (*count)++;
      }
      else
      {
        util_message_error(chp, "max output");
        return false;
      }
    }
  }

  return true;
}


bool fetch_clocks_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);

  util_message_info(chp, "PLLM         = %4u", STM32_PLLM_VALUE);
  util_message_info(chp, "PLLN         = %4u", STM32_PLLN_VALUE);
  util_message_info(chp, "PLLP         = %4u", STM32_PLLP_VALUE);
  util_message_info(chp, "PLLQ         = %4u", STM32_PLLQ_VALUE);

  util_message_info(chp, "HSECLK       = %4u MHz", STM32_HSECLK/1000000);
  util_message_info(chp, "SYSCLK       = %4u MHz", STM32_SYSCLK/1000000);
  util_message_info(chp, "PLLVCO       = %4u MHz", STM32_PLLVCO/1000000);
  util_message_info(chp, "HCLK         = %4u MHz", STM32_HCLK/1000000);
  
  util_message_info(chp, "PCLK1        = %4u MHz", STM32_PCLK1/1000000);
  util_message_info(chp, "PCLK2        = %4u MHz", STM32_PCLK2/1000000);

  util_message_info(chp, "TIMCLK1      = %4u MHz", STM32_TIMCLK1/1000000);
  util_message_info(chp, "TIMCLK2      = %4u MHz", STM32_TIMCLK2/1000000);
  util_message_info(chp, "ADCCLK       = %4u MHz", STM32_ADCCLK/1000000);

#if STM32_GPT_USE_TIM1
  util_message_info(chp, "GPTD1.clock  = %4u Mhz", GPTD1.clock/1000000);
#endif
#if STM32_GPT_USE_TIM2
  util_message_info(chp, "GPTD2.clock  = %4u Mhz", GPTD2.clock/1000000);
#endif
#if STM32_GPT_USE_TIM3
  util_message_info(chp, "GPTD3.clock  = %4u Mhz", GPTD3.clock/1000000);
#endif
#if STM32_GPT_USE_TIM4
  util_message_info(chp, "GPTD4.clock  = %4u Mhz", GPTD4.clock/1000000);
#endif
#if STM32_GPT_USE_TIM5
  util_message_info(chp, "GPTD5.clock  = %4u Mhz", GPTD5.clock/1000000);
#endif
#if STM32_GPT_USE_TIM6
  util_message_info(chp, "GPTD6.clock  = %4u Mhz", GPTD6.clock/1000000);
#endif
#if STM32_GPT_USE_TIM7
  util_message_info(chp, "GPTD7.clock  = %4u Mhz", GPTD7.clock/1000000);
#endif
#if STM32_GPT_USE_TIM8
  util_message_info(chp, "GPTD8.clock  = %4u Mhz", GPTD8.clock/1000000);
#endif
#if STM32_GPT_USE_TIM9
  util_message_info(chp, "GPTD9.clock  = %4u Mhz", GPTD9.clock/1000000);
#endif
#if STM32_GPT_USE_TIM10
  util_message_info(chp, "GPTD10.clock = %4u Mhz", GPTD10.clock/1000000);
#endif
#if STM32_GPT_USE_TIM11
  util_message_info(chp, "GPTD11.clock = %4u Mhz", GPTD11.clock/1000000);
#endif
#if STM32_GPT_USE_TIM12
  util_message_info(chp, "GPTD12.clock = %4u Mhz", GPTD12.clock/1000000);
#endif
#if STM32_GPT_USE_TIM13
  util_message_info(chp, "GPTD13.clock = %4u Mhz", GPTD13.clock/1000000);
#endif
#if STM32_GPT_USE_TIM14
  util_message_info(chp, "GPTD14.clock = %4u Mhz", GPTD14.clock/1000000);
#endif

  return true;
}

bool fetch_test_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  util_message_info(chp, "argc %U", argc);

  for( uint32_t i = 0; i < argc; i++ )
  {
    util_message_info(chp, "argv[%d] = '%s'", i, argv[i]);
  }

	return true;
}

bool fetch_test_data_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  uint32_t count = 0;

  if( !fetch_parse_bytes(chp, argc, argv, fetch_shared_buffer, sizeof(fetch_shared_buffer), &count) )
  {
    util_message_error(chp, "fetch_parse_bytes failed");
    return false;
  }

  util_message_hex_uint8_array(chp, "data", fetch_shared_buffer, count);

  return true;
}

bool fetch_help_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);

  FETCH_HELP_LEGEND(chp);

  FETCH_HELP_CMD(chp, "+help");
  FETCH_HELP_DES(chp, "Display shell help");
  FETCH_HELP_CMD(chp, "gpio.help");
  FETCH_HELP_DES(chp, "Display gpio help");
  FETCH_HELP_CMD(chp, "adc.help");
  FETCH_HELP_DES(chp, "Display adc help");
  FETCH_HELP_CMD(chp, "dac.help");
  FETCH_HELP_DES(chp, "Display dac help");
  FETCH_HELP_CMD(chp, "spi.help");
  FETCH_HELP_DES(chp, "Display spi help");
  FETCH_HELP_CMD(chp, "i2c.help");
  FETCH_HELP_DES(chp, "Display i2c help");
  FETCH_HELP_CMD(chp, "mbus.help");
  FETCH_HELP_DES(chp, "Display mbus help");
  FETCH_HELP_CMD(chp, "clocks");
  FETCH_HELP_DES(chp, "Display info about internal clocks");
  FETCH_HELP_CMD(chp, "reset");
  FETCH_HELP_DES(chp, "Reset all peripheral modules");
  FETCH_HELP_CMD(chp, "chipid");
  FETCH_HELP_DES(chp, "Query unique cpu chip id");
  FETCH_HELP_CMD(chp, "version");
  FETCH_HELP_DES(chp, "Query firmware version information");

	return true;
}

bool fetch_version_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);

	util_message_string_format(chp, "firmware_version", GIT_COMMIT_VERSION);

	return true;
}

bool fetch_chip_id_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);
  
  uint32_t chip_id[3] = {0,0,0};

  chip_id[0] = *(uint32_t*)STM32F4_UNIQUE_ID_LOW;
  chip_id[1] = *(uint32_t*)STM32F4_UNIQUE_ID_CENTER;
  chip_id[2] = *(uint32_t*)STM32F4_UNIQUE_ID_HIGH;

	util_message_hex_uint32_array(chp, "chipid", chip_id, 3);
	return true;
}

bool fetch_reset_cmd( BaseSequentialStream * chp, uint32_t argc, char * argv[] )
{
  FETCH_MAX_ARGS(chp, argc, 0);

  // Add any new peripheral reset functions here
  fetch_adc_reset(chp);
  fetch_dac_reset(chp);
  fetch_spi_reset(chp);
  fetch_i2c_reset(chp);
  fetch_gpio_reset(chp);
  fetch_mbus_reset(chp);
  fetch_sd_reset(chp);
  fetch_timer_reset(chp);

  // make sure all pin assignments are set to defaults
  // ~not needed at the moment~
	//palInit(&pal_default_config);

	return true;
}

/*! \brief Fetch initialization 
 * 
 * This needs to be called before parsing fetch commands
 */
void fetch_init(void)
{
  fetch_gpio_init();
	fetch_adc_init();
  fetch_dac_init();
  fetch_spi_init();
  fetch_i2c_init();
  fetch_mbus_init();
  fetch_sd_init();
  fetch_timer_init();
  fetch_serial_init();
}

bool fetch_execute( BaseSequentialStream * chp, const char * input_line )
{
  //FIXME add mutual exclusion to this function

  // add one to guarentee space for null at end
	static char   output_buffer[ FETCH_MAX_LINE_CHARS + 1 ];
	static char * argv[ FETCH_MAX_DATA_TOKS + 1 ];
  fetch_func_t func = NULL;
  uint32_t argc = 0;

  if( fetch_command_parser(input_line, FETCH_MAX_LINE_CHARS, output_buffer, FETCH_MAX_LINE_CHARS, &func, &argc, argv, FETCH_MAX_DATA_TOKS) == false )
  {
    util_message_error(chp, "Error parsing fetch command");
    util_message_error(chp, "offset: %d", fetch_parser_info.offset);
    util_message_error(chp, "error_msg: %s", fetch_parser_info.error_msg);
    DEBUG_VMSG(chp, "fsm_state: %d", fetch_parser_info.fsm_state);
    DEBUG_VMSG(chp, "fsm_state_first_final: %d", fetch_parser_info.fsm_state_first_final);
    DEBUG_VMSG(chp, "fsm_state_error: %d", fetch_parser_info.fsm_state_error);
    DEBUG_VMSG(chp, "fsm_state_start: %d", fetch_parser_info.fsm_state_start);
    return false;
  }

  // null terminate the argv list so that we can iterate till NULL
  argv[argc] = NULL;

  if( func != NULL )
  {
    return func(chp, argc, argv);
  }
  else
  {
    util_message_error(chp, "null function pointer");
    return false;
  }
}


/*! @} */

