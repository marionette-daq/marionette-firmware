#include <stddef.h>
#include <stdbool.h>

#include "ch.h"
#include "hal.h"

#include "util_general.h"
#include "util_led.h"


static Thread * hbthd    = NULL;

static bool     hb_state = false;

/* Pre-filled led_configs */
#ifdef BOARD_ST_STM32F4_DISCOVERY
const LED GREEN          = {GPIOD, GPIO_LED4, DOWN};
const LED ORANGE         = {GPIOD, GPIO_LED3, DOWN};
const LED RED            = {GPIOD, GPIO_LED5, DOWN};
const LED BLUE           = {GPIOD, GPIO_LED6, DOWN};

LED_config led_cfg =
{
	.cycle_ms = 500,
	.start_ms = 2500,
	.led = (const struct led * [])
	{
		&GREEN,
		&ORANGE,
		&RED,
		&BLUE,
		NULL
	}
};

#elif defined BOARD_WAVESHARE_CORE407I
const LED LED1 = {GPIOH, GPIOH_PIN2, DOWN};
const LED LED2 = {GPIOH, GPIOD_PIN3, DOWN};
const LED LED3 = {GPIOI, GPIOD_PIN8, DOWN};
const LED LED4 = {GPIOI, GPIOD_PIN10, DOWN};
LED_config led_cfg =
{
	.cycle_ms = 800,
	.start_ms = 4000,
	.led = (const struct led * [])
	{
		&LED1,
		&LED2,
		&LED3,
		&LED4,
		NULL
	}
};
#else
LED_config led_cfg = {0};
#endif

void ledOn(const LED * led)
{
	if(led)
	{
		if(led->pol == DOWN)
		{
			palSetPad(led->port, led->pad);
		}
		else
		{
			palClearPad(led->port, led->pad);
		}
	}
}

void ledOff(const LED * led)
{
	if(led)
	{
		if(led->pol == DOWN)
		{
			palClearPad(led->port, led->pad);
		}
		else
		{
			palSetPad(led->port, led->pad);
		}

	}
}

void ledToggle(const LED * led)
{
	if(led)
	{
		palTogglePad(led->port, led->pad);
	}
}

static WORKING_AREA(wa_hbled, 512);
NORETURN static void hbled_thd(void * arg)
{
	struct       led_config   *  cfg           = (struct led_config *) arg;
	const struct led      **     led           = cfg->led;

	uint32_t                     start_cycles  = 0;
	systime_t                    start_blink   = 0;

	uint32_t                     hbled         = 0;
	systime_t                    cycletime     = 0;

	chRegSetThreadName("Heartbeat");

	// Initialize the led(s)
	uint8_t         num_leds                = 0;

	for(; led[num_leds]; ++num_leds)
	{
		palSetPadMode(led[num_leds]->port, led[num_leds]->pad, PAL_MODE_OUTPUT_PUSHPULL);
		ledOff(led[num_leds]);
	}

	start_cycles  = 2;
	start_blink   = cfg->cycle_ms / num_leds;

	// Start pattern
	for(int i = 0; start_cycles > 0; --start_cycles)
	{
		for(i = 0; i < num_leds; ++i)
		{
			ledOn(led[i]);
			chThdSleepMilliseconds(start_blink);
			ledOff(led[i]);
		}
		chThdSleepMilliseconds(200);
	}

	// Toggle pattern
	hbled     = 0;
	cycletime = cfg->cycle_ms;
	while (TRUE)
	{
		if(chThdShouldTerminate() )
		{
			chThdExit(THD_TERMINATE);
		}
		ledToggle(led[hbled]);
		switch(M_Status.status)
		{
			case GEN_OK:
				cycletime = cfg->cycle_ms;
				hbled = 0;
				break;
			case GEN_NOMINAL:
				cycletime = cfg->cycle_ms / 2;
				hbled = 0;
				break;
			case GEN_ERROR:
				cycletime = cfg->cycle_ms / 4;
				if(led[1]->port != NULL)
				{
					hbled = 1;
				}
				break;
			default:
				break;
		}
		chThdSleepMilliseconds(cycletime);
	}
}


void hbToggle()
{
	if(hb_state)
	{
		hbStop(&led_cfg);
	}
	else
	{
		hbStart(&led_cfg);
	}
}

void hbStop(LED_config * cfg)
{
	// If cfg is null see if a default one can be found. If it can't it is
	// set to an to an invalid config.
	if(cfg == NULL)
	{
		cfg = &led_cfg;
	}

	//Triggers if cfg isnvalid.
	if(!(cfg->led && cfg->led[0]->port))
	{
		return; //no defined leds
	}

	// Kill the heartbeat thread.
	if(hbthd != NULL ) {
		chThdTerminate(hbthd);
	}

	hb_state = false;
	
	const struct led **          led           = cfg->led;
	// UnInitialize the led(s)
	uint8_t		num_leds 		= 0;

	for(; led[num_leds]; ++num_leds)
	{
		palSetPadMode(led[num_leds]->port, led[num_leds]->pad, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING);
		ledOff(led[num_leds]);
	}
}


void hbStart(LED_config * cfg)
{
	// If cfg is null see if a default one can be found. If it can't it is
	// set to an to an invalid config.
	if(cfg==NULL)
	{
		cfg = &led_cfg;
	}

	//Triggers if cfg isnvalid.
	if(!(cfg->led && cfg->led[0]->port))
	{
		return; //no defined leds
	}
	chDbgAssert(cfg->cycle_ms > 0, DBG_PREFIX "LED cycle time > 0", NULL);

	hb_state = true;
	hbthd = chThdCreateStatic(wa_hbled, sizeof(wa_hbled), NORMALPRIO, (tfunc_t)hbled_thd, cfg);
}


