/*! \file dac_lld.h */

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

/*!
 *
 * @addtogroup M_DAC
 * @{
 */

#ifndef DAC_LLD_H_
#define DAC_LLD_H_

#if HAL_USE_DAC || defined(__DOXYGEN__)

#define STM32_HAS_DAC_CH1 TRUE
#define STM32_HAS_DAC_CH2 TRUE

/**
 * @name    DAC peripheral specific RCC operations
 * @{
 */
/**
 * @brief   Enables the DAC peripheral clock.
 *
 * @param[in] lp        low power enable flag
 *
 * @api
 */
#define rccEnableDAC(lp) rccEnableAPB1(RCC_APB1ENR_DACEN, lp)

/**
 * @brief   Disables the DAC peripheral clock.
 *
 * @param[in] lp        low power enable flag
 *
 * @api
 */
#define rccDisableDAC(lp) rccDisableAPB1(RCC_APB1ENR_DACEN, lp)

/**
 * @brief   Resets the DAC peripheral.
 *
 * @api
 */
#define rccResetDAC() rccResetAPB1(RCC_APB1RSTR_DACRST)
/** @} */


/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#define STM32_DAC_CR_EN             DAC_CR_EN1

#define STM32_DAC_CR_MASK           (uint32_t)0x00000FFE

#define STM32_DAC_CR_BOFF_ENABLE    (uint32_t)0x00000000
#define STM32_DAC_CR_BOFF_DISABLE   DAC_CR_BOFF1

#define STM32_DAC_CR_TSEL_NONE      (uint32_t)0x00000000
#define STM32_DAC_CR_TSEL_TIM2      DAC_CR_TEN1 | DAC_CR_TSEL1_2
#define STM32_DAC_CR_TSEL_TIM4      DAC_CR_TEN1 | DAC_CR_TEN0 | DAC_CR_TSEL1_2
#define STM32_DAC_CR_TSEL_TIM5      DAC_CR_TEN1 | DAC_CR_TEN0 | DAC_CR_TSEL1_1
#define STM32_DAC_CR_TSEL_TIM6      DAC_CR_TEN1
#define STM32_DAC_CR_TSEL_TIM7      DAC_CR_TEN1 | DAC_CR_TSEL1_1
#define STM32_DAC_CR_TSEL_TIM3      DAC_CR_TEN1 | DAC_CR_TSEL1_0
#define STM32_DAC_CR_TSEL_TIM18     DAC_CR_TEN1 | DAC_CR_TSEL1_0 | DAC_CR_TSEL1_1
#define STM32_DAC_CR_TSEL_EXT_IT9   DAC_CR_TEN1 | DAC_CR_TEN1 | DAC_CR_TSEL1_2
#define STM32_DAC_CR_TSEL_SOFT      DAC_CR_TEN1 | DAC_CR_TEN0 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_2

#define STM32_DAC_CR_WAVE_NONE      (uint32_t)0x00000000
#define STM32_DAC_CR_WAVE_NOISE     DAC_CR_WAVE1_0
#define STM32_DAC_CR_WAVE_TRIANGLE  DAC_CR_WAVE1_1

#define STM32_DAC_MAMP_1            (uint32_t)0x00000000
#define STM32_DAC_MAMP_3            DAC_CR_MAMP1_0
#define STM32_DAC_MAMP_7            DAC_CR_MAMP1_1
#define STM32_DAC_MAMP_15           DAC_CR_MAMP1_0 | DAC_CR_MAMP1_1
#define STM32_DAC_MAMP_31           DAC_CR_MAMP1_2
#define STM32_DAC_MAMP_63           DAC_CR_MAMP1_0 | DAC_CR_MAMP1_2
#define STM32_DAC_MAMP_127          DAC_CR_MAMP1_1 | DAC_CR_MAMP1_2
#define STM32_DAC_MAMP_255          DAC_CR_MAMP1_0 | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_2
#define STM32_DAC_MAMP_511          DAC_CR_MAMP1_3
#define STM32_DAC_MAMP_1023         DAC_CR_MAMP1_0 | DAC_CR_MAMP1_3
#define STM32_DAC_MAMP_2047         DAC_CR_MAMP1_1 | DAC_CR_MAMP1_3
#define STM32_DAC_MAMP_4095         DAC_CR_MAMP1_0 | DAC_CR_MAMP1_1 | DAC_CR_MAMP1_2

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   DAC CH1 driver enable switch.
 * @details If set to @p TRUE the support for DAC CH1 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_DAC_USE_CH1) || defined(__DOXYGEN__)
#define STM32_DAC_USE_CH1       TRUE
#endif

/**
 * @brief   DAC CH2 driver enable switch.
 * @details If set to @p TRUE the support for DAC CH2 is included.
 * @note    The default is @p TRUE.
 */
#if !defined(STM32_DAC_USE_CH2) || defined(__DOXYGEN__)
#define STM32_DAC_USE_CH2       TRUE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/
#if STM32_DAC_USE_CH1 && !STM32_HAS_DAC_CH1
#error "DAC CH1 not present in the selected device"
#endif

#if STM32_DAC_USE_CH2 && !STM32_HAS_DAC_CH2
#error "DAC CH2 not present in the selected device"
#endif

#if !STM32_DAC_USE_CH1  && !STM32_DAC_USE_CH2
#error "DAC driver activated but no DAC channels assigned"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Type of a structure representing an DAC driver.
 */
typedef struct DACDriver DACDriver;

/**
 * @brief   Type representing a DAC sample.
 */
typedef uint16_t dacsample_t;

typedef enum
{
	DAC_DHRM_12BIT_RIGHT      = 0,
	DAC_DHRM_12BIT_LEFT       = 1,
	DAC_DHRM_8BIT_RIGHT       = 2,
	DAC_DHRM_12BIT_RIGHT_DUAL = 3,
	DAC_DHRM_12BIT_LEFT_DUAL  = 4,
	DAC_DHRM_8BIT_RIGHT_DUAL  = 5
} dacdhrmode_t;

/**
 * @brief   Driver configuration structure.
 */
typedef struct
{
	/**
	 * @brief   DAC data holding register mode.
	 */
	dacdhrmode_t       dhrm;
	/**
	 * @brief DAC initialization data.
	 */
	uint32_t           cr_flags;
} DACConfig;

/**
 * @brief   Structure representing a DAC driver.
 */
struct DACDriver
{
	/**
	 * @brief Driver state.
	 */
	dacstate_t         state;

	/**
	 * @brief Current configuration data.
	 */
	const DACConfig  * config;

	/**
	 * @brief Pointer to the DAC registers block.
	 */
	DAC_TypeDef    *   dac;
};

#if !defined(__DOXYGEN__)
extern DACDriver DACD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
void dac_lld_init(void);
void dac_lld_start(DACDriver * dacp);
void dac_lld_stop(DACDriver * dacp);
//void dac_lld_start_conversion(DACDriver * dacp);
//void dac_lld_stop_conversion(DACDriver * dacp);
#ifdef __cplusplus
}
#endif

#endif

#endif

/** @} */

