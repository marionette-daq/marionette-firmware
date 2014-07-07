/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"
#include "shell.h"
#include "lis302dl.h"
#include "board.h"

#include "fetch.h"
#include "util_version.h"
#include "usbcfg.h"

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

#define SHELL_WA_SIZE   THD_WA_SIZE(8192)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

static void cmd_mem(BaseSequentialStream * chp, int argc, char * argv[])
{
	size_t n, size;
	(void)argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream * chp, int argc, char * argv[])
{
	static const char * states[] = {THD_STATE_NAMES};
	Thread * tp;
	(void)argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do
	{
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
		         (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
		         (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
		         states[tp->p_state], (uint32_t)tp->p_time);
		tp = chRegNextThread(tp);
	}
	while (tp != NULL);
}

static void cmd_test(BaseSequentialStream * chp, int argc, char * argv[])
{
	Thread * tp;
	(void)argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: test\r\n");
		return;
	}
	tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
	                         TestThread, chp);
	if (tp == NULL)
	{
		chprintf(chp, "out of memory\r\n");
		return;
	}
	chThdWait(tp);
}

static const ShellCommand commands[] =
{
	{"mem", cmd_mem},
	{"threads", cmd_threads},
	{"test", cmd_test},
	{NULL, NULL}
};

static const ShellConfig shell_cfg1 =
{
	(BaseSequentialStream *) & SDU1,
	commands
};

/*
 * PWM configuration structure.
 * Cyclic callback enabled
 * the active state is a logic one.
 */
static const PWMConfig pwmcfg =
{
	100000,                                   /* 100kHz PWM clock frequency.  */
	128,                                      /* PWM period is 128 cycles.    */
	NULL,
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}
	},
	/* HW dependent part.*/
	0,
	0
};

/* heartbeat example thread */
static WORKING_AREA(waHBThread, 128);
static msg_t HBThread(void * arg)
{
	systime_t time;                   /* Next deadline.*/
	(void)arg;
	chRegSetThreadName("heartbeat");
	bool up = true;

	pwmcnt_t        pwm_cnt = 0;

	time = chTimeNow();
	while (TRUE)
	{
		uint16_t i;
		const uint16_t highcount = 50;

		if(up)
		{
			pwm_cnt = (pwm_cnt + 1);
			if(pwm_cnt % highcount == 0)
			{
				up = !up;
			}
		}
		else
		{
			pwm_cnt = (pwm_cnt - 1);
			if(pwm_cnt % highcount == 0)
			{
				up = !up;
			}
		}
		pwmEnableChannel(&PWMD4, 0, pwm_cnt);
		/* Waiting until the next N milliseconds time interval.*/
		chThdSleepUntil(time += MS2ST(40));
	}
	return 0;
}

int main(void)
{
	Thread * shelltp = NULL;

	halInit();
	chSysInit();

	shellInit();

	static VERSIONData version_data;
	util_hwversion(&version_data);
	usb_set_serial_strings(version_data.hardware.id_high, version_data.hardware.id_center,
	                       version_data.hardware.id_low);
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));      /* Green.   */

	chThdCreateStatic(waHBThread, sizeof(waHBThread),
	                  NORMALPRIO + 10, HBThread, NULL);

	while (TRUE)
	{
		if (!shelltp)
		{
			if (SDU1.config->usbp->state == USB_ACTIVE)
			{
				shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
			}
		}
		else
		{
			if (chThdTerminated(shelltp))
			{
				chThdRelease(shelltp);
				shelltp = NULL;
			}
		}
		chThdSleepMilliseconds(500);
	}
	return(0);
}

