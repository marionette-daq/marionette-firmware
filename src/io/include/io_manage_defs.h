/*! \file io_manaage_defs.h
 *
 * Definitions for io_manage module
 *
 * @defgroup io_manaage_defs IO Management Tables
 * @{
 */

#ifndef IO_MANAGE_DEFS_H_
#define IO_MANAGE_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PIN_STRLEN  8
#define MAX_PORT_STRLEN 8

typedef enum io_pin {
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	INVALID_PIN=99
} iopin_t;

typedef struct str_pin_map
{
	const char      * string;
	const uint32_t    pin;
} str_pin_map_t;

typedef struct str_port_map
{
	const char      * string;
	const ioportid_t  port;
} str_port_map_t;

typedef struct port_pin
{
  const ioportid_t  port;
  const uint32_t    pin;
} port_pin_t;

typedef enum io_alloc
{
	IO_NONE        = 0b0,   // possibly a hardwired connection
	IO_GPIO        = 0b1,
	IO_ADC         = 0b10,
	IO_DAC         = 0b100,
	IO_SPI         = 0b1000,
	IO_I2C         = 0b10000,
	IO_USB         = 0b100000,
  IO_CAN         = 0b1000000,
  IO_UART        = 0b10000000,
  IO_SERIAL      = 0b100000000
} io_alloc_t;

typedef struct io_namestr
{
	io_alloc_t       alloc;
	const char     * name;
} io_namestr_t;

typedef struct io_state
{
	const uint32_t   pin;
	const iomode_t   default_mode;
	iomode_t         current_mode;
	const io_alloc_t default_alloc;
	io_alloc_t       current_alloc;
	const uint32_t   available_alloc;
} io_state_t;

typedef struct io_table
{
	const ioportid_t port;
	io_state_t       pins[16];
} io_table_t;

/****************************************/
/** Pin Mappings for each target board **/
/****************************************/

#if defined(BOARD_MARIONETTE) || defined(__DOXYGEN__)

// Peripheral specific port assignments

static port_pin_t adc1_pins[]   = { { GPIOA, PIN2 },    // ADC_IN2
                                    { GPIOA, PIN6 },    // ADC_IN6
                                    { GPIOA, PIN7 },    // ADC_IN7
                                    { GPIOC, PIN1 },    // ADC_IN11
                                    { GPIOC, PIN3 },    // ADC_IN13
                                    { GPIOC, PIN4 },    // ADC_IN14
                                    { GPIOC, PIN5 } };  // ADC_IN15

static port_pin_t adc2_pins[]   = { { GPIOA, PIN2 },    // ADC_IN2
                                    { GPIOA, PIN6 },    // ADC_IN6
                                    { GPIOA, PIN7 },    // ADC_IN7
                                    { GPIOC, PIN1 },    // ADC_IN11
                                    { GPIOC, PIN3 },    // ADC_IN13
                                    { GPIOC, PIN4 },    // ADC_IN14
                                    { GPIOC, PIN5 } };  // ADC_IN15

static port_pin_t adc3_pins[]   = { { GPIOA, PIN2 },    // ADC_IN2
                                    { GPIOF, PIN6 },    // ADC_IN4
                                    { GPIOF, PIN7 },    // ADC_IN5
                                    { GPIOF, PIN8 },    // ADC_IN6
                                    { GPIOF, PIN9 },    // ADC_IN7
                                    { GPIOF, PIN10 },   // ADC_IN8
                                    { GPIOF, PIN3 },    // ADC_IN9
                                    { GPIOC, PIN1 },    // ADC_IN11
                                    { GPIOC, PIN3 },    // ADC_IN13
                                    { GPIOF, PIN4 },    // ADC_IN14
                                    { GPIOF, PIN5 } };  // ADC_IN15

static port_pin_t dac_pins[]    = { { GPIOA, PIN4 }};    // DAC_CH0


static port_pin_t spi2_pins[]   = { { GPIOI, PIN1 },    // SPI_SCK
                                    { GPIOI, PIN2 },    // SPI_MISO
                                    { GPIOI, PIN3 } };  // SPI_MOSI*/


static port_pin_t spi3_pins[]   = { { GPIOC, PIN10 },   // SPI_SCK
                                    { GPIOC, PIN11 },   // SPI_MISO
                                    { GPIOC, PIN12 } }; // SPI_MOSI

static port_pin_t i2c1_pins[]   = { { GPIOB, PIN6 },    // I2C_SCL
                                    { GPIOB, PIN7 } };  // I2C_SDA

static port_pin_t i2c2_pins[]   = { { GPIOF, PIN1 },   // I2C_SCL
                                    { GPIOF, PIN0 } }; // I2C_SDA

static port_pin_t i2c3_pins[]   = { { GPIOH, PIN7 },    // I2C_SCL
                                    { GPIOH, PIN8 } };  // I2C_SDA

static port_pin_t can1_pins[]   = { { GPIOH, PIN13 },    // CAN_TX  Default for Open C breakout board, need to change for marionette
	                            { GPIOI, PIN9 }};     // CAN_RX

static port_pin_t uart4_pins[]  = { { GPIOA, PIN0 },   //UART_TX
				    { GPIOA, PIN1 }};  //UART_RX
// Port/Pin configuration matrix

static io_table_t io_porta =
{
	.port = GPIOA,
	.pins =
	{
		{.pin = GPIOA_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_UART } ,
		{.pin = GPIOA_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_UART } ,
		{.pin = GPIOA_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{.pin = GPIOA_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB, .current_alloc = IO_USB,  .available_alloc = IO_NONE } ,
		{.pin = GPIOA_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_DAC } ,
		{.pin = GPIOA_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB, .current_alloc = IO_USB,  .available_alloc = IO_NONE } ,
		{.pin = GPIOA_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{.pin = GPIOA_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{.pin = GPIOA_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{.pin = GPIOA_OTG_FS_VBUS, .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .available_alloc = IO_NONE } ,
		{.pin = GPIOA_OTG_FS_ID,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .available_alloc = IO_NONE } ,
		{.pin = GPIOA_OTG_FS_DM,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .available_alloc = IO_NONE } ,
		{.pin = GPIOA_OTG_FS_DP,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .available_alloc = IO_NONE } ,
		{.pin = GPIOA_SWDIO,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{.pin = GPIOA_SWCLK,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{.pin = GPIOA_JTDI,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE }
	}
};

static io_table_t io_portb =
{
	.port = GPIOB,
	.pins =
	{
		{ .pin = GPIOB_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_GPIO, .current_alloc = IO_GPIO,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_SWO,         .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_NJTRST,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_I2C } ,
		{ .pin = GPIOB_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_I2C } ,
		{ .pin = GPIOB_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO  } ,
		{ .pin = GPIOB_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_portc =
{
	.port = GPIOC,
	.pins =
	{
		{ .pin = GPIOC_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE} ,
		{ .pin = GPIOC_OTG_PWR_OUT, .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,   .available_alloc =  IO_ADC  } ,
		{ .pin = GPIOC_OTG_FLG,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB, .current_alloc = IO_USB,   .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOC_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOC_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOC_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOC_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI } ,
		{ .pin = GPIOC_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI} ,
		{ .pin = GPIOC_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI } ,
		{ .pin = GPIOC_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_OSC32_IN,    .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOC_OSC32_OUT,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE }
	}
};

static io_table_t io_portd =
{
	.port = GPIOD,
	.pins =
	{
		{ .pin = GPIOD_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOD_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOD_LED2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_LED1_RED,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_LED1_GREEN,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_LED1_BLUE,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_porte =
{
	.port = GPIOE,
	.pins =
	{
		{ .pin = GPIOE_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOE_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_portf =
{
	.port = GPIOF,
	.pins =
	{
		{ .pin = GPIOF_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_I2C } ,
		{ .pin = GPIOF_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_I2C } ,
		{ .pin = GPIOF_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOF_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_ADC } ,
		{ .pin = GPIOF_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_portg =
{
	.port = GPIOG,
	.pins =
	{

		{ .pin = GPIOG_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED4,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED5,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED6,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED7,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED8,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_LED9,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_porth =
{
	.port = GPIOH,
	.pins =
	{
		{ .pin = GPIOH_OSC_IN,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOH_OSC_OUT,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOH_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB, .current_alloc = IO_USB,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOH_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_I2C } ,
		{ .pin = GPIOH_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_I2C } ,
		{ .pin = GPIOH_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_CAN} ,
		{ .pin = GPIOH_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_porti =
{
	.port = GPIOI,
	.pins =
	{
		{ .pin = GPIOI_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI } ,
		{ .pin = GPIOI_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI } ,
		{ .pin = GPIOI_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_SPI } ,
		{ .pin = GPIOI_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc =  IO_CAN} ,
		{ .pin = GPIOI_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOI_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

#elif defined (BOARD_ST_STM32F4_DISCOVERY)
#error "ST Discovery Board not defined for Marionette"
#else
#error "Board not defined for ADC"
#endif


#ifdef __cplusplus
}
#endif

#endif

//! @}

