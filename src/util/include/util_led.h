/*! \file util_led

  Attribution:
  Derived from similar work at:
        github.com/psas/stm32.git/common/utils_led.c

  License:
    The license is believed to be the same as ChibiOS/license.txt (GPLv3)
*/

/*!
 * Includes:
 *   Led blinker thread, useful for diagnostics.
 *   Specific existing board definitions
 */

#ifndef UTIL_LED_H_
#define UTIL_LED_H_

#include "ch.h"
#include "hal.h"

typedef enum led_polarity
{
	UP,           //>! LED is connected through VDD
	DOWN          //>! LED is connected through VSS
} LED_polarity;

typedef struct led
{
	ioportid_t                  port;
	uint32_t                    pad;
	LED_polarity                pol;            //>! some LED's are connected through VDD, others through VSS.             
} LED;

typedef struct led_config
{
	const systime_t                       cycle_ms;   //>! Main sequence led toggle time, must be positive
	const systime_t                       start_ms;   //>! Start pattern duration
	const  LED             **       led;        //>! Null terminated array of led pointers
} LED_config;

/* Board dependent physical leds */
#ifdef BOARD_ST_STM32F4_DISCOVERY
extern const LED             GREEN;
extern const LED             ORANGE;
extern const LED             RED;
extern const LED             BLUE;
#elif defined BOARD_WAVESHARE_CORE407I
extern const LED             LED1;
extern const LED             LED2;
extern const LED             LED3;
extern const LED             LED4;
#endif

extern Util_status   M_Status;
/*!
 * Suggested project code usage:
 *
 *   Each project defines a set of logical leds and then maps physical leds
 *   to the logical leds in a board dependent way.
 *
 *    logical leds (Defining them in this way lets them default to null):
 *
 *            extern const struct led * behavior_alpha;
 *            extern const struct led * on_bar;
 *
 *    map:
 *        #ifdef BOARD_BAR
 *            const struct led * behavior_alpha = &RED;
 *            const struct led * on_bar = &BLUE;
 *            #elif define FOOBOARD
 *            const struct led * behavior_1 = &GREEN;
 *            on_bar has no corresponding physical led
 *        #endif
 */

/* Functions that operate on LED structures, does nothing if null is passed */
void ledOn(const struct led * led);
void ledOff(const struct led * led);
void ledToggle(const struct led * led);

/*!
 * Starts a thread that will:
 * 1. Turn off all given leds.
 * 2. Cycle through all leds for start_ms milliseconds. start_ms should be
 *    greater than cycle_ms.
 * 3. Then toggle the first led in the list every cycle_ms milliseconds.
 *
 * If the argument to led_init is null it will attempt to find a configuration
 * suitable for the board it was compiled for. Otherwise it will raise an
 * assertion.
 */
void hbStart(LED_config * cfg);


/*! Stops a thread if it is running */
void hbStop(LED_config * cfg);

/*!
 * Indicate an error has occurred.
 * Will cause the led to flash at twice the normal rate and, if it exists,
 * change the blinking led to the second in the list.
 */
void ledError(void);

/*!
 * Change led blinking to the default behavior.
 */
void ledNominal(void);

#endif

