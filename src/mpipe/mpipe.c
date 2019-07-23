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

#include "mpipe.h"

#ifndef MPIPE_ADC_MB_SIZE
#define MPIPE_ADC_MB_SIZE 16
#endif

#ifndef MPIPE_CAN_MB_SIZE
#define MPIPE_CAN_MB_SIZE 8
#endif

#ifndef MPIPE_INPUT_WA_SIZE
#define MPIPE_INPUT_WA_SIZE  128
#endif

#ifndef MPIPE_ADC_WA_SIZE
#define MPIPE_ADC_WA_SIZE  128
#endif

#ifndef MPIPE_CAN_WA_SIZE
#define MPIPE_CAN_WA_SIZE  128
#endif

#ifndef MPIPE_SERIAL_WA_SIZE
#define MPIPE_SERIAL_WA_SIZE  128
#endif

// mutex used to control access to printing out on mpipe stream
mutex_t mpipe_output_mutex;

thread_t * mpipe_input_tp = NULL;
thread_t * mpipe_adc2_tp = NULL;
thread_t * mpipe_adc3_tp = NULL;
thread_t * mpipe_can_tp = NULL;
thread_t * mpipe_serial_tp = NULL;

static THD_WORKING_AREA(mpipe_input_wa, MPIPE_INPUT_WA_SIZE);
static THD_WORKING_AREA(mpipe_adc2_wa, MPIPE_ADC_WA_SIZE);
static THD_WORKING_AREA(mpipe_adc3_wa, MPIPE_ADC_WA_SIZE);
static THD_WORKING_AREA(mpipe_can_wa, MPIPE_CAN_WA_SIZE);
static THD_WORKING_AREA(mpipe_serial_wa, MPIPE_SERIAL_WA_SIZE);

msg_t mpipe_adc2_mb_buffer[MPIPE_ADC_MB_SIZE];
mailbox_t mpipe_adc2_mb;

msg_t mpipe_adc3_mb_buffer[MPIPE_ADC_MB_SIZE];
mailbox_t mpipe_adc3_mb;

msg_t mpipe_can_mb_buffer[MPIPE_CAN_MB_SIZE];
mailbox_t mpipe_can_mb;

#define IS_EOL(x) (x == '\n' || x == '\r')

static bool parse_hex(uint8_t c, uint8_t * output)
{
  if( c >= '0' && c <= '9' )
  {
    *output = (c - '0');
  }
  else if( c >= 'A' && c <= 'F' )
  {
    *output = (c - 'A' + 10);
  }
  else if( c >= 'a' && c <= 'f' )
  {
    *output = (c - 'a' + 10);
  }
  else
  {
    return false;
  }
  return true;
}

static void print_hex_nibble(BaseSequentialStream *chp, uint8_t data)
{
  if( chp == NULL )
    return;

  data &= 0xf;

  if( data < 10 )
  {
    streamPut(chp, '0' + data);
  }
  else
  {
    streamPut(chp, 'A' + data - 10);
  }
}

static void print_hex8(BaseSequentialStream *chp, uint8_t data)
{
  print_hex_nibble(chp, data >> 4);
  print_hex_nibble(chp, data);
}

static void print_hex16(BaseSequentialStream *chp, uint16_t data)
{
  print_hex_nibble(chp, data >> 12);
  print_hex_nibble(chp, data >> 8);
  print_hex_nibble(chp, data >> 4);
  print_hex_nibble(chp, data);
}

static void print_hex32(BaseSequentialStream *chp, uint16_t data)
{
  print_hex_nibble(chp, data >> 28);
  print_hex_nibble(chp, data >> 24);
  print_hex_nibble(chp, data >> 20);
  print_hex_nibble(chp, data >> 16);
  print_hex_nibble(chp, data >> 12);
  print_hex_nibble(chp, data >> 8);
  print_hex_nibble(chp, data >> 4);
  print_hex_nibble(chp, data);
}

static void print_serial_output(BaseSequentialStream *chp, SerialDriver *sdp, char dev)
{
  static uint8_t uart_buffer[SERIAL_BUFFERS_SIZE];
  uint32_t count;

  count = sdAsynchronousRead(sdp, uart_buffer, sizeof(uart_buffer));
  
  if( count == 0 )
    return;

  chprintf(chp, "S%c:", dev);
  for(uint32_t i = 0; i < count; i++ )
  {
    if( isprint(uart_buffer[i]) )
    {
      streamPut(chp, uart_buffer[i]);
    }
    else
    {
      streamPut(chp, '\\');
      switch( uart_buffer[i] )
      {
        case '\0':
          streamPut(chp, '0');
          break;
        case '\n':
          streamPut(chp, 'n');
          break;
        case '\r':
          streamPut(chp, 'r');
          break;
        case '\t':
          streamPut(chp, 't');
          break;
        default:
          streamPut(chp, 'x');
          print_hex8(chp, uart_buffer[i]);
          break;
      }
    }
  }
  streamPut(chp, '\r');
  streamPut(chp, '\n');
}

/* MARIONETTE -> PC */
static void mpipe_serial_thread(void * p)
{
	BaseSequentialStream * chp   = (BaseSequentialStream*)p;
	chRegSetThreadName("mpipe_serial");

  // round robin all data sources
  while(!chThdShouldTerminateX())
  {
    // FIXME this should be done through the fetch serial module
    //print_serial_output(chp, &SD2, '2');
    //print_serial_output(chp, &SD3, '3');
    //print_serial_output(chp, &SD4, '4');
    chThdSleepMilliseconds(1);
  }
  chThdExit(MSG_OK);
}

/* MARIONETTE -> PC */
static void mpipe_adc2_thread(void * p)
{
	BaseSequentialStream * chp   = (BaseSequentialStream*)p;
	chRegSetThreadName("mpipe_adc2");
  adc_sample_set_t *ssp;
  msg_t msg;

  while(!chThdShouldTerminateX())
  {
    if( chMBFetch(&mpipe_adc2_mb, &msg, MS2ST(10)) == MSG_OK )
    {
      chMtxLock(&mpipe_output_mutex);
      ssp = (adc_sample_set_t*)msg;
      streamPut(chp, 'A');
      streamPut(chp, '2');
      streamPut(chp, ':');
      print_hex16(chp, ssp->sequence_number);
      for( int i = 0; i < ADC_SAMPLE_SET_SIZE; i++ )
      {
        print_hex16(chp, ssp->sample[i]);
      }
      streamPut(chp, '\r');
      streamPut(chp, '\n');
      chMtxUnlock(&mpipe_output_mutex);
      fetch_adc_free_sample_set(ssp);
    }
  }
  chThdExit(MSG_OK);
}

/* MARIONETTE -> PC */
static void mpipe_adc3_thread(void * p)
{
	BaseSequentialStream * chp   = (BaseSequentialStream*)p;
	chRegSetThreadName("mpipe_adc3");
  adc_sample_set_t *ssp;
  msg_t msg;

  while(!chThdShouldTerminateX())
  {
    if( chMBFetch(&mpipe_adc3_mb, &msg, MS2ST(10)) == MSG_OK )
    {
      chMtxLock(&mpipe_output_mutex);
      ssp = (adc_sample_set_t*)msg;
      streamPut(chp, 'A');
      streamPut(chp, '2');
      streamPut(chp, ':');
      print_hex16(chp, ssp->sequence_number);
      for( int i = 0; i < ADC_SAMPLE_SET_SIZE; i++ )
      {
        print_hex16(chp, ssp->sample[i]);
      }
      streamPut(chp, '\r');
      streamPut(chp, '\n');
      chMtxUnlock(&mpipe_output_mutex);
      fetch_adc_free_sample_set(ssp);
    }
  }
  chThdExit(MSG_OK);
}

/* MARIONETTE -> PC */
static void mpipe_can_thread(void * p)
{
	BaseSequentialStream * chp   = (BaseSequentialStream*)p;
	chRegSetThreadName("mpipe_can");
  msg_t msg;

  while(!chThdShouldTerminateX())
  {
    if( chMBFetch(&mpipe_can_mb, &msg, MS2ST(10)) == MSG_OK )
    {
      chMtxLock(&mpipe_output_mutex);
      //rxfp = (CANRxFrame*)msg;
      // FIXME TODO
      //
      chMtxUnlock(&mpipe_output_mutex);
      //fetch_can_free_rx_frame(rxfp);
    }
  }
  chThdExit(MSG_OK);
}

/* PC -> MARIONETTE */
static void mpipe_input_thread(void * p)
{
	BaseSequentialStream * chp   = (BaseSequentialStream*)p;
	chRegSetThreadName("mpipe_in");
  
  // process command char
  while(!chThdShouldTerminateX())
  {
    char c = streamGet(chp); // FIXME add timeout here so that we dont stall the thread exiting
    switch(c)
    {
      case '\r': // ignore blank lines or extra newlines
      case '\n':
        break;
      //case 'D': // dac
      //case 'S': // serial
      //case 'C': // can
      default:
        while(!IS_EOL(streamGet(chp)));
        break;
    }
  }
  chThdExit(MSG_OK);
}

void mpipe_start(const mpipe_config_t * cfg)
{
  // start/restart io threads
  if( mpipe_adc2_tp == NULL || chThdTerminatedX(mpipe_adc2_tp))
  {
    mpipe_adc2_tp = chThdCreateStatic(mpipe_adc2_wa, sizeof(mpipe_adc2_wa), NORMALPRIO, mpipe_adc2_thread, (void*)cfg->channel);
  }
  if( mpipe_adc3_tp == NULL || chThdTerminatedX(mpipe_adc3_tp))
  {
    mpipe_adc3_tp = chThdCreateStatic(mpipe_adc3_wa, sizeof(mpipe_adc3_wa), NORMALPRIO, mpipe_adc3_thread, (void*)cfg->channel);
  }
  if( mpipe_serial_tp == NULL || chThdTerminatedX(mpipe_serial_tp))
  {
    mpipe_serial_tp = chThdCreateStatic(mpipe_serial_wa, sizeof(mpipe_serial_wa), NORMALPRIO, mpipe_serial_thread, (void*)cfg->channel);
  }
  if( mpipe_can_tp == NULL || chThdTerminatedX(mpipe_can_tp))
  {
    mpipe_can_tp = chThdCreateStatic(mpipe_can_wa, sizeof(mpipe_can_wa), NORMALPRIO, mpipe_can_thread, (void*)cfg->channel);
  }
  if( mpipe_input_tp == NULL || chThdTerminatedX(mpipe_input_tp))
  {
    mpipe_input_tp = chThdCreateStatic(mpipe_input_wa, sizeof(mpipe_input_wa), NORMALPRIO, mpipe_input_thread, (void*)cfg->channel);
  }
}

void mpipe_stop(void)
{
  // kill all io threads
  if( mpipe_input_tp )
  {
    chThdTerminate(mpipe_input_tp);
    chThdWait(mpipe_input_tp);
    mpipe_input_tp = NULL;
  }

  if( mpipe_adc2_tp )
  {
    chThdTerminate(mpipe_adc2_tp);
    chThdWait(mpipe_adc2_tp);
    mpipe_adc2_tp = NULL;
  }

  if( mpipe_adc3_tp )
  {
    chThdTerminate(mpipe_adc3_tp);
    chThdWait(mpipe_adc3_tp);
    mpipe_adc3_tp = NULL;
  }

  if( mpipe_serial_tp )
  {
    chThdTerminate(mpipe_serial_tp);
    chThdWait(mpipe_serial_tp);
    mpipe_serial_tp = NULL;
  }

  if( mpipe_can_tp )
  {
    chThdTerminate(mpipe_can_tp);
    chThdWait(mpipe_can_tp);
    mpipe_can_tp = NULL;
  }
}

void mpipe_init(void)
{
  chMtxObjectInit(&mpipe_output_mutex);

  chMBObjectInit(&mpipe_adc2_mb, mpipe_adc2_mb_buffer, MPIPE_ADC_MB_SIZE);
  chMBObjectInit(&mpipe_adc3_mb, mpipe_adc3_mb_buffer, MPIPE_ADC_MB_SIZE);
  chMBObjectInit(&mpipe_can_mb, mpipe_can_mb_buffer, MPIPE_CAN_MB_SIZE);
}

