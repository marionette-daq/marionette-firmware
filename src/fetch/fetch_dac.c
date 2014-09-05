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
#include <math.h>
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

#include "dac.h"
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

bool fetch_dac_start(void);
bool fetch_dac_stop(void);

static DACConfig  fetch_dac_config_ch1 =
{
	.dhrm     = DAC_DHRM_12BIT_RIGHT,
	.cr_flags = STM32_DAC_CR_EN
};

/*! \brief track the state of the dac
 */
static FETCH_dac_state fetch_dac_state =
{
	.vref_mv              = 3300,
	.dc_mv_ch1            = 0,
	.dc_mv_ch2            = 0,
	.init                 = false,
	.config               = &fetch_dac_config_ch1,
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

/*! \brief Initialize the DAC and the output channels
 */
static void fetch_dac_init(BaseSequentialStream * chp)
{
	fetch_dac_state.dc_mv_ch1 = 0;
	fetch_dac_state.dc_mv_ch2 = 0;
	fetch_dac_state.chp       = chp;

	dacInit();

	// Default output channel to Analog Mode
	fetch_dac_io_to_analog(DAC_CH1);
	fetch_dac_io_to_analog(DAC_CH2);

	fetch_dac_state.init      = true;
	fetch_dac_start();

	// start at 0v
	DACD1.dac->DHR12R1 = 0;
	DACD1.dac->DHR12R2 = 0;

};

/*! \brief Reset dac
 */
static void fetch_dac_reset(void)
{
	fetch_dac_state.dc_mv_ch1 = 0;
	fetch_dac_state.dc_mv_ch2 = 0;

	fetch_dac_state.init      = false;
	fetch_dac_stop();

	// Reset inputs
	fetch_dac_io_set_defaults();

	// Default output channel to Analog Mode
	fetch_dac_io_to_analog(DAC_CH1);
	fetch_dac_io_to_analog(DAC_CH2);

	fetch_dac_state.init      = true;
	fetch_dac_start();

	DACD1.dac->DHR12R1 = 0;
	DACD1.dac->DHR12R2 = 0;
}

static bool fetch_dac_dc(int dc_mv, DACChannel ch)
{
	uint32_t    ch_dor      = 0;

	//! \todo code review: Asserts are user-unfriendly
	// chDbgAssert((dc_mv >= 0), "fetch_dac_dc(), #1", "No negative DAC voltages.");
	if(dc_mv < 0)
	{
		util_message_error(getMShellStreamPtr(), "Negative voltage is not possible for DAC");
		return false;
	}

	if(dc_mv > fetch_dac_state.vref_mv)
	{
		dc_mv = fetch_dac_state.vref_mv;
		util_message_info(getMShellStreamPtr(), "Setting to Vref (maximum voltage)");
	}

	/*! STM32F4 Reference, Rev 7, page 434, Section 14.3.5
	 * \warning gcc lrint(...) default mode round-to-nearest
	 */
	ch_dor = (int) (lrint(((dc_mv) * 4095 ) / fetch_dac_state.vref_mv));

	if(ch == DAC_CH1)
	{
		fetch_dac_state.dc_mv_ch1 = dc_mv;
		DACD1.dac->DHR12R1 = ch_dor;
	}
	if(ch == DAC_CH2)
	{
		fetch_dac_state.dc_mv_ch2 = dc_mv;
		DACD1.dac->DHR12R2 = ch_dor;
	}

	return true;
}

static bool fetch_dac_ch1_configure(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if (strncasecmp(cmd_list[DAC_CONFIGURE], "dc_mv", strlen("dc_mv") ) == 0)
  {
    return(fetch_dac_dc(atoi(data_list[0]), DAC_CH1));
  }
	return false;
}

static bool fetch_dac_ch2_configure(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  if (strncasecmp(cmd_list[DAC_CONFIGURE], "dc_mv", strlen("dc_mv") ) == 0)
  {
    return(fetch_dac_dc(atoi(data_list[0]), DAC_CH2));
  }
	return false;
}

static bool fetch_dac_release(void)
{
	fetch_dac_state.dc_mv_ch1 = 0;
	fetch_dac_state.dc_mv_ch2 = 0;

	DACD1.dac->DHR12R1 = 0;
	DACD1.dac->DHR12R2 = 0;

	fetch_dac_stop();
	fetch_dac_state.init      = false;

	// Reset inputs
	fetch_dac_io_set_defaults();

	return true;
}

bool fetch_dac_start()
{
	if(fetch_dac_state.init)
	{
		dacStart(&DACD1, &fetch_dac_config_ch1);
		return true;
	}
	else
	{
		util_message_error(getMShellStreamPtr(), "DAC is not initialized. %d", fetch_dac_state.init);
		return false;
	}
	return false;
}

bool fetch_dac_stop()
{
	dacStop(&DACD1);
	return true;
}

/*! \brief dispatch an DAC command
 */
bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
	fetch_dac_state.chp = chp;

	if(!fetch_dac_state.init)
	{
		fetch_dac_init(chp);
	};

  if (strncasecmp(cmd_list[DAC_SCA], "conf_ch1", strlen("conf_ch1") ) == 0)
  {
    return(fetch_dac_ch1_configure(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[DAC_SCA], "conf_ch2", strlen("conf_ch2") ) == 0)
  {
    return(fetch_dac_ch2_configure(chp, cmd_list, data_list));
  }
  else if (strncasecmp(cmd_list[DAC_SCA], "vref_mv", strlen("vref_mv") ) == 0)
  {
    if(data_list[0] != NULL)
    {
      fetch_dac_state.vref_mv = atoi(data_list[0]);
    }
    return true;
  }
  else if (strncasecmp(cmd_list[DAC_SCA], "start", strlen("start") ) == 0)
  {
    return(fetch_dac_start());
  }
  else if (strncasecmp(cmd_list[DAC_SCA], "stop", strlen("stop") ) == 0)
  {
    return(fetch_dac_stop());
  }
  else if (strncasecmp(cmd_list[DAC_SCA], "release", strlen("release") ) == 0)
  {
    return(fetch_dac_release());
  }
  else
  {
    DBG_MSG(chp, "sub-command not ready yet...");
    return(false) ;
  }
}
//! @}

