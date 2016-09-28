#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"
#include "util_io.h"

#include "fetch_defs.h"
#include "fetch_mbus.h"
#include "fetch.h"


// list all command function prototypes here 
static bool fetch_mbus_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_mbus_select_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_mbus_detect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_mbus_read_analog_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_mbus_read_eeprom_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_mbus_write_eeprom_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);

static fetch_command_t fetch_mbus_commands[] = {
    { fetch_mbus_help_cmd,          "help",       "Display command info"},
    { fetch_mbus_select_cmd,        "select",     "Select bus to act on"},
    { fetch_mbus_detect_cmd,        "detect",     "Detect eeprom or analog"},
    { fetch_mbus_read_analog_cmd,   "readanalog", "Read analog voltages on mbus pins"},
    { fetch_mbus_read_eeprom_cmd,   "read",       "Read EEPROM data"},
    { fetch_mbus_write_eeprom_cmd,  "write",      "Write EEPROM data"},
    { NULL, NULL, NULL } // null terminate list
  };

#define ADC_SMPR1(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24))
#define ADC_SMPR2(smp) (smp | (smp<<3) | (smp<<6) | (smp<<9) | (smp<<12) | (smp<<15) | (smp<<18) | (smp<<21) | (smp<<24) | (smp<<27))

static ADCConversionGroup adc3_mbus_conv_grp = {
	.circular        = false,
	.num_channels    = 2,
	.end_cb          = NULL,
	.error_cb        = NULL,
	/* HW dependent part.*/
	.cr1             = 0,
	.cr2             = ADC_CR2_SWSTART,
	.smpr1           = ADC_SMPR1(ADC_SAMPLE_480),
	.smpr2           = ADC_SMPR2(ADC_SAMPLE_480),
	.sqr1            = ADC_SQR1_NUM_CH(2),
	.sqr2            = 0,
	.sqr3            = ADC_SQR3_SQ1_N(4) | ADC_SQR3_SQ2_N(8)
};

static I2CConfig i2c1_cfg = {OPMODE_I2C, 100000, STD_DUTY_CYCLE};

typedef enum
{
  MBUS_SEL_NONE = 0,
  MBUS_SEL_GPIO,
  MBUS_SEL_SERIAL,
  MBUS_SEL_ANALOG
} mbus_select_t;

mbus_select_t mbus_select = MBUS_SEL_NONE;

typedef enum 
{
  MBUS_PIN_MODE_FLOAT = 0,
  MBUS_PIN_MODE_SCL_UP_SDA_UP,
  MBUS_PIN_MODE_SCL_DOWN_SDA_DOWN,
  MBUS_PIN_MODE_SCL_UP_SDA_DOWN,
  MBUS_PIN_MODE_SCL_DOWN_SDA_UP,
  MBUS_PIN_MODE_I2C,
  MBUS_PIN_MODE_ANALOG
} mbus_pin_mode_t;

static void fetch_mbus_set_pin_mode(mbus_pin_mode_t mode)
{
  switch(mode)
  {
    case MBUS_PIN_MODE_FLOAT:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT);
      break;
    case MBUS_PIN_MODE_SCL_UP_SDA_UP:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT_PULLUP);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT_PULLUP);
      break;
    case MBUS_PIN_MODE_SCL_DOWN_SDA_DOWN:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT_PULLDOWN);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT_PULLDOWN);
      break;
    case MBUS_PIN_MODE_SCL_UP_SDA_DOWN:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT_PULLUP);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT_PULLDOWN);
      break;
    case MBUS_PIN_MODE_SCL_DOWN_SDA_UP:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT_PULLDOWN);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT_PULLUP);
      break;
    case MBUS_PIN_MODE_I2C:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_ALTERNATE(4) | PAL_STM32_PUPDR_PULLUP);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_ALTERNATE(4) | PAL_STM32_PUPDR_PULLUP);
      break;
    case MBUS_PIN_MODE_ANALOG:
      palSetPadMode(GPIOH, GPIOH_PH7_ADC3_CH4, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOH, GPIOH_PH8_ADC3_CH8, PAL_MODE_INPUT_ANALOG);
      palSetPadMode(GPIOB, GPIOB_PB6_I2C1_SCL, PAL_MODE_INPUT);
      palSetPadMode(GPIOB, GPIOB_PB7_I2C1_SDA, PAL_MODE_INPUT);
      break;
  }
}

static uint8_t fetch_mbus_i2c_scan(void)
{
  //uint8_t address_list[] = {0x50}; // FIXME add i2c eeprom addresses here
  uint8_t rx_buffer;

  // returns i2c address or 0 if none found

  for( uint8_t address = 1; address <= 0x7f; address++)
  {
    switch( i2cMasterReceiveTimeout(&I2CD1, address, &rx_buffer, 1, MS2ST(50)) )
    {
      case MSG_TIMEOUT:
        i2cStart(&I2CD1, &i2c1_cfg);
        break;
      case MSG_RESET:
        i2cStart(&I2CD1, &i2c1_cfg);
        break;
      case MSG_OK:
        return address;
    }
  }

  return 0;
}


static bool fetch_mbus_analog_check(BaseSequentialStream *chp)
{
  // this function checks to see if pins follow weak pullups
  // if they don't then the lines are likely tied to analog voltages

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_SCL_UP_SDA_UP);

  chThdSleepMilliseconds(20);

  if( !palReadPad(GPIOB, GPIOB_PB6_I2C1_SCL) || !palReadPad(GPIOB, GPIOB_PB7_I2C1_SDA) )
  {
    return true;
  }

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_SCL_DOWN_SDA_DOWN);
  
  chThdSleepMilliseconds(20);

  if( palReadPad(GPIOB, GPIOB_PB6_I2C1_SCL) || palReadPad(GPIOB, GPIOB_PB7_I2C1_SDA) )
  {
    return true;
  }

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_SCL_UP_SDA_DOWN);
  
  chThdSleepMilliseconds(20);

  if( !palReadPad(GPIOB, GPIOB_PB6_I2C1_SCL) || palReadPad(GPIOB, GPIOB_PB7_I2C1_SDA) )
  {
    return true;
  }

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_SCL_DOWN_SDA_UP);
  
  chThdSleepMilliseconds(20);

  if( palReadPad(GPIOB, GPIOB_PB6_I2C1_SCL) || !palReadPad(GPIOB, GPIOB_PB7_I2C1_SDA) )
  {
    return true;
  }

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_FLOAT);

  return false;
}


static bool fetch_mbus_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  fetch_display_help_legend(chp);
  fetch_display_help(chp, fetch_mbus_commands);
	return true;
}

static bool fetch_mbus_select_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 1, 1);

  palClearPad(GPIOI, GPIOI_PI7_MBUS_GPIO_SELECT);
  palClearPad(GPIOI, GPIOI_PI6_MBUS_SER_SELECT);
  palClearPad(GPIOI, GPIOI_PI5_MBUS_ANA_SELECT);

  if( strcasecmp(data_list[0], "gpio") == 0 )
  {
    mbus_select = MBUS_SEL_GPIO;
    palSetPad(GPIOI,GPIOI_PI7_MBUS_GPIO_SELECT);
  }
  else if( strcasecmp(data_list[0], "serial") == 0 )
  {
    mbus_select = MBUS_SEL_SERIAL;
    palSetPad(GPIOI,GPIOI_PI6_MBUS_SER_SELECT);
  }
  else if( strcasecmp(data_list[0], "analog") == 0 )
  {
    mbus_select = MBUS_SEL_ANALOG;
    palSetPad(GPIOI,GPIOI_PI5_MBUS_ANA_SELECT);
  }
  else if( strcasecmp(data_list[0], "none") == 0 )
  {
    mbus_select = MBUS_SEL_NONE;
  }
  else
  {
    util_message_error(chp, "Invalid mbus port name");
  }

	return true;
}

static bool fetch_mbus_detect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  adcsample_t mbus_samples[2];

  if( fetch_mbus_analog_check(chp) )
  {
    util_message_info(chp, "Strong pull up/down detected, likely analog.");
  }
  //else
  //{
    // check for i2c eeprom
    fetch_mbus_set_pin_mode(MBUS_PIN_MODE_I2C);

    uint8_t address = fetch_mbus_i2c_scan();

    if( address > 0 )
    {
      util_message_info(chp, "I2C device found");
    }
    else
    {
      util_message_info(chp, "No I2C device found");
    }
    util_message_uint8(chp, "address", address);
  //}

	return true;
}

static bool fetch_mbus_read_analog_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);
  adcsample_t mbus_samples[2];

  // sample analog voltages
  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_ANALOG);

  // FIXME change this to use adcAquire etc?
  if( ADCD3.state != ADC_READY )
  {
    util_message_error(chp, "ADC3 not in ready state");
    return false;
  }

  adcConvert( &ADCD3, &adc3_mbus_conv_grp, mbus_samples, 2);
  util_message_uint16_array(chp, "values", mbus_samples, 2);

  return true;
}


static bool fetch_mbus_read_eeprom_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  fetch_mbus_set_pin_mode(MBUS_PIN_MODE_I2C);
  
  uint8_t address = 0x50;//fetch_mbus_i2c_scan();

  if( address == 0 )
  {
    util_message_error(chp, "No eeprom found");
    return false;
  }

  static uint8_t rx_buffer[256];
  uint8_t mem_address = 0;
  
  switch( i2cMasterReceiveTimeout(&I2CD1, address, rx_buffer, sizeof(rx_buffer), MS2ST(50)) )
  {
    case MSG_TIMEOUT:
      util_message_error(chp, "i2c timeout");
      i2cStart(&I2CD1, &i2c1_cfg);
      return false;
    case MSG_RESET:
      util_message_error(chp, "i2c error");
      util_message_hex_uint32(chp, "error_flags", i2cGetErrors(&I2CD1));
      i2cStart(&I2CD1, &i2c1_cfg);
      return false;
    case MSG_OK:
    default:
      util_message_hex_uint8_array(chp,"data", rx_buffer, sizeof(rx_buffer));
      return true;
  }
}

static bool fetch_mbus_write_eeprom_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  // TODO

	return true;
}

void fetch_mbus_init(BaseSequentialStream * chp)
{
  static bool mbus_init_flag = false;

  if( mbus_init_flag )
    return;

  adcStart(&ADCD3, NULL);
  
  i2cStart(&I2CD1, &i2c1_cfg);

  mbus_select = MBUS_SEL_NONE;

  palClearPad(GPIOI, GPIOI_PI7_MBUS_GPIO_SELECT);
  palClearPad(GPIOI, GPIOI_PI6_MBUS_SER_SELECT);
  palClearPad(GPIOI, GPIOI_PI5_MBUS_ANA_SELECT);

  mbus_init_flag = true;
}

/*! \brief dispatch a specific command
 */
bool fetch_mbus_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_mbus_commands, cmd_list, data_list);
}

bool fetch_mbus_reset(BaseSequentialStream * chp)
{
  // TODO
  return true;
}

/*! @} */

