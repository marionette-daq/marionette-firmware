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

#include "chprintf.h"

#include "mshell.h"
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

/*! \brief Show running threads
 *
 * @param chp
 * @param argc
 * @param argv
 */
void cmd_threads(BaseSequentialStream * chp, int argc, char * argv[])
{
	(void)chp;
	static const char * states[] = {THD_STATE_NAMES};
	Thread * tp;

	(void)argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "addr\t\tstack\t\tprio\trefs\tstate\t\ttime\tname\r\n");
	tp = chRegFirstThread();
	do
	{
		chprintf(chp, "%.8lx\t%.8lx\t%4lu\t%4lu\t%9s\t%lu\t%s\r\n",
		         (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
		         (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
		         states[tp->p_state], (uint32_t)tp->p_time, tp->p_name);
		tp = chRegNextThread(tp);
	}
	while (tp != NULL);
}

static const MShellCommand commands[] =
{
	{"mem", cmd_mem},
	{"threads", cmd_threads},
	{NULL, NULL}
};

static const MShellConfig shell_cfg1 =
{
	(BaseSequentialStream *) & SDU1,
	commands
};


int main(void)
{
	Thread             *            mshelltp = NULL;
	static          VERSIONData     version_data;

	halInit();
	chSysInit();

	mshellInit();

	util_hwversion(&version_data);
	usb_set_serial_strings(version_data.hardware.id_high, version_data.hardware.id_center,
	                       version_data.hardware.id_low);
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	while (TRUE)
	{
		if (!mshelltp)
		{
			if (SDU1.config->usbp->state == USB_ACTIVE)
			{
				mshelltp = mshellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
			}
		}
		else
		{
			if (chThdTerminated(mshelltp))
			{
				chThdRelease(mshelltp);
				mshelltp = NULL;
			}
		}
		chThdSleepMilliseconds(500);
	}
	return(0);
}


