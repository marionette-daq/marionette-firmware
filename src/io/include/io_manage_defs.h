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
  IO_LED         = 0b1000000,
  IO_CAN         = 0b10000000
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

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static io_table_t io_porta =
{
	.port = GPIOA,
	.pins =
	{
		{.pin = GPIOA_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{.pin = GPIOA_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{.pin = GPIOA_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{.pin = GPIOA_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{.pin = GPIOA_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC | IO_DAC } ,
		{.pin = GPIOA_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC | IO_DAC | IO_SPI } ,
		{.pin = GPIOA_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC | IO_SPI } ,
		{.pin = GPIOA_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC | IO_SPI } ,
		{.pin = GPIOA_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
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
		{ .pin = GPIOB_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOB_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_SWO,         .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_NJTRST,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOB_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOB_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOB_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_portc =
{
	.port = GPIOC,
	.pins =
	{
		{ .pin = GPIOC_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOC_OTG_PWR_OUT, .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_USB,   .available_alloc = IO_GPIO | IO_ADC | IO_USB } , // REMOVE JUMPER TO USE
		{ .pin = GPIOC_OTG_FLG,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_USB,   .available_alloc = IO_GPIO | IO_ADC | IO_USB } , // REMOVE JUMPER TO USE
		{ .pin = GPIOC_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOC_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOC_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOC_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOC_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOC_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOC_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOC_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOC_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOC_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOC_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
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
		{ .pin = GPIOD_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOD_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
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
		{ .pin = GPIOF_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOF_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
		{ .pin = GPIOF_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_ADC } ,
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
		{ .pin = GPIOG_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOG_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO }
	}
};

static io_table_t io_porth =
{
	.port = GPIOH,
	.pins =
	{
		{ .pin = GPIOH_OSC_IN,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOH_OSC_OUT,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_NONE } ,
		{ .pin = GPIOH_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_LED,  .current_alloc = IO_LED,   .available_alloc = IO_LED } ,
		{ .pin = GPIOH_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_LED,  .current_alloc = IO_LED,   .available_alloc = IO_LED } ,
		{ .pin = GPIOH_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOH_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
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
		{ .pin = GPIOI_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOI_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOI_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO | IO_SPI } ,
		{ .pin = GPIOI_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_LED,  .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
		{ .pin = GPIOI_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_LED,  .current_alloc = IO_NONE,  .available_alloc = IO_GPIO } ,
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

