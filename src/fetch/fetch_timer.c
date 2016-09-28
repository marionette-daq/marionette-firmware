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
#include "fetch_timer.h"
#include "fetch.h"

// chibios header defining the stm32 timer peripheral registers
#include "stm32_tim.h"

typedef enum {
  TIM_MODE_PWM,
  TIM_MODE_CAPTURE,
  TIM_MODE_COUNTER
} timer_channel_mode_t;

/*
 * PB8  TIM4 CH3
 * PB9  TIM4 CH4
 * PE5  TIM9 CH1
 * PE6  TIM9 CH2
 * PE9  TIM1 CH1
 * PE13 TIM1 CH3
 * PH10 TIM5 CH1
 * PH11 TIM5 CH2
 * PH12 TIM5 CH3
 * PA15 TIM2 CH1 ETR
 *
 * TIM7 as freq time ref
 */

#define STM32_TIM1_CLK  STM32_TIMCLK2
#define STM32_TIM2_CLK  STM32_TIMCLK1
#define STM32_TIM4_CLK  STM32_TIMCLK1
#define STM32_TIM5_CLK  STM32_TIMCLK1
#define STM32_TIM7_CLK  STM32_TIMCLK1
#define STM32_TIM9_CLK  STM32_TIMCLK2

static void timer_init( void )
{
  
}

static bool timer_start( uint32_t timer )
{
  return false;
}

static bool timer_stop( uint32_t timer )
{
  return false;

}

static bool timer_clear( uint32_t timer)
{
  return false;
}

static bool timer_count( uint32_t timer )
{
  return false;
}

static void timer_period( uint32_t timer, uint32_t period)
{

}

static void timer_frequency( uint32_t timer, uint32_t frequency)
{

}

static bool timer_channel_mode( uint32_t timer, uint32_t channel, timer_channel_mode_t mode )
{
  return false;
}

static bool timer_channel_read( uint32_t timer, uint32_t channel, uint32_t *count )
{
  return false;
}

static bool timer_channel_write( uint32_t timer, uint32_t channel, uint32_t count )
{
  return false;
}




// list all command function prototypes here 
static bool fetch_timer_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_period_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_frequency_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_pwm_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_cap_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_clear_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_timer_count_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_timer_commands[] = {
    { fetch_timer_help_cmd,        "help",       "Display command info"},
    { fetch_timer_config_cmd,      "config",     "Configure timer pin mode"},
    { fetch_timer_period_cmd,      "period",     "Configure timer period"},
    { fetch_timer_frequency_cmd,   "frequency",  "Configure timer frequency"},
    { fetch_timer_pwm_cmd,         "pwm",        "Set pwm pin duty cycle"},
    { fetch_timer_cap_cmd,         "cap",        "Read last pin capture value"},
    { fetch_timer_clear_cmd,       "clear",      "Clear timer count"},
    { fetch_timer_count_cmd,       "count",      "Read timer count"},
    { NULL, NULL, NULL } // null terminate list
  };

static bool fetch_timer_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  fetch_display_help_legend(chp);
  fetch_display_help(chp, fetch_timer_commands);
	return true;
}

static bool fetch_timer_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id
  // timer channel
  // timer mode
  // pin mode (pull up/down, opendrain/pushpull)
  //
  // timer.config(0,0,pwm,float,pushpull)

	return true;
}

static bool fetch_timer_period_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id
  // period in us

	return true;
}

static bool fetch_timer_frequency_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id
  // frequency in Hz

	return true;
}

static bool fetch_timer_pwm_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id
  // timer channel
  // pwm value from 0-1000

	return true;
}

static bool fetch_timer_cap_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id
  // timer channel
  //

	return true;
}

static bool fetch_timer_clear_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id

	return true;
}

static bool fetch_timer_count_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // timer id

	return true;
}

void fetch_timer_init(BaseSequentialStream * chp)
{
  static bool timer_init_flag = false;

  if( timer_init_flag )
    return;

  // Init stuff goes here
  // ...
  // TODO
  // pwmStart(&PWMD1, ???); 
  
  // all channels disabled by default

  timer_init_flag = true;
}

/*! \brief dispatch a specific command
 */
bool fetch_timer_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_timer_commands, cmd_list, data_list);
}

bool fetch_timer_reset(BaseSequentialStream * chp)
{
  // TODO
  // disable all channels
  // 
  return true;
}

/*! @} */

