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

/*! \file    dac.h
 * @brief   DAC Driver macros and structures.
 *
 * @addtogroup DAC
 * @{
 */

#ifndef  DAC_H_
#define  DAC_H_

#if HAL_USE_DAC || defined(__DOXYGEN__)

/**
 * @brief   Driver state machine possible states.
 */
typedef enum
{
	DAC_UNINIT = 0,                   /**< Not initialized.                   */
	DAC_STOP = 1,                     /**< Stopped.                           */
	DAC_READY = 2,                    /**< Ready.                             */
	DAC_ACTIVE = 3,                   /**< Exchanging data.                   */
	DAC_COMPLETE = 4,                 /**< Asynchronous operation complete.   */
	DAC_ERROR = 5                                 /**< Error.                             */
} dacstate_t;

#include "dac_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void dacInit(void);
void dacObjectInit(DACDriver * dacp);
void dacStart(DACDriver * dacp, const DACConfig * config);
void dacStop(DACDriver * dacp);
#ifdef __cplusplus
}
#endif

#endif

#endif

//! @}

