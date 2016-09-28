#include <string.h>
#include <ctype.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "util_general.h"
#include "util_strings.h"
#include "util_messages.h"
#include "util_version.h"

#include "fetch_adc.h"

#include "mcard.h"

#ifndef MCARD_ADC_MB_SIZE
#define MCARD_ADC_MB_SIZE 128
#endif

#ifndef MCARD_CAN_MB_SIZE
#define MCARD_CAN_MB_SIZE 128
#endif

msg_t mcard_adc2_mb_buffer[MCARD_ADC_MB_SIZE];
mailbox_t mcard_adc2_mb;
msg_t mcard_adc3_mb_buffer[MCARD_ADC_MB_SIZE];
mailbox_t mcard_adc3_mb;


static void mcard_out_thread(void * p)
{
	chRegSetThreadName("mcard_out");
  msg_t msg;

  while(1)
  {
    // TODO
		chThdSleepMilliseconds(1);
  }
}

static void mcard_in_thread(void * p)
{
	chRegSetThreadName("mcard_in");
  
  while(1)
  {
    // TODO
		chThdSleepMilliseconds(1);
  }
}


void mcardInit(void)
{
  chMBObjectInit(&mcard_adc2_mb, mcard_adc2_mb_buffer, MCARD_ADC_MB_SIZE);
  chMBObjectInit(&mcard_adc3_mb, mcard_adc3_mb_buffer, MCARD_ADC_MB_SIZE);
}


