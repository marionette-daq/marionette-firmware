#include <stddef.h>

#include "ch.h"
#include "hal.h"

#include "util_general.h"
#include "util_led.h"

/* Pre-filled led_configs */
#ifdef BOARD_ST_STM32F4_DISCOVERY
const struct led GREEN          = {GPIOD, GPIO_LED4, DOWN};
const struct led ORANGE         = {GPIOD, GPIO_LED3, DOWN};
const struct led RED            = {GPIOD, GPIO_LED5, DOWN};
const struct led BLUE           = {GPIOD, GPIO_LED6, DOWN};

static struct led_config led_cfg =
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
#elif defined BOARD_OLIMEX_STM32_E407
const struct led GREEN = {GPIOC, GPIOC_LED, UP};

static struct led_config led_cfg =
{
	.cycle_ms = 500,
	.start_ms = 0,
	.led = (const struct led * [])
	{
		&GREEN,
		NULL
	}
};
#elif defined BOARD_WAVESHARE_CORE407I
const struct led LEDA = {GPIOD, GPIOD_LED2, DOWN};
const struct led LEDB = {GPIOD, GPIOD_LED4, DOWN};
const struct led LEDC = {GPIOD, GPIOD_LED5, DOWN};
const struct led LEDD = {GPIOD, GPIOD_RGB_R, DOWN};
static struct led_config led_cfg =
{
	.cycle_ms = 500,
	.start_ms = 4000,
	.led = (const struct led * [])
	{
		&LEDA,
		&LEDB,
		&LEDC,
		&LEDD,
		NULL
	}
};
#else
static struct led_config led_cfg = {0};
#endif

#define SIZE 32
static msg_t buffer[SIZE];
MAILBOX_DECL(mailbox, buffer, SIZE);

#define NOMINAL 1
#define ERROR 2

void ledError(void)
{
	chMBPost(&mailbox, ERROR, TIME_IMMEDIATE);
}

void ledNominal(void)
{
	chMBPost(&mailbox, NOMINAL, TIME_IMMEDIATE);
}

void ledOn(const struct led * led)
{
	if(led)
	{
		if(led->LED_polarity == DOWN)
		{
			palSetPad(led->port, led->pad);
		}
		else
		{
			palClearPad(led->port, led->pad);
		}
	}
}

void ledOff(const struct led * led)
{
	if(led)
	{
		if(led->LED_polarity == DOWN)
		{
			palClearPad(led->port, led->pad);
		}
		else
		{
			palSetPad(led->port, led->pad);
		}

	}
}

void ledToggle(const struct led * led)
{
	if(led)
	{
		palTogglePad(led->port, led->pad);
	}
}

static WORKING_AREA(wa_led, 512); //fixme mailboxes demand a bizarrely large amount of space
NORETURN static void led(void * arg)
{
	struct led_config * cfg = (struct led_config *) arg;
	const struct led ** led = cfg->led;

	chRegSetThreadName("LED");

	/* Turn off leds, also count them */
	int 		num_leds 		= 0;

	for(; led[num_leds]; ++num_leds)
	{
		palSetPadMode(led[num_leds]->port, led[num_leds]->pad, PAL_MODE_OUTPUT_PUSHPULL);
		ledOff(led[num_leds]);
	}

	unsigned start_cycles = cfg->start_ms / cfg->cycle_ms;
	systime_t start_blink = cfg->cycle_ms / num_leds;

	/* Run the start pattern */
	int i = 0;
	for(; start_cycles > 0; --start_cycles)
	{
		for(i = 0; i < num_leds; ++i)
		{
			ledOn(led[i]);
			chThdSleepMilliseconds(start_blink);
			ledOff(led[i]);
		}
		chThdSleepMilliseconds(cfg->cycle_ms % num_leds);
	}

	/* Run the toggle pattern */
	int activeled = 0;
	systime_t cycletime = cfg->cycle_ms;
	while (TRUE)
	{
		ledToggle(led[activeled]);
		msg_t msg = 0;
		chMBFetch(&mailbox, &msg, cycletime);
		switch(msg)
		{
			case NOMINAL:
				cycletime = cfg->cycle_ms;
				activeled = 0;
				break;
			case ERROR:
				cycletime = cfg->cycle_ms / 2;
				if(led[1]->port != NULL)
				{
					activeled = 1;
				}
				break;
		}
	}
}

void ledStart(struct led_config * cfg)
{
	// If cfg is null see if a default one can be found. If it can't it is
	// set to an to an invalid config.
	if(!cfg)
	{
		cfg = &led_cfg;
	}

	//Triggers if cfg isnvalid.
	if(!(cfg->led && cfg->led[0]->port))
	{
		return; //no defined leds
	}
	chDbgAssert(cfg->cycle_ms > 0, DBG_PREFIX "LED cycle time must be positive", NULL);

	chThdCreateStatic(wa_led, sizeof(wa_led), NORMALPRIO, (tfunc_t)led, cfg);
}


