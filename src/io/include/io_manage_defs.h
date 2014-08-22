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

typedef enum io_alloc
{
	IO_NONE        = 0b0,   // possibly a hardwired connection
	IO_GPIO        = 0b1,
	IO_ADC         = 0b10,
	IO_DAC         = 0b100,
	IO_SPI         = 0b1000,
	IO_I2C         = 0b10000,
	IO_USB         = 0b100000
} IO_alloc;

typedef struct io_namestr
{
	IO_alloc   alloc;
	const char   *   name;
} IO_namestr;

typedef struct io_state
{
	const uint32_t   pad;
	const iomode_t   default_mode;
	iomode_t         current_mode;
	const IO_alloc   default_alloc;
	IO_alloc         current_alloc;
	const uint16_t   fn_available;
} IO_state;

typedef struct io_table
{
	const ioportid_t port;
	IO_state         pin[16];
} IO_table;

#if defined(BOARD_WAVESHARE_CORE407I) || defined(__DOXYGEN__)
static IO_table io_porta =
{
	.port = GPIOA,
	.pin =
	{
		{.pad = GPIOA_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC | IO_DAC } ,
		{.pad = GPIOA_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC | IO_DAC } ,
		{.pad = GPIOA_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{.pad = GPIOA_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{.pad = GPIOA_OTG_FS_VBUS, .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .fn_available = IO_NONE } ,
		{.pad = GPIOA_OTG_FS_ID,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .fn_available = IO_NONE } ,
		{.pad = GPIOA_OTG_FS_DM,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .fn_available = IO_NONE } ,
		{.pad = GPIOA_OTG_FS_DP,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_USB,  .current_alloc = IO_USB,   .fn_available = IO_NONE } ,
		{.pad = GPIOA_SWDIO,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{.pad = GPIOA_SWCLK,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{.pad = GPIOA_JTDI,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE }
	}
};

static IO_table io_portb =
{
	.port = GPIOB,
	.pin =
	{
		{ .pad = GPIOB_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{ .pad = GPIOB_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_SWO,         .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOB_NJTRST,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOB_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOB_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOB_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_portc =
{
	.port = GPIOC,
	.pin =
	{
		{ .pad = GPIOC_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{ .pad = GPIOC_OTG_PWR_OUT, .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOC_OTG_FLG,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOC_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{ .pad = GPIOC_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{ .pad = GPIOC_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO | IO_ADC } ,
		{ .pad = GPIOC_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOC_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOC_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOC_OSC32_IN,    .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOC_OSC32_OUT,   .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE }
	}
};

static IO_table io_portd =
{
	.port = GPIOD,
	.pin =
	{
		{ .pad = GPIOD_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOD_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_porte =
{
	.port = GPIOE,
	.pin =
	{
		{ .pad = GPIOE_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOE_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_portf =
{
	.port = GPIOF,
	.pin =
	{
		{ .pad = GPIOF_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOF_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_portg =
{
	.port = GPIOG,
	.pin =
	{

		{ .pad = GPIOG_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOG_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_porth =
{
	.port = GPIOH,
	.pin =
	{
		{ .pad = GPIOH_OSC_IN,      .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOH_OSC_OUT,     .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_NONE } ,
		{ .pad = GPIOH_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOH_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
	}
};

static IO_table io_porti =
{
	.port = GPIOI,
	.pin =
	{
		{ .pad = GPIOI_PIN0,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN1,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN2,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN3,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN4,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN5,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN6,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN7,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN8,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN9,        .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN10,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN11,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN12,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN13,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN14,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO } ,
		{ .pad = GPIOI_PIN15,       .default_mode = PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .current_mode = PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING, .default_alloc = IO_NONE, .current_alloc = IO_NONE,  .fn_available = IO_GPIO }
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

