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

#include "io_manage.h"
#include "io_manage_defs.h"
#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_pwm.h"

static pwmcnt_t pwm_period1 = NULL;
static pwmcnt_t pwm_period2 = NULL;
static pwmmode_t pwm_out = NULL;

static bool pwm_init_flag = true;
static int32_t pwm_duty_cycle;
static PWMConfig pwmcfg1 = {
	0,
	0,
	NULL,
	{
	 {PWM_OUTPUT_DISABLED, NULL},
	 {PWM_OUTPUT_DISABLED, NULL},
	 {PWM_OUTPUT_ACTIVE_HIGH, NULL},
	 {PWM_OUTPUT_DISABLED, NULL}
	},
	0,
	0,
	0
};
static PWMConfig pwmcfg2 = {
	0,
	0,
	NULL,
	{
	 {PWM_OUTPUT_ACTIVE_HIGH, NULL},
	 {PWM_OUTPUT_DISABLED, NULL},
	 {PWM_OUTPUT_DISABLED, NULL},
	 {PWM_OUTPUT_DISABLED, NULL}
	},
	0,
	0,
	0
};

// list all command function prototypes here 
static bool fetch_pwm_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_pwm_start_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_pwm_stop_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_pwm_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_pwm_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);


static fetch_command_t fetch_pwm_commands[] = {
    { fetch_pwm_start_cmd,  "start",  "Start Pulse Width Modulation\n" \
                                            "Usage: PWM(<dev>,<duty_cycle>)\n" \
                                            "\tDuty Cycle = 0 - 100%\n" },

    { fetch_pwm_config_cmd,    "config",    "Configure Pulse Width Modulation\n" \
                                            "Usage: config(<dev>,<freq>)\n" \
                                            "\tfreq = short freq list for demo"},
    { fetch_pwm_stop_cmd,     "stop",     "Stop PWM driver\n" \
                                            "Usage: Stop(<dev>)" },
    { fetch_pwm_reset_cmd,     "reset",     "Reset PWM driver\n" \
                                            "Usage: reset(<dev>)" },
    { fetch_pwm_help_cmd,      "help",      "PWM command help" },
    { NULL, NULL, NULL } // null terminate list
  };

static bool fetch_pwm_config_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }

int32_t  pwm_dev = strtol(data_list[0],&endptr,0);

 if( *endptr != '\0' || pwm_dev > 2 || pwm_dev < 1 ) {
    util_message_error(chp, "invalid PWM device");
    return false;
  } 

 int32_t pwm_freq = strtol(data_list[1],&endptr,0);

 if( *endptr != '\0') {
    util_message_error(chp, "invalid frequency value");
    return false;
  } 
  else if(pwm_dev == 1)  {
   switch(pwm_freq)
   {
	   case 1:
		   pwmcfg1.frequency = 100000;
		   pwmcfg1.frequency = 100000;
		   break;
	   case 2:
		   pwmcfg1.frequency = 20000;
		   pwmcfg1.period = 10000;
		   break;
	   case 3:
		   pwmcfg1.frequency = 60000;
		   pwmcfg1.period = 20000;
		   break;
	   case 4:
		   pwmcfg1.frequency = 32000;
		   pwmcfg1.period = 8000;
		   break;
	   case 5:
		   pwmcfg1.frequency = 200000;
		   pwmcfg1.period = 40000;
		   break;
	   case 10:
		   pwmcfg1.frequency = 100000;
		   pwmcfg1.frequency = 10000;
		   break;
	   case 100:
		   pwmcfg1.frequency = 100000;
		   pwmcfg1.period = 1000;
		   break;
	   case 1000:
		   pwmcfg1.frequency = 100000;
		   pwmcfg1.period = 100;
		   break;
	   case 10000:
		   pwmcfg1.frequency = 168000000;
		   pwmcfg1.period = 16800;
		   break;
	   case 100000:
		   pwmcfg1.frequency = 168000000;
		   pwmcfg1.period = 1680;
		   break;
	   case 1000000:
		   pwmcfg1.frequency = 168000000;
		   pwmcfg1.period = 168;
		   break;
	   	  default:
           	  util_message_error(chp, "invalid frequency value");
	          return false;
	
   }
  }
  else  {

   switch(pwm_freq)
   {
	   case 1:
		   pwmcfg2.frequency = 10000;
		   pwmcfg2.period = 10000;
		   palSetPadMode(GPIOG, GPIOG_LED9, PAL_MODE_OUTPUT_PUSHPULL);
		   palTogglePad(GPIOG, GPIOG_LED9);
		   break;
	   case 2:
		   pwmcfg2.frequency = 20000;
		   pwmcfg2.period = 10000;
		   break;
	   case 3:
		   pwmcfg2.frequency = 60000;
		   pwmcfg2.period = 20000;
		   break;
	   case 4:
		   pwmcfg2.frequency = 32000;
		   pwmcfg2.period = 8000;
		   break;
	   case 5:
		   pwmcfg2.frequency = 200000;
		   pwmcfg2.period = 40000;
		   break;
	   case 10:
		   pwmcfg2.frequency = 100000;
		   pwmcfg2.period = 10000;
		   break;
	   case 100:
		   pwmcfg2.frequency = 100000;
		   pwmcfg2.period = 1000;
		   break;
	   case 1000:
		   pwmcfg2.frequency = 100000;
		   pwmcfg2.period = 100;
		   break;
	   case 10000:
		   pwmcfg2.frequency = 84000000;
		   pwmcfg2.period = 8400;
		   break;
	   case 100000:
		   pwmcfg2.frequency = 84000000;
		   pwmcfg2.period = 840;
		   break;
	   case 1000000:
		   pwmcfg2.frequency = 84000000;
		   pwmcfg2.period = 84;
		   break;
		   default:
        	   util_message_error(chp, "invalid frequency value");
	 	   return false;
	
   }
  }

  

 
    switch(pwm_dev)
    {
	    case 1:
		    palSetPadMode(GPIOE, GPIOE_PIN13, PAL_MODE_ALTERNATE(1));
		    pwmStart(&PWMD1, &pwmcfg1);  
		    
		    break;
	    case 2:
		    palSetPadMode(GPIOE, GPIOE_PIN5, PAL_MODE_ALTERNATE(3));
		    pwmStart(&PWMD9, &pwmcfg2);   
		    break;
	    default:
		    break;
    }	    
return true;  

}

static bool fetch_pwm_start_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;
  int32_t pwm_dev;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 2) )
  {
    return false;
  }
int32_t pwm_duty = strtol(data_list[1],&endptr,0);
  
  if( *endptr != '\0' || pwm_duty > 99 || pwm_duty < 1 ) {
    util_message_error(chp, "invalid duty cycle");
    return false;
  } 
  else {
  pwm_duty_cycle = pwm_duty * 100;
  }

  pwm_dev = strtol(data_list[0], &endptr, 0);

  if( *endptr != '\0' || pwm_dev < 1 || pwm_dev > 2 )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }
  else
  {
	  switch(pwm_dev)
	  {
		  case 1:
	  	    pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, pwm_duty_cycle));
		    break;
		  case 2:
		    pwmEnableChannel(&PWMD9, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD9, pwm_duty_cycle));
		    break;
  	  }
  }
  return true;
}
static bool fetch_pwm_stop_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  char * endptr;
  int32_t pwm_dev;

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }

  pwm_dev = strtol(data_list[0], &endptr, 0);

  if( *endptr != '\0' || pwm_dev < 1 || pwm_dev > 2 )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }
  else
  {
	  switch (pwm_dev)
	  {
		  case 1:
	       	  pwmDisableChannel(&PWMD1, 2);
		  break;
		  case 2:
		  pwmDisableChannel(&PWMD9, 0);
		  break;
  	  }
  }
  return true;


}
static bool fetch_pwm_reset_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  int32_t pwm_dev;
  char * endptr;
  ioportid_t port = string_to_port("GPIOE");

  if( !fetch_input_check(chp, cmd_list, FETCH_TOK_SUBCMD_0, data_list, 1) )
  {
    return false;
  }
  pwm_dev = strtol(data_list[0], &endptr, 0);

  if( *endptr != '\0' || pwm_dev < 1 || pwm_dev > 2 )
  {
    util_message_error(chp, "invalid device identifier");
    return false;
  }
  
  else
  {
    switch(pwm_dev)
    {
      case 1:
       pwmDisableChannel(&PWMD1, pwm_dev);
       io_manage_set_default_mode(port, 13, IO_GPIO);
       pwmStop(&PWMD1);
       break;
      case 2:
       pwmDisableChannel(&PWMD9, pwm_dev);
       io_manage_set_default_mode(port, 5, IO_GPIO);
       pwmStop(&PWMD9);
       break;
      default:
      break;
    }
 
     
   return true;
  }
}
static bool fetch_pwm_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  util_message_info(chp, "Usage legend: <> required, [] optional, | or,");
  util_message_info(chp, "              ... continuation, {} comment");
  
  util_message_info(chp, "dev = 1, 2");
  util_message_info(chp, "base = (reference strtol c function)");

  fetch_display_help(chp, fetch_pwm_commands);
	return true;
}

void fetch_pwm_init(BaseSequentialStream * chp)
{
  // TODO do we need anything here?
  
  pwm_init_flag = false;
}

/*! \brief dispatch a specific spi command
 */
bool fetch_pwm_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if( pwm_init_flag )
  {
    fetch_pwm_init(chp);
  }
  return fetch_dispatch(chp, fetch_pwm_commands, cmd_list[FETCH_TOK_SUBCMD_0], cmd_list, data_list);
}


bool fetch_pwm_reset(BaseSequentialStream * chp)
{
  return true;
}

/*! @} */

