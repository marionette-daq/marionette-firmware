/*! \file    STM32/DACv1/dac_lld.c */

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

/* @brief   STM32 DAC subsystem low level driver source.
*
* @defgroup M_DAC Marionette DAC Low Level Peripheral Driver
* @{
*/

#include "ch.h"
#include "hal.h"

#if HAL_USE_DAC || defined(__DOXYGEN__)

#include "dac.h"
#include "util_messages.h"
#include "mshell_state.h"
/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if !defined(DAC1)
#define DAC1 DAC
#define rccEnableDAC1 rccEnableDAC
#define rccDisableDAC1 rccDisableDAC
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief CHN1 driver identifier.*/
#if STM32_DAC_USE_CHN1 || defined(__DOXYGEN__)
DACDriver DACD1;
#endif

/** @brief CHN2 driver identifier.*/
#if STM32_DAC_USE_CHN2 || defined(__DOXYGEN__)
DACDriver DACD2;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level DAC driver initialization.
 *
 * @notapi
 */
void dac_lld_init(void)
{
	dacObjectInit(&DACD1);
	DACD1.dac     = DAC1;
}

/**
 * @brief   Configures and activates the DAC peripheral.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 *
 * @notapi
 */
void dac_lld_start(DACDriver * dacp)
{
	uint32_t arr, regshift, trgo, dataoffset;
	bool_t b;
	/* If in stopped state then enables the DAC and DMA clocks.*/
	if (dacp->state == DAC_STOP)
	{
#if STM32_DAC_USE_CHN1
		if (&DACD1 == dacp)
		{
			rccEnableDAC1(FALSE);
			/* DAC1 CR data is at bits 0:15 */
			dacp->dac->CR |= (STM32_DAC_CR_EN | dacp->config->cr_flags);
		}
#endif
#if STM32_DAC_USE_CHN2
		if (&DACD1 == dacp)
		{
			rccEnableDAC1(FALSE);
			/* DAC1 CR data is at bits 16:31 */
			dacp->dac->CR |= ((STM32_DAC_CR_EN << 16) | dacp->config->cr_flags);
		}
#endif
	}
}

/**
 * @brief   Deactivates the DAC peripheral.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 *
 * @notapi
 */
void dac_lld_stop(DACDriver * dacp)
{
	/* If in ready state then disables the DAC clock.*/
	if (dacp->state == DAC_READY)
	{

#if STM32_DAC_USE_CHN1
		if (&DACD1 == dacp)
		{
			dacp->dac->CR &= ~STM32_DAC_CR_EN; /* DAC1 disable.*/
		}
#endif
#if STM32_DAC_USE_CHN2
		if (&DACD2 == dacp)
		{
			dacp->dac->CR &= ~STM32_DAC_CR_EN << 16; /* DAC1 disable.*/
		}
#endif

		dacp->state = DAC_STOP;

		if (!(DAC1->CR & (STM32_DAC_CR_EN | STM32_DAC_CR_EN << 16)))
		{
			/* DAC Clock disable only if all channels are off.*/
			rccDisableDAC1(FALSE);
		}
	}
}

#endif /* HAL_USE_DAC */

/** @} */

