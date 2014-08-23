/*! \file fetch_dac.c
  *
  * Supporting Fetch DSL
  *
  * \sa fetch.c
  * @defgroup fetch_dac Fetch DAC
  * @{
   */

/*!
 * <hr>
 *
 *  In ChibiOS 2.6.x there is no DAC module in the HAL. It is available
 *  in the kernel_3_dev branch.
 *
 * <hr>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"
#include "mshell_state.h"

#include "io_manage.h"
#include "fetch_gpio.h"

#include "fetch_defs.h"
#include "fetch.h"

#include "fetch_dac.h"

/* Reference STF4 Reference
 *   Once the DAC channelx is enabled, the corresponding GPIO pin (PA4 or PA5) is
 *   automatically connected to the analog converter output (DAC_OUTx). In order to avoid
 *   parasitic consumption, the PA4 or PA5 pin should first be configured to analog (AIN).
 */


/*! \brief Channel output pins */
#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static DAC_output DAC_CH1_PIN   = { GPIOA, GPIOA_PIN4 };
static DAC_output DAC_CH2_PIN   = { GPIOA, GPIOA_PIN5 };

static DAC_output * DAC_outputs[] = {&DAC_CH1_PIN, &DAC_CH2_PIN};
#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for DAC"
#else
#error "Board not defined for DAC"
#endif


/*! \brief track the state of the dac
 */
static FETCH_dac_state fetch_dac_state =
{
	.dc_mv_ch1            = 0,
	.dc_mv_ch2            = 0,
	.init                 = false,
	.chp                  = NULL
};

/*! Set a channel to Analog Mode
 */
static bool fetch_dac_io_to_analog(DACChannel channel)
{
	if(DAC_outputs[channel] != NULL)
	{
		return(io_manage_set_mode(DAC_outputs[channel]->port, DAC_outputs[channel]->pad, PAL_MODE_INPUT_ANALOG, IO_DAC));
	}
	return false;
}

static void fetch_dac_io_set_defaults(void)
{
	for(uint8_t i = 0; i < NELEMS(DAC_outputs); ++i)
	{
		if(DAC_outputs[i] != NULL)
		{
			io_manage_set_default_mode(DAC_outputs[i]->port, DAC_outputs[i]->pad);
		}
	}
}


/*! \brief release the allocated outputs to IO_NONE 
 *   Stop the DAC and turn off the APBx clock.
 */
fetch_dac_deallocate(DACChannel ch) {
		// release io
		// Turn off DAC
		// Default state and init false
		// turn off DAC clock

}


/*! \brief Reset adc1
 */
static void fetch_adc1_reset(void)
{
	fetch_dac_stop(&DACD1);

	fetch_dac_start(&DACD1);

    fetch_dac_state.dc_mv_ch1 = 0;
    fetch_dac_state.dc_mv_ch2 = 0;

	// Reset inputs
	fetch_dac_io_set_defaults();

	// Default output channel to Analog Mode
	fetch_dac_io_to_analog(DAC_CH1);
    fetch_dac_io_to_analog(DAC_CH2);

	adcStart(&ADCD1, NULL);

	// enable the thermal sensor.
	adcSTM32EnableTSVREFE();

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
	//>! \warning on stm32f42x and stm32f43x EITHER the Temperature or Battery can be monitored-but not both
	adcSTM32EnableVBATE();
#endif

}



/*! \brief Initialize the DAC and the output channels 
 */
static void fetch_dac_init(BaseSequentialStream * chp)
{
    fetch_dac_state.dc_mv_ch1 = 0;
    fetch_dac_state.dc_mv_ch2 = 0;
    fetch_dac_state.chp       = chp;

	// enable clock to dac

	util_message_error(chp, "init: Command not yet available");

    fetch_dac_state.init      = true;
};

static inline int fetch_dac_is_valid_dac_configure(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkdac_configure)
{

	return(token_match(chp, fetch_terms->dac_configure, chkdac_configure,
	                   ((int) NELEMS(fetch_terms->dac_configure)) ) );
}


static bool fetch_dac_ch1_configure(BaseSequentialStream * chp ,
                                    Fetch_terminals * fetch_terms, char * cmd_list[], char * data_list[])
{

	if(fetch_dac_is_valid_dac_configure(chp, fetch_terms, cmd_list[DAC_CONFIGURE])) { DBG_MSG(chp, "configure Valid"); };

	// enable IO to ch1

	util_message_error(chp, "ch1 Command not yet available");
	return false;
}

static bool fetch_dac_ch2_configure(BaseSequentialStream * chp ,
                                    Fetch_terminals * fetch_terms, char * cmd_list[], char * data_list[])
{

	if(fetch_dac_is_valid_dac_configure(chp, fetch_terms, cmd_list[DAC_CONFIGURE])) {};

	util_message_error(chp, "Command not yet available");
	return false;
}

bool fetch_dac_start(void)
{
	util_message_error(getMShellStreamPtr(), "Command not yet available");
	return false;
}

bool fetch_dac_stop(void)
{
	util_message_error(getMShellStreamPtr(), "Command not yet available");
	return false;
}

static inline int fetch_dac_is_valid_dac_subcommandA(BaseSequentialStream * chp,
                Fetch_terminals * fetch_terms,
                char * chkdac_subcommandA)
{
	return(token_match(chp, fetch_terms->dac_subcommandA, chkdac_subcommandA, ((int) NELEMS(fetch_terms->dac_subcommandA)) ) );
}

/*! \brief dispatch an DAC command
 */
bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                        Fetch_terminals * fetch_terms)
{
	fetch_dac_state.chp = chp;

	if(!fetch_dac_state.init)
	{
		fetch_dac_init(chp);
	};

	if(fetch_dac_is_valid_dac_subcommandA(chp, fetch_terms, cmd_list[DAC_SCA]) >= 0)
	{
		if (strncasecmp(cmd_list[DAC_SCA], "conf_ch1", strlen("conf_ch1") ) == 0)
		{
			return(fetch_dac_ch1_configure(chp, fetch_terms, cmd_list, data_list));
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "conf_ch2", strlen("conf_ch2") ) == 0)
		{
			return(fetch_dac_ch2_configure(chp, fetch_terms, cmd_list, data_list));
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "start", strlen("start") ) == 0)
		{
			return(fetch_dac_start());
		}
		else if (strncasecmp(cmd_list[DAC_SCA], "stop", strlen("stop") ) == 0)
		{
			return(fetch_dac_stop());
		}
		else
		{
			DBG_MSG(chp, "sub-command not ready yet...");
			return(false) ;
		}
	}
	return false;
}
//! @}

