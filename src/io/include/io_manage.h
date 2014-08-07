/*! \file fetch_dac.h
 *
 * @addtogroup fetch_dac
 * @{
 */

#ifndef FETCH_DAC_H_
#define FETCH_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ch.h"
#include "hal.h"
#include "board.h"

typedef enum io_alloc {
	IO_NONE  = 0b0,
	IO_OTHER = 0b1,
	IO_GPIO  = 0b10,
	IO_ADC   = 0b100,
	IO_DAC   = 0b1000,
	IO_SPI   = 0b10000,
	IO_I2C   = 0b100000,
	IO_ALL   = 0xffff
} IO_alloc;

typedef struct io_state {
    const ioportid_t port;
    const uint32_t   pad;
	const iomode_t   default_mode;
	iomode_t         current_mode;
	IO_alloc         current_alloc;
	uint16_t         conflicts;
} IO_state;


#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static IO_state A_0  = { .port=GPIOA, .pad=GPIOA_PIN0,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_1  = { .port=GPIOA, .pad=GPIOA_PIN1,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_2  = { .port=GPIOA, .pad=GPIOA_PIN2,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_3  = { .port=GPIOA, .pad=GPIOA_PIN3,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_OTHER, IO_ALL } ;  // used for OTG
static IO_state A_4  = { .port=GPIOA, .pad=GPIOA_PIN4,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_5  = { .port=GPIOA, .pad=GPIOA_PIN5,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_6  = { .port=GPIOA, .pad=GPIOA_PIN6,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_7  = { .port=GPIOA, .pad=GPIOA_PIN7,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_8  = { .port=GPIOA, .pad=GPIOA_PIN8,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_9  = { .port=GPIOA, .pad=GPIOA_OTG_FS_VBUS, .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_10 = { .port=GPIOA, .pad=GPIOA_OTG_FS_ID,   .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_11 = { .port=GPIOA, .pad=GPIOA_OTG_FS_DM,   .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_OTHER, IO_ALL } ;  // used for OTG
static IO_state A_12 = { .port=GPIOA, .pad=GPIOA_OTG_FS_DP,   .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_OTHER, IO_ALL } ;  // used for OTG
static IO_state A_13 = { .port=GPIOA, .pad=GPIOA_SWDIO,       .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_14 = { .port=GPIOA, .pad=GPIOA_SWDCLK,      .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;
static IO_state A_15 = { .port=GPIOA, .pad=GPIOA_JTDI,        .default_mode=PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode=PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_alloc=IO_GPIO, IO_NONE } ;

static IO_state * IO_set[] = {


} ;
#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for ADC"
#else
#error "Board not defined for ADC"
#endif






#ifdef __cplusplus
}
#endif

#endif


//! @}


