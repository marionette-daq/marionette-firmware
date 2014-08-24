/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file    dac.c
 * @brief   DAC Driver code.
 *
 * @defgroup DAC Marionette DAC Peripheral
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "dac.h"

#if HAL_USE_DAC || defined(__DOXYGEN__)

/**
 * @brief   DAC Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void dacInit(void)
{
	dac_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p DACDriver structure.
 *
 * @param[out] dacp     pointer to the @p DACDriver object
 *
 * @init
 */
void dacObjectInit(DACDriver * dacp)
{
	dacp->state         = DAC_STOP;
	dacp->config        = NULL;
}

/**
 * @brief   Configures and activates the DAC peripheral.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 * @param[in] config    pointer to the @p DACConfig object
 *
 * @api
 */
void dacStart(DACDriver * dacp, const DACConfig * config)
{

	chDbgCheck((dacp != NULL) && (config != NULL), "dacStart");

	chSysLock();
	chDbgAssert((dacp->state == DAC_STOP) || (dacp->state == DAC_READY),
	            "dacStart(), #1", "invalid state");
	dacp->config = config;
	dac_lld_start(dacp);
	dacp->state = DAC_READY;
	chSysUnlock();
}

/**
 * @brief Deactivates the DAC peripheral.
 * @note  Deactivating the peripheral also enforces a release of the slave
 *        select line.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 *
 * @api
 */
void dacStop(DACDriver * dacp)
{

	chDbgCheck(dacp != NULL, "dacStop");

	chSysLock();
	chDbgAssert((dacp->state == DAC_STOP) || (dacp->state == DAC_READY),
	            "dacStop(), #1", "invalid state");
	dac_lld_stop(dacp);
	dacp->state = DAC_STOP;
	chSysUnlock();
}
#endif

//! @}

