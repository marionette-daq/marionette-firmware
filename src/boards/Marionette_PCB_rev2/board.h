
#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Board identifier.
 */
#define BOARD_MARIONETTE_PCB_REV2
#define BOARD_NAME                  "Marionette PCB rev2"


/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                26000000
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   330

/* Board uses ULPI phy */
#define BOARD_OTG2_USES_ULPI

/*
 * MCU type as defined in the ST header.
 */
#define STM32F427xx
#define STM32F427_437xx

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_2M(n)            (0U << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1U << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2U << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3U << ((n) * 2))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v##U) << ((n % 8) * 4))

/*
 * IO pins assignements.
 */

#define GPIOA_UART4_TX                 0 
#define GPIOA_UART4_RX                 1 
#define GPIOA_ADC123_CH2               2 
#define GPIOA_ULPI_D0                  3 
#define GPIOA_STM_DAC                  4 
#define GPIOA_ULPI_CK                  5 
#define GPIOA_ADC12_CH6                6 
#define GPIOA_ADC12_CH7                7 
#define GPIOA_MCO1                     8 //26Mhz to ULPI phy
#define GPIOA_USB_FS_VUSB              9 
#define GPIOA_SDIO_PWR                 10 //active low
#define GPIOA_USB_FS_DM                11 
#define GPIOA_USB_FS_DP                12 
#define GPIOA_SWDIO                    13 
#define GPIOA_SWCLK                    14 
#define GPIOA_PIN15                    15 

#define GPIOB_ULPI_D1                  0 
#define GPIOB_ULPI_D2                  1 
#define GPIOB_BOOT1                    2 
#define GPIOB_SWO                      3 
#define GPIOB_ULPI_NRST                4 
#define GPIOB_ULPI_D7                  5 
#define GPIOB_I2C1_SCL_MBUS            6 //external pullups
#define GPIOB_I2C1_SDA_MBUS            7 //external pullups
#define GPIOB_PIN8                     8 
#define GPIOB_PIN9                     9 
#define GPIOB_ULPI_D3                  10 
#define GPIOB_ULPI_D4                  11 
#define GPIOB_ULPI_D5                  12 
#define GPIOB_ULPI_D6                  13 
#define GPIOB_PIN14                    14 
#define GPIOB_PIN15                    15 

#define GPIOC_ULPI_STP                 0 
#define GPIOC_ADC123_CH11              1 
#define GPIOC_ULPI_DIR                 2 
#define GPIOC_ADC123_CH13              3 
#define GPIOC_ADC12_CH14               4 
#define GPIOC_ADC12_CH15               5 
#define GPIOC_PIN6                     6 //TP18, USB_PHY_REFCLK_12MHZ?
#define GPIOC_SD_DETECT                7 //switch on sd card socket
#define GPIOC_SDIO_D0                  8 
#define GPIOC_SDIO_D1                  9 
#define GPIOC_SDIO_D2                  10 
#define GPIOC_SDIO_D3                  11 
#define GPIOC_SDIO_CLK                 12 
#define GPIOC_PIN13                    13 
#define GPIOC_OSC32_IN                 14 
#define GPIOC_OSC32_OUT                15 

#define GPIOD_PIN0                     0 
#define GPIOD_PIN1                     1 
#define GPIOD_SDIO_CMD                 2 
#define GPIOD_PIN3                     3 
#define GPIOD_PIN4                     4 
#define GPIOD_PIN5                     5 
#define GPIOD_PIN6                     6 
#define GPIOD_PIN7                     7
#define GPIOD_PIN8                     8 
#define GPIOD_PIN9                     9 
#define GPIOD_PIN10                    10 
#define GPIOD_PIN11                    11 
#define GPIOD_PIN12                    12 
#define GPIOD_LED_STATUS_R             13 //active low
#define GPIOD_LED_STATUS_G             14 //active low
#define GPIOD_LED_STATUS_B             15 //active low

#define GPIOE_PIN0                     0 
#define GPIOE_PIN1                     1 
#define GPIOE_PIN2                     2 
#define GPIOE_PIN3                     3 
#define GPIOE_PIN4                     4 
#define GPIOE_PIN5                     5 
#define GPIOE_PIN6                     6 
#define GPIOE_PIN7                     7 
#define GPIOE_PIN8                     8 
#define GPIOE_PIN9                     9 
#define GPIOE_PIN10                    10 
#define GPIOE_SPI4_NSS                 11 // EXT DAC !SYNC
#define GPIOE_SPI4_SCK                 12 // EXT DAC
#define GPIOE_PIN13                13 // NC
#define GPIOE_SPI4_MOSI                14 // EXT DAC
#define GPIOE_PIN15                    15 

#define GPIOF_I2C2_SDA                 0 //external pullup
#define GPIOF_I2C2_SCL                 1 //external pullup
#define GPIOF_CAN_SHDN                 2 //active high
#define GPIOF_ADC3_CH9                 3 
#define GPIOF_ADC3_CH14                4 
#define GPIOF_ADC3_CH15                5 
#define GPIOF_ADC3_CH4                 6 
#define GPIOF_ADC3_CH5                 7 
#define GPIOF_ADC3_CH6                 8 
#define GPIOF_ADC3_CH7                 9 
#define GPIOF_ADC3_CH8                 10 
#define GPIOF_PIN11                    11 
#define GPIOF_PIN12                    12 
#define GPIOF_PIN13                    13 
#define GPIOF_PIN14                    14 
#define GPIOF_PIN15                    15 

#define GPIOG_PIN0                     0 
#define GPIOG_PIN1                     1 
#define GPIOG_PIN2                     2 
#define GPIOG_PIN3                     3 
#define GPIOG_PIN4                     4 
#define GPIOG_PIN5                     5 
#define GPIOG_PIN6                     6 
#define GPIOG_PIN7                     7 
#define GPIOG_PIN8                     8
#define GPIOG_LED_STATUS_1             9 //active low
#define GPIOG_LED_STATUS_2             10 //active low
#define GPIOG_LED_STATUS_3             11 //active low
#define GPIOG_SPI6_MISO                12
#define GPIOG_SPI6_SCK                 13
#define GPIOG_SPI6_MOSI                14
#define GPIOG_LED_STATUS_7             15 //active low

#define GPIOH_OSC_IN                   0 
#define GPIOH_OSC_OUT                  1 
#define GPIOH_PIN2                     2 
#define GPIOH_PIN3                     3 
#define GPIOH_ULPI_NXT                 4 
#define GPIOH_PIN5                     5 
#define GPIOH_PIN6                     6 
#define GPIOH_ADC3_CH4_MBUS            7 //MBUS, I2C/Analog
#define GPIOH_ADC3_CH8_MBUS            8 //MBUS, I2C/Analog
#define GPIOH_PIN9                     9 
#define GPIOH_PIN10                    10 
#define GPIOH_PIN11                    11 
#define GPIOH_PIN12                    12 
#define GPIOH_CAN1_TX                  13 
#define GPIOH_PIN14                    14 
#define GPIOH_PIN15                    15 

#define GPIOI_PIN0                     0 
#define GPIOI_SPI2_SCK                 1 
#define GPIOI_SPI2_MISO                2 
#define GPIOI_SPI2_MOSI                3 
#define GPIOI_PIN4                     4 
#define GPIOI_ANALOG_BUS_ID            5 //active low
#define GPIOI_SERIAL_BUS_ID            6 //active low
#define GPIOI_GPIO_BUS_ID              7 //active low
#define GPIOI_PIN8                     8 
#define GPIOI_CAN1_RX                  9 
#define GPIOI_PIN10                    10 
#define GPIOI_PIN11                    11 
#define GPIOI_PIN12                    12 
#define GPIOI_PIN13                    13 
#define GPIOI_PIN14                    14 
#define GPIOI_PIN15                    15 

/*
 * GPIOA setup:
 *
*/
#define VAL_GPIOA_MODER     (PIN_MODE_ALTERNATE(GPIOA_UART4_TX) | \
                             PIN_MODE_ALTERNATE(GPIOA_UART4_RX) | \
                             PIN_MODE_ANALOG(GPIOA_ADC123_CH2) | \
                             PIN_MODE_ALTERNATE(GPIOA_ULPI_D0) | \
                             PIN_MODE_ANALOG(GPIOA_STM_DAC) | \
                             PIN_MODE_ALTERNATE(GPIOA_ULPI_CK) | \
                             PIN_MODE_ANALOG(GPIOA_ADC12_CH6) | \
                             PIN_MODE_ANALOG(GPIOA_ADC12_CH7) | \
                             PIN_MODE_ALTERNATE(GPIOA_MCO1) | \
                             PIN_MODE_INPUT(GPIOA_USB_FS_VUSB) | \
                             PIN_MODE_OUTPUT(GPIOA_SDIO_PWR) | \
                             PIN_MODE_ALTERNATE(GPIOA_USB_FS_DM) | \
                             PIN_MODE_ALTERNATE(GPIOA_USB_FS_DP) | \
                             PIN_MODE_ALTERNATE(GPIOA_SWDIO) | \
                             PIN_MODE_ALTERNATE(GPIOA_SWCLK) | \
                             PIN_MODE_INPUT(GPIOA_PIN15))
#define VAL_GPIOA_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOA_UART4_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_UART4_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC123_CH2) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ULPI_D0) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_STM_DAC) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ULPI_CK) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC12_CH6) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC12_CH7) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_MCO1) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_USB_FS_VUSB) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_SDIO_PWR) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_USB_FS_DM) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_USB_FS_DP) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PIN15))
#define VAL_GPIOA_OSPEEDR   (PIN_OSPEED_100M(GPIOA_UART4_TX) | \
                             PIN_OSPEED_100M(GPIOA_UART4_RX) | \
                             PIN_OSPEED_100M(GPIOA_ADC123_CH2) | \
                             PIN_OSPEED_100M(GPIOA_ULPI_D0) | \
                             PIN_OSPEED_100M(GPIOA_STM_DAC) | \
                             PIN_OSPEED_100M(GPIOA_ULPI_CK) | \
                             PIN_OSPEED_100M(GPIOA_ADC12_CH6) | \
                             PIN_OSPEED_100M(GPIOA_ADC12_CH7) | \
                             PIN_OSPEED_100M(GPIOA_MCO1) | \
                             PIN_OSPEED_100M(GPIOA_USB_FS_VUSB) | \
                             PIN_OSPEED_100M(GPIOA_SDIO_PWR) | \
                             PIN_OSPEED_100M(GPIOA_USB_FS_DM) | \
                             PIN_OSPEED_100M(GPIOA_USB_FS_DP) | \
                             PIN_OSPEED_100M(GPIOA_SWDIO) | \
                             PIN_OSPEED_100M(GPIOA_SWCLK) | \
                             PIN_OSPEED_100M(GPIOA_PIN15))
#define VAL_GPIOA_PUPDR     (PIN_PUPDR_FLOATING(GPIOA_UART4_TX) | \
                             PIN_PUPDR_PULLUP(GPIOA_UART4_RX) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC123_CH2) | \
                             PIN_PUPDR_FLOATING(GPIOA_ULPI_D0) | \
                             PIN_PUPDR_FLOATING(GPIOA_STM_DAC) | \
                             PIN_PUPDR_FLOATING(GPIOA_ULPI_CK) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC12_CH6) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC12_CH7) | \
                             PIN_PUPDR_FLOATING(GPIOA_MCO1) | \
                             PIN_PUPDR_FLOATING(GPIOA_USB_FS_VUSB) | \
                             PIN_PUPDR_FLOATING(GPIOA_SDIO_PWR) | \
                             PIN_PUPDR_FLOATING(GPIOA_USB_FS_DM) | \
                             PIN_PUPDR_FLOATING(GPIOA_USB_FS_DP) | \
                             PIN_PUPDR_FLOATING(GPIOA_SWDIO) | \
                             PIN_PUPDR_FLOATING(GPIOA_SWCLK) | \
                             PIN_PUPDR_FLOATING(GPIOA_PIN15))
#define VAL_GPIOA_ODR       (PIN_ODR_HIGH(GPIOA_UART4_TX) | \
                             PIN_ODR_HIGH(GPIOA_UART4_RX) | \
                             PIN_ODR_HIGH(GPIOA_ADC123_CH2) | \
                             PIN_ODR_HIGH(GPIOA_ULPI_D0) | \
                             PIN_ODR_HIGH(GPIOA_STM_DAC) | \
                             PIN_ODR_HIGH(GPIOA_ULPI_CK) | \
                             PIN_ODR_HIGH(GPIOA_ADC12_CH6) | \
                             PIN_ODR_HIGH(GPIOA_ADC12_CH7) | \
                             PIN_ODR_HIGH(GPIOA_MCO1) | \
                             PIN_ODR_HIGH(GPIOA_USB_FS_VUSB) | \
                             PIN_ODR_HIGH(GPIOA_SDIO_PWR) | \
                             PIN_ODR_HIGH(GPIOA_USB_FS_DM) | \
                             PIN_ODR_HIGH(GPIOA_USB_FS_DP) | \
                             PIN_ODR_HIGH(GPIOA_SWDIO) | \
                             PIN_ODR_HIGH(GPIOA_SWCLK) | \
                             PIN_ODR_HIGH(GPIOA_PIN15))
#define VAL_GPIOA_AFRL      (PIN_AFIO_AF(GPIOA_UART4_TX, 8) | \
                             PIN_AFIO_AF(GPIOA_UART4_RX, 8) | \
                             PIN_AFIO_AF(GPIOA_ADC123_CH2, 0) | \
                             PIN_AFIO_AF(GPIOA_ULPI_D0, 10) | \
                             PIN_AFIO_AF(GPIOA_STM_DAC, 0) | \
                             PIN_AFIO_AF(GPIOA_ULPI_CK, 10) | \
                             PIN_AFIO_AF(GPIOA_ADC12_CH6, 0) | \
                             PIN_AFIO_AF(GPIOA_ADC12_CH7, 0))
#define VAL_GPIOA_AFRH      (PIN_AFIO_AF(GPIOA_MCO1, 0) | \
                             PIN_AFIO_AF(GPIOA_USB_FS_VUSB, 0) | \
                             PIN_AFIO_AF(GPIOA_SDIO_PWR, 0) | \
                             PIN_AFIO_AF(GPIOA_USB_FS_DM, 10) | \
                             PIN_AFIO_AF(GPIOA_USB_FS_DP, 10) | \
                             PIN_AFIO_AF(GPIOA_SWDIO, 0) | \
                             PIN_AFIO_AF(GPIOA_SWCLK, 0) | \
                             PIN_AFIO_AF(GPIOA_PIN15, 0))
/*
 * GPIOB setup:
 *
*/
#define VAL_GPIOB_MODER     (PIN_MODE_ALTERNATE(GPIOB_ULPI_D1) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D2) | \
                             PIN_MODE_INPUT(GPIOB_BOOT1) | \
                             PIN_MODE_ALTERNATE(GPIOB_SWO) | \
                             PIN_MODE_OUTPUT(GPIOB_ULPI_NRST) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D7) | \
                             PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL_MBUS) | \
                             PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA_MBUS) | \
                             PIN_MODE_INPUT(GPIOB_PIN8) | \
                             PIN_MODE_INPUT(GPIOB_PIN9) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D3) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D4) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D5) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D6) | \
                             PIN_MODE_INPUT(GPIOB_PIN14) | \
                             PIN_MODE_INPUT(GPIOB_PIN15))
#define VAL_GPIOB_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D1) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D2) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_SWO) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_NRST) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D7) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_I2C1_SCL_MBUS) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_I2C1_SDA_MBUS) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PIN8) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PIN9) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D3) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D4) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D5) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D6) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PIN14) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PIN15))
#define VAL_GPIOB_OSPEEDR   (PIN_OSPEED_100M(GPIOB_ULPI_D1) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D2) | \
                             PIN_OSPEED_100M(GPIOB_BOOT1) | \
                             PIN_OSPEED_100M(GPIOB_SWO) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_NRST) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D7) | \
                             PIN_OSPEED_100M(GPIOB_I2C1_SCL_MBUS) | \
                             PIN_OSPEED_100M(GPIOB_I2C1_SDA_MBUS) | \
                             PIN_OSPEED_100M(GPIOB_PIN8) | \
                             PIN_OSPEED_100M(GPIOB_PIN9) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D3) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D4) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D5) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D6) | \
                             PIN_OSPEED_100M(GPIOB_PIN14) | \
                             PIN_OSPEED_100M(GPIOB_PIN15))
#define VAL_GPIOB_PUPDR     (PIN_PUPDR_FLOATING(GPIOB_ULPI_D1) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D2) | \
                             PIN_PUPDR_FLOATING(GPIOB_BOOT1) | \
                             PIN_PUPDR_FLOATING(GPIOB_SWO) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_NRST) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D7) | \
                             PIN_PUPDR_FLOATING(GPIOB_I2C1_SCL_MBUS) | \
                             PIN_PUPDR_FLOATING(GPIOB_I2C1_SDA_MBUS) | \
                             PIN_PUPDR_FLOATING(GPIOB_PIN8) | \
                             PIN_PUPDR_FLOATING(GPIOB_PIN9) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D3) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D4) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D5) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D6) | \
                             PIN_PUPDR_FLOATING(GPIOB_PIN14) | \
                             PIN_PUPDR_FLOATING(GPIOB_PIN15))
#define VAL_GPIOB_ODR       (PIN_ODR_HIGH(GPIOB_ULPI_D1) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D2) | \
                             PIN_ODR_HIGH(GPIOB_BOOT1) | \
                             PIN_ODR_HIGH(GPIOB_SWO) | \
                             PIN_ODR_LOW(GPIOB_ULPI_NRST) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D7) | \
                             PIN_ODR_HIGH(GPIOB_I2C1_SCL_MBUS) | \
                             PIN_ODR_HIGH(GPIOB_I2C1_SDA_MBUS) | \
                             PIN_ODR_HIGH(GPIOB_PIN8) | \
                             PIN_ODR_HIGH(GPIOB_PIN9) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D3) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D4) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D5) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D6) | \
                             PIN_ODR_HIGH(GPIOB_PIN14) | \
                             PIN_ODR_HIGH(GPIOB_PIN15))
#define VAL_GPIOB_AFRL      (PIN_AFIO_AF(GPIOB_ULPI_D1, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D2, 10) | \
                             PIN_AFIO_AF(GPIOB_BOOT1, 0) | \
                             PIN_AFIO_AF(GPIOB_SWO, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_NRST, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D7, 10) | \
                             PIN_AFIO_AF(GPIOB_I2C1_SCL_MBUS, 4) | \
                             PIN_AFIO_AF(GPIOB_I2C1_SDA_MBUS, 4))
#define VAL_GPIOB_AFRH      (PIN_AFIO_AF(GPIOB_PIN8, 0) | \
                             PIN_AFIO_AF(GPIOB_PIN9, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D3, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D4, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D5, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D6, 10) | \
                             PIN_AFIO_AF(GPIOB_PIN14, 0) | \
                             PIN_AFIO_AF(GPIOB_PIN15, 0))
/*
 * GPIOC setup:
 *
*/
#define VAL_GPIOC_MODER     (PIN_MODE_ALTERNATE(GPIOC_ULPI_STP) | \
                             PIN_MODE_ANALOG(GPIOC_ADC123_CH11) | \
                             PIN_MODE_ALTERNATE(GPIOC_ULPI_DIR) | \
                             PIN_MODE_ANALOG(GPIOC_ADC123_CH13) | \
                             PIN_MODE_ANALOG(GPIOC_ADC12_CH14) | \
                             PIN_MODE_ANALOG(GPIOC_ADC12_CH15) | \
                             PIN_MODE_OUTPUT(GPIOC_PIN6) | \
                             PIN_MODE_INPUT(GPIOC_SD_DETECT) | \
                             PIN_MODE_ALTERNATE(GPIOC_SDIO_D0) | \
                             PIN_MODE_ALTERNATE(GPIOC_SDIO_D1) | \
                             PIN_MODE_ALTERNATE(GPIOC_SDIO_D2) | \
                             PIN_MODE_ALTERNATE(GPIOC_SDIO_D3) | \
                             PIN_MODE_ALTERNATE(GPIOC_SDIO_CLK) | \
                             PIN_MODE_INPUT(GPIOC_PIN13) | \
                             PIN_MODE_ALTERNATE(GPIOC_OSC32_IN) | \
                             PIN_MODE_ALTERNATE(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOC_ULPI_STP) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC123_CH11) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ULPI_DIR) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC123_CH13) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC12_CH14) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC12_CH15) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PIN6) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SD_DETECT) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D0) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D1) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D2) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D3) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_SDIO_CLK) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PIN13) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR   (PIN_OSPEED_100M(GPIOC_ULPI_STP) | \
                             PIN_OSPEED_100M(GPIOC_ADC123_CH11) | \
                             PIN_OSPEED_100M(GPIOC_ULPI_DIR) | \
                             PIN_OSPEED_100M(GPIOC_ADC123_CH13) | \
                             PIN_OSPEED_100M(GPIOC_ADC12_CH14) | \
                             PIN_OSPEED_100M(GPIOC_ADC12_CH15) | \
                             PIN_OSPEED_100M(GPIOC_PIN6) | \
                             PIN_OSPEED_100M(GPIOC_SD_DETECT) | \
                             PIN_OSPEED_100M(GPIOC_SDIO_D0) | \
                             PIN_OSPEED_100M(GPIOC_SDIO_D1) | \
                             PIN_OSPEED_100M(GPIOC_SDIO_D2) | \
                             PIN_OSPEED_100M(GPIOC_SDIO_D3) | \
                             PIN_OSPEED_100M(GPIOC_SDIO_CLK) | \
                             PIN_OSPEED_100M(GPIOC_PIN13) | \
                             PIN_OSPEED_100M(GPIOC_OSC32_IN) | \
                             PIN_OSPEED_100M(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR     (PIN_PUPDR_FLOATING(GPIOC_ULPI_STP) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC123_CH11) | \
                             PIN_PUPDR_FLOATING(GPIOC_ULPI_DIR) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC123_CH13) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC12_CH14) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC12_CH15) | \
                             PIN_PUPDR_FLOATING(GPIOC_PIN6) | \
                             PIN_PUPDR_PULLUP(GPIOC_SD_DETECT) | \
                             PIN_PUPDR_FLOATING(GPIOC_SDIO_D0) | \
                             PIN_PUPDR_FLOATING(GPIOC_SDIO_D1) | \
                             PIN_PUPDR_FLOATING(GPIOC_SDIO_D2) | \
                             PIN_PUPDR_FLOATING(GPIOC_SDIO_D3) | \
                             PIN_PUPDR_FLOATING(GPIOC_SDIO_CLK) | \
                             PIN_PUPDR_FLOATING(GPIOC_PIN13) | \
                             PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) | \
                             PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR       (PIN_ODR_HIGH(GPIOC_ULPI_STP) | \
                             PIN_ODR_HIGH(GPIOC_ADC123_CH11) | \
                             PIN_ODR_HIGH(GPIOC_ULPI_DIR) | \
                             PIN_ODR_HIGH(GPIOC_ADC123_CH13) | \
                             PIN_ODR_HIGH(GPIOC_ADC12_CH14) | \
                             PIN_ODR_HIGH(GPIOC_ADC12_CH15) | \
                             PIN_ODR_LOW(GPIOC_PIN6) | \
                             PIN_ODR_HIGH(GPIOC_SD_DETECT) | \
                             PIN_ODR_HIGH(GPIOC_SDIO_D0) | \
                             PIN_ODR_HIGH(GPIOC_SDIO_D1) | \
                             PIN_ODR_HIGH(GPIOC_SDIO_D2) | \
                             PIN_ODR_HIGH(GPIOC_SDIO_D3) | \
                             PIN_ODR_HIGH(GPIOC_SDIO_CLK) | \
                             PIN_ODR_HIGH(GPIOC_PIN13) | \
                             PIN_ODR_HIGH(GPIOC_OSC32_IN) | \
                             PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL      (PIN_AFIO_AF(GPIOC_ULPI_STP, 10) | \
                             PIN_AFIO_AF(GPIOC_ADC123_CH11, 0) | \
                             PIN_AFIO_AF(GPIOC_ULPI_DIR, 10) | \
                             PIN_AFIO_AF(GPIOC_ADC123_CH13, 0) | \
                             PIN_AFIO_AF(GPIOC_ADC12_CH14, 0) | \
                             PIN_AFIO_AF(GPIOC_ADC12_CH15, 0) | \
                             PIN_AFIO_AF(GPIOC_PIN6, 0) | \
                             PIN_AFIO_AF(GPIOC_SD_DETECT, 0))
#define VAL_GPIOC_AFRH      (PIN_AFIO_AF(GPIOC_SDIO_D0, 12) | \
                             PIN_AFIO_AF(GPIOC_SDIO_D1, 12) | \
                             PIN_AFIO_AF(GPIOC_SDIO_D2, 12) | \
                             PIN_AFIO_AF(GPIOC_SDIO_D3, 12) | \
                             PIN_AFIO_AF(GPIOC_SDIO_CLK, 12) | \
                             PIN_AFIO_AF(GPIOC_PIN13, 0) | \
                             PIN_AFIO_AF(GPIOC_OSC32_IN, 0) | \
                             PIN_AFIO_AF(GPIOC_OSC32_OUT, 0))
/*
 * GPIOD setup:
 *
*/
#define VAL_GPIOD_MODER     (PIN_MODE_INPUT(GPIOD_PIN0) | \
                             PIN_MODE_INPUT(GPIOD_PIN1) | \
                             PIN_MODE_ALTERNATE(GPIOD_SDIO_CMD) | \
                             PIN_MODE_INPUT(GPIOD_PIN3) | \
                             PIN_MODE_INPUT(GPIOD_PIN4) | \
                             PIN_MODE_INPUT(GPIOD_PIN5) | \
                             PIN_MODE_INPUT(GPIOD_PIN6) | \
                             PIN_MODE_OUTPUT(GPIOD_PIN7) | \
                             PIN_MODE_INPUT(GPIOD_PIN8) | \
                             PIN_MODE_INPUT(GPIOD_PIN9) | \
                             PIN_MODE_INPUT(GPIOD_PIN10) | \
                             PIN_MODE_INPUT(GPIOD_PIN11) | \
                             PIN_MODE_INPUT(GPIOD_PIN12) | \
                             PIN_MODE_OUTPUT(GPIOD_LED_STATUS_R) | \
                             PIN_MODE_OUTPUT(GPIOD_LED_STATUS_G) | \
                             PIN_MODE_OUTPUT(GPIOD_LED_STATUS_B))
#define VAL_GPIOD_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOD_PIN0) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN1) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_SDIO_CMD) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN3) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN4) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN5) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN6) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN7) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN8) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN9) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN10) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN11) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PIN12) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_LED_STATUS_R) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_LED_STATUS_G) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_LED_STATUS_B))
#define VAL_GPIOD_OSPEEDR   (PIN_OSPEED_100M(GPIOD_PIN0) | \
                             PIN_OSPEED_100M(GPIOD_PIN1) | \
                             PIN_OSPEED_100M(GPIOD_SDIO_CMD) | \
                             PIN_OSPEED_100M(GPIOD_PIN3) | \
                             PIN_OSPEED_100M(GPIOD_PIN4) | \
                             PIN_OSPEED_100M(GPIOD_PIN5) | \
                             PIN_OSPEED_100M(GPIOD_PIN6) | \
                             PIN_OSPEED_100M(GPIOD_PIN7) | \
                             PIN_OSPEED_100M(GPIOD_PIN8) | \
                             PIN_OSPEED_100M(GPIOD_PIN9) | \
                             PIN_OSPEED_100M(GPIOD_PIN10) | \
                             PIN_OSPEED_100M(GPIOD_PIN11) | \
                             PIN_OSPEED_100M(GPIOD_PIN12) | \
                             PIN_OSPEED_100M(GPIOD_LED_STATUS_R) | \
                             PIN_OSPEED_100M(GPIOD_LED_STATUS_G) | \
                             PIN_OSPEED_100M(GPIOD_LED_STATUS_B))
#define VAL_GPIOD_PUPDR     (PIN_PUPDR_FLOATING(GPIOD_PIN0) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN1) | \
                             PIN_PUPDR_FLOATING(GPIOD_SDIO_CMD) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN3) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN4) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN5) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN6) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN7) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN8) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN9) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN10) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN11) | \
                             PIN_PUPDR_FLOATING(GPIOD_PIN12) | \
                             PIN_PUPDR_FLOATING(GPIOD_LED_STATUS_R) | \
                             PIN_PUPDR_FLOATING(GPIOD_LED_STATUS_G) | \
                             PIN_PUPDR_FLOATING(GPIOD_LED_STATUS_B))
#define VAL_GPIOD_ODR       (PIN_ODR_HIGH(GPIOD_PIN0) | \
                             PIN_ODR_HIGH(GPIOD_PIN1) | \
                             PIN_ODR_HIGH(GPIOD_SDIO_CMD) | \
                             PIN_ODR_HIGH(GPIOD_PIN3) | \
                             PIN_ODR_HIGH(GPIOD_PIN4) | \
                             PIN_ODR_HIGH(GPIOD_PIN5) | \
                             PIN_ODR_HIGH(GPIOD_PIN6) | \
                             PIN_ODR_HIGH(GPIOD_PIN7) | \
                             PIN_ODR_HIGH(GPIOD_PIN8) | \
                             PIN_ODR_HIGH(GPIOD_PIN9) | \
                             PIN_ODR_HIGH(GPIOD_PIN10) | \
                             PIN_ODR_HIGH(GPIOD_PIN11) | \
                             PIN_ODR_HIGH(GPIOD_PIN12) | \
                             PIN_ODR_HIGH(GPIOD_LED_STATUS_R) | \
                             PIN_ODR_HIGH(GPIOD_LED_STATUS_G) | \
                             PIN_ODR_HIGH(GPIOD_LED_STATUS_B))
#define VAL_GPIOD_AFRL      (PIN_AFIO_AF(GPIOD_PIN0, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN1, 0) | \
                             PIN_AFIO_AF(GPIOD_SDIO_CMD, 12) | \
                             PIN_AFIO_AF(GPIOD_PIN3, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN4, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN5, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN6, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN7, 0))
#define VAL_GPIOD_AFRH      (PIN_AFIO_AF(GPIOD_PIN8, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN9, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN10, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN11, 0) | \
                             PIN_AFIO_AF(GPIOD_PIN12, 0) | \
                             PIN_AFIO_AF(GPIOD_LED_STATUS_R, 0) | \
                             PIN_AFIO_AF(GPIOD_LED_STATUS_G, 0) | \
                             PIN_AFIO_AF(GPIOD_LED_STATUS_B, 0))
/*
 * GPIOE setup:
 *
*/
#define VAL_GPIOE_MODER     (PIN_MODE_INPUT(GPIOE_PIN0) | \
                             PIN_MODE_INPUT(GPIOE_PIN1) | \
                             PIN_MODE_INPUT(GPIOE_PIN2) | \
                             PIN_MODE_INPUT(GPIOE_PIN3) | \
                             PIN_MODE_INPUT(GPIOE_PIN4) | \
                             PIN_MODE_INPUT(GPIOE_PIN5) | \
                             PIN_MODE_INPUT(GPIOE_PIN6) | \
                             PIN_MODE_INPUT(GPIOE_PIN7) | \
                             PIN_MODE_INPUT(GPIOE_PIN8) | \
                             PIN_MODE_INPUT(GPIOE_PIN9) | \
                             PIN_MODE_INPUT(GPIOE_PIN10) | \
                             PIN_MODE_OUTPUT(GPIOE_SPI4_NSS) | \
                             PIN_MODE_ALTERNATE(GPIOE_SPI4_SCK) | \
                             PIN_MODE_INPUT(GPIOE_PIN13) | \
                             PIN_MODE_ALTERNATE(GPIOE_SPI4_MOSI) | \
                             PIN_MODE_OUTPUT(GPIOE_PIN15))
#define VAL_GPIOE_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOE_PIN0) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN1) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN2) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN3) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN4) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN5) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN6) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN7) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN8) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN9) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN10) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_SPI4_NSS) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_SPI4_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN13) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_SPI4_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PIN15))
#define VAL_GPIOE_OSPEEDR   (PIN_OSPEED_100M(GPIOE_PIN0) | \
                             PIN_OSPEED_100M(GPIOE_PIN1) | \
                             PIN_OSPEED_100M(GPIOE_PIN2) | \
                             PIN_OSPEED_100M(GPIOE_PIN3) | \
                             PIN_OSPEED_100M(GPIOE_PIN4) | \
                             PIN_OSPEED_100M(GPIOE_PIN5) | \
                             PIN_OSPEED_100M(GPIOE_PIN6) | \
                             PIN_OSPEED_100M(GPIOE_PIN7) | \
                             PIN_OSPEED_100M(GPIOE_PIN8) | \
                             PIN_OSPEED_100M(GPIOE_PIN9) | \
                             PIN_OSPEED_100M(GPIOE_PIN10) | \
                             PIN_OSPEED_100M(GPIOE_SPI4_NSS) | \
                             PIN_OSPEED_100M(GPIOE_SPI4_SCK) | \
                             PIN_OSPEED_100M(GPIOE_PIN13) | \
                             PIN_OSPEED_100M(GPIOE_SPI4_MOSI) | \
                             PIN_OSPEED_100M(GPIOE_PIN15))
#define VAL_GPIOE_PUPDR     (PIN_PUPDR_FLOATING(GPIOE_PIN0) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN1) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN2) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN3) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN4) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN5) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN6) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN7) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN8) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN9) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN10) | \
                             PIN_PUPDR_FLOATING(GPIOE_SPI4_NSS) | \
                             PIN_PUPDR_FLOATING(GPIOE_SPI4_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN13) | \
                             PIN_PUPDR_FLOATING(GPIOE_SPI4_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOE_PIN15))
#define VAL_GPIOE_ODR       (PIN_ODR_HIGH(GPIOE_PIN0) | \
                             PIN_ODR_HIGH(GPIOE_PIN1) | \
                             PIN_ODR_HIGH(GPIOE_PIN2) | \
                             PIN_ODR_HIGH(GPIOE_PIN3) | \
                             PIN_ODR_HIGH(GPIOE_PIN4) | \
                             PIN_ODR_HIGH(GPIOE_PIN5) | \
                             PIN_ODR_HIGH(GPIOE_PIN6) | \
                             PIN_ODR_HIGH(GPIOE_PIN7) | \
                             PIN_ODR_HIGH(GPIOE_PIN8) | \
                             PIN_ODR_HIGH(GPIOE_PIN9) | \
                             PIN_ODR_HIGH(GPIOE_PIN10) | \
                             PIN_ODR_HIGH(GPIOE_SPI4_NSS) | \
                             PIN_ODR_HIGH(GPIOE_SPI4_SCK) | \
                             PIN_ODR_HIGH(GPIOE_PIN13) | \
                             PIN_ODR_HIGH(GPIOE_SPI4_MOSI) | \
                             PIN_ODR_HIGH(GPIOE_PIN15))
#define VAL_GPIOE_AFRL      (PIN_AFIO_AF(GPIOE_PIN0, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN1, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN2, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN3, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN4, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN5, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN6, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN7, 0))
#define VAL_GPIOE_AFRH      (PIN_AFIO_AF(GPIOE_PIN8, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN9, 0) | \
                             PIN_AFIO_AF(GPIOE_PIN10, 0) | \
                             PIN_AFIO_AF(GPIOE_SPI4_NSS, 0) | \
                             PIN_AFIO_AF(GPIOE_SPI4_SCK, 5) | \
                             PIN_AFIO_AF(GPIOE_PIN13, 5) | \
                             PIN_AFIO_AF(GPIOE_SPI4_MOSI, 5) | \
                             PIN_AFIO_AF(GPIOE_PIN15, 0))
/*
 * GPIOF setup:
 *
*/
#define VAL_GPIOF_MODER     (PIN_MODE_ALTERNATE(GPIOF_I2C2_SDA) | \
                             PIN_MODE_ALTERNATE(GPIOF_I2C2_SCL) | \
                             PIN_MODE_OUTPUT(GPIOF_CAN_SHDN) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH9) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH14) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH15) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH4) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH5) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH6) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH7) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH8) | \
                             PIN_MODE_INPUT(GPIOF_PIN11) | \
                             PIN_MODE_INPUT(GPIOF_PIN12) | \
                             PIN_MODE_INPUT(GPIOF_PIN13) | \
                             PIN_MODE_INPUT(GPIOF_PIN14) | \
                             PIN_MODE_INPUT(GPIOF_PIN15))
#define VAL_GPIOF_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOF_I2C2_SDA) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_I2C2_SCL) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_CAN_SHDN) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH9) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH14) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH15) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH4) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH5) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH6) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH7) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH8) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PIN11) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PIN12) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PIN13) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PIN14) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PIN15))
#define VAL_GPIOF_OSPEEDR   (PIN_OSPEED_100M(GPIOF_I2C2_SDA) | \
                             PIN_OSPEED_100M(GPIOF_I2C2_SCL) | \
                             PIN_OSPEED_100M(GPIOF_CAN_SHDN) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH9) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH14) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH15) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH4) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH5) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH6) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH7) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH8) | \
                             PIN_OSPEED_100M(GPIOF_PIN11) | \
                             PIN_OSPEED_100M(GPIOF_PIN12) | \
                             PIN_OSPEED_100M(GPIOF_PIN13) | \
                             PIN_OSPEED_100M(GPIOF_PIN14) | \
                             PIN_OSPEED_100M(GPIOF_PIN15))
#define VAL_GPIOF_PUPDR     (PIN_PUPDR_FLOATING(GPIOF_I2C2_SDA) | \
                             PIN_PUPDR_FLOATING(GPIOF_I2C2_SCL) | \
                             PIN_PUPDR_FLOATING(GPIOF_CAN_SHDN) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH9) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH14) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH15) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH4) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH5) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH6) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH7) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH8) | \
                             PIN_PUPDR_FLOATING(GPIOF_PIN11) | \
                             PIN_PUPDR_FLOATING(GPIOF_PIN12) | \
                             PIN_PUPDR_FLOATING(GPIOF_PIN13) | \
                             PIN_PUPDR_FLOATING(GPIOF_PIN14) | \
                             PIN_PUPDR_FLOATING(GPIOF_PIN15))
#define VAL_GPIOF_ODR       (PIN_ODR_HIGH(GPIOF_I2C2_SDA) | \
                             PIN_ODR_HIGH(GPIOF_I2C2_SCL) | \
                             PIN_ODR_HIGH(GPIOF_CAN_SHDN) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH9) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH14) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH15) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH4) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH5) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH6) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH7) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH8) | \
                             PIN_ODR_HIGH(GPIOF_PIN11) | \
                             PIN_ODR_HIGH(GPIOF_PIN12) | \
                             PIN_ODR_HIGH(GPIOF_PIN13) | \
                             PIN_ODR_HIGH(GPIOF_PIN14) | \
                             PIN_ODR_HIGH(GPIOF_PIN15))
#define VAL_GPIOF_AFRL      (PIN_AFIO_AF(GPIOF_I2C2_SDA, 4) | \
                             PIN_AFIO_AF(GPIOF_I2C2_SCL, 4) | \
                             PIN_AFIO_AF(GPIOF_CAN_SHDN, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH9, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH14, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH15, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH4, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH5, 0))
#define VAL_GPIOF_AFRH      (PIN_AFIO_AF(GPIOF_ADC3_CH6, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH7, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH8, 0) | \
                             PIN_AFIO_AF(GPIOF_PIN11, 0) | \
                             PIN_AFIO_AF(GPIOF_PIN12, 0) | \
                             PIN_AFIO_AF(GPIOF_PIN13, 0) | \
                             PIN_AFIO_AF(GPIOF_PIN14, 0) | \
                             PIN_AFIO_AF(GPIOF_PIN15, 0))
/*
 * GPIOG setup:
 *
*/
#define VAL_GPIOG_MODER     (PIN_MODE_INPUT(GPIOG_PIN0) | \
                             PIN_MODE_INPUT(GPIOG_PIN1) | \
                             PIN_MODE_INPUT(GPIOG_PIN2) | \
                             PIN_MODE_INPUT(GPIOG_PIN3) | \
                             PIN_MODE_INPUT(GPIOG_PIN4) | \
                             PIN_MODE_INPUT(GPIOG_PIN5) | \
                             PIN_MODE_INPUT(GPIOG_PIN6) | \
                             PIN_MODE_INPUT(GPIOG_PIN7) | \
                             PIN_MODE_INPUT(GPIOG_PIN8) | \
                             PIN_MODE_OUTPUT(GPIOG_LED_STATUS_1) | \
                             PIN_MODE_OUTPUT(GPIOG_LED_STATUS_2) | \
                             PIN_MODE_OUTPUT(GPIOG_LED_STATUS_3) | \
                             PIN_MODE_OUTPUT(GPIOG_SPI6_MISO) | \
                             PIN_MODE_OUTPUT(GPIOG_SPI6_SCK) | \
                             PIN_MODE_OUTPUT(GPIOG_SPI6_MOSI) | \
                             PIN_MODE_OUTPUT(GPIOG_LED_STATUS_7))
#define VAL_GPIOG_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOG_PIN0) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN1) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN2) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN3) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN4) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN5) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN6) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN7) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PIN8) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_LED_STATUS_1) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_LED_STATUS_2) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_LED_STATUS_3) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_SPI6_MISO) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_SPI6_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_SPI6_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_LED_STATUS_7))
#define VAL_GPIOG_OSPEEDR   (PIN_OSPEED_100M(GPIOG_PIN0) | \
                             PIN_OSPEED_100M(GPIOG_PIN1) | \
                             PIN_OSPEED_100M(GPIOG_PIN2) | \
                             PIN_OSPEED_100M(GPIOG_PIN3) | \
                             PIN_OSPEED_100M(GPIOG_PIN4) | \
                             PIN_OSPEED_100M(GPIOG_PIN5) | \
                             PIN_OSPEED_100M(GPIOG_PIN6) | \
                             PIN_OSPEED_100M(GPIOG_PIN7) | \
                             PIN_OSPEED_100M(GPIOG_PIN8) | \
                             PIN_OSPEED_100M(GPIOG_LED_STATUS_1) | \
                             PIN_OSPEED_100M(GPIOG_LED_STATUS_2) | \
                             PIN_OSPEED_100M(GPIOG_LED_STATUS_3) | \
                             PIN_OSPEED_100M(GPIOG_SPI6_MISO) | \
                             PIN_OSPEED_100M(GPIOG_SPI6_SCK) | \
                             PIN_OSPEED_100M(GPIOG_SPI6_MOSI) | \
                             PIN_OSPEED_100M(GPIOG_LED_STATUS_7))
#define VAL_GPIOG_PUPDR     (PIN_PUPDR_FLOATING(GPIOG_PIN0) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN1) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN2) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN3) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN4) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN5) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN6) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN7) | \
                             PIN_PUPDR_FLOATING(GPIOG_PIN8) | \
                             PIN_PUPDR_FLOATING(GPIOG_LED_STATUS_1) | \
                             PIN_PUPDR_FLOATING(GPIOG_LED_STATUS_2) | \
                             PIN_PUPDR_FLOATING(GPIOG_LED_STATUS_3) | \
                             PIN_PUPDR_FLOATING(GPIOG_SPI6_MISO) | \
                             PIN_PUPDR_FLOATING(GPIOG_SPI6_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOG_SPI6_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOG_LED_STATUS_7))
#define VAL_GPIOG_ODR       (PIN_ODR_HIGH(GPIOG_PIN0) | \
                             PIN_ODR_HIGH(GPIOG_PIN1) | \
                             PIN_ODR_HIGH(GPIOG_PIN2) | \
                             PIN_ODR_HIGH(GPIOG_PIN3) | \
                             PIN_ODR_HIGH(GPIOG_PIN4) | \
                             PIN_ODR_HIGH(GPIOG_PIN5) | \
                             PIN_ODR_HIGH(GPIOG_PIN6) | \
                             PIN_ODR_HIGH(GPIOG_PIN7) | \
                             PIN_ODR_HIGH(GPIOG_PIN8) | \
                             PIN_ODR_HIGH(GPIOG_LED_STATUS_1) | \
                             PIN_ODR_HIGH(GPIOG_LED_STATUS_2) | \
                             PIN_ODR_HIGH(GPIOG_LED_STATUS_3) | \
                             PIN_ODR_HIGH(GPIOG_SPI6_MISO) | \
                             PIN_ODR_HIGH(GPIOG_SPI6_SCK) | \
                             PIN_ODR_HIGH(GPIOG_SPI6_MOSI) | \
                             PIN_ODR_HIGH(GPIOG_LED_STATUS_7))
#define VAL_GPIOG_AFRL      (PIN_AFIO_AF(GPIOG_PIN0, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN1, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN2, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN3, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN4, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN5, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN6, 0) | \
                             PIN_AFIO_AF(GPIOG_PIN7, 0))
#define VAL_GPIOG_AFRH      (PIN_AFIO_AF(GPIOG_PIN8, 0) | \
                             PIN_AFIO_AF(GPIOG_LED_STATUS_1, 0) | \
                             PIN_AFIO_AF(GPIOG_LED_STATUS_2, 0) | \
                             PIN_AFIO_AF(GPIOG_LED_STATUS_3, 0) | \
                             PIN_AFIO_AF(GPIOG_SPI6_MISO, 0) | \
                             PIN_AFIO_AF(GPIOG_SPI6_SCK, 0) | \
                             PIN_AFIO_AF(GPIOG_SPI6_MOSI, 0) | \
                             PIN_AFIO_AF(GPIOG_LED_STATUS_7, 0))
/*
 * GPIOH setup:
 *
*/
#define VAL_GPIOH_MODER     (PIN_MODE_ALTERNATE(GPIOH_OSC_IN) | \
                             PIN_MODE_ALTERNATE(GPIOH_OSC_OUT) | \
                             PIN_MODE_INPUT(GPIOH_PIN2) | \
                             PIN_MODE_INPUT(GPIOH_PIN3) | \
                             PIN_MODE_ALTERNATE(GPIOH_ULPI_NXT) | \
                             PIN_MODE_INPUT(GPIOH_PIN5) | \
                             PIN_MODE_INPUT(GPIOH_PIN6) | \
                             PIN_MODE_INPUT(GPIOH_ADC3_CH4_MBUS) | \
                             PIN_MODE_INPUT(GPIOH_ADC3_CH8_MBUS) | \
                             PIN_MODE_INPUT(GPIOH_PIN9) | \
                             PIN_MODE_INPUT(GPIOH_PIN10) | \
                             PIN_MODE_INPUT(GPIOH_PIN11) | \
                             PIN_MODE_INPUT(GPIOH_PIN12) | \
                             PIN_MODE_ALTERNATE(GPIOH_CAN1_TX) | \
                             PIN_MODE_INPUT(GPIOH_PIN14) | \
                             PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN2) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN3) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_ULPI_NXT) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN5) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN6) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_ADC3_CH4_MBUS) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_ADC3_CH8_MBUS) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN9) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN10) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN11) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN12) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_CAN1_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN14) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR   (PIN_OSPEED_100M(GPIOH_OSC_IN) | \
                             PIN_OSPEED_100M(GPIOH_OSC_OUT) | \
                             PIN_OSPEED_100M(GPIOH_PIN2) | \
                             PIN_OSPEED_100M(GPIOH_PIN3) | \
                             PIN_OSPEED_100M(GPIOH_ULPI_NXT) | \
                             PIN_OSPEED_100M(GPIOH_PIN5) | \
                             PIN_OSPEED_100M(GPIOH_PIN6) | \
                             PIN_OSPEED_100M(GPIOH_ADC3_CH4_MBUS) | \
                             PIN_OSPEED_100M(GPIOH_ADC3_CH8_MBUS) | \
                             PIN_OSPEED_100M(GPIOH_PIN9) | \
                             PIN_OSPEED_100M(GPIOH_PIN10) | \
                             PIN_OSPEED_100M(GPIOH_PIN11) | \
                             PIN_OSPEED_100M(GPIOH_PIN12) | \
                             PIN_OSPEED_100M(GPIOH_CAN1_TX) | \
                             PIN_OSPEED_100M(GPIOH_PIN14) | \
                             PIN_OSPEED_100M(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR     (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) | \
                             PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN2) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN3) | \
                             PIN_PUPDR_FLOATING(GPIOH_ULPI_NXT) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN5) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN6) | \
                             PIN_PUPDR_FLOATING(GPIOH_ADC3_CH4_MBUS) | \
                             PIN_PUPDR_FLOATING(GPIOH_ADC3_CH8_MBUS) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN9) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN10) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN11) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN12) | \
                             PIN_PUPDR_FLOATING(GPIOH_CAN1_TX) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN14) | \
                             PIN_PUPDR_FLOATING(GPIOH_PIN15))
#define VAL_GPIOH_ODR       (PIN_ODR_HIGH(GPIOH_OSC_IN) | \
                             PIN_ODR_HIGH(GPIOH_OSC_OUT) | \
                             PIN_ODR_HIGH(GPIOH_PIN2) | \
                             PIN_ODR_HIGH(GPIOH_PIN3) | \
                             PIN_ODR_HIGH(GPIOH_ULPI_NXT) | \
                             PIN_ODR_HIGH(GPIOH_PIN5) | \
                             PIN_ODR_HIGH(GPIOH_PIN6) | \
                             PIN_ODR_HIGH(GPIOH_ADC3_CH4_MBUS) | \
                             PIN_ODR_HIGH(GPIOH_ADC3_CH8_MBUS) | \
                             PIN_ODR_HIGH(GPIOH_PIN9) | \
                             PIN_ODR_HIGH(GPIOH_PIN10) | \
                             PIN_ODR_HIGH(GPIOH_PIN11) | \
                             PIN_ODR_HIGH(GPIOH_PIN12) | \
                             PIN_ODR_HIGH(GPIOH_CAN1_TX) | \
                             PIN_ODR_HIGH(GPIOH_PIN14) | \
                             PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL      (PIN_AFIO_AF(GPIOH_OSC_IN, 0) | \
                             PIN_AFIO_AF(GPIOH_OSC_OUT, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN2, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN3, 0) | \
                             PIN_AFIO_AF(GPIOH_ULPI_NXT, 10) | \
                             PIN_AFIO_AF(GPIOH_PIN5, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN6, 0) | \
                             PIN_AFIO_AF(GPIOH_ADC3_CH4_MBUS, 0))
#define VAL_GPIOH_AFRH      (PIN_AFIO_AF(GPIOH_ADC3_CH8_MBUS, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN9, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN10, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN11, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN12, 0) | \
                             PIN_AFIO_AF(GPIOH_CAN1_TX, 9) | \
                             PIN_AFIO_AF(GPIOH_PIN14, 0) | \
                             PIN_AFIO_AF(GPIOH_PIN15, 0))
/*
 * GPIOI setup:
 *
*/
#define VAL_GPIOI_MODER     (PIN_MODE_INPUT(GPIOI_PIN0) | \
                             PIN_MODE_ALTERNATE(GPIOI_SPI2_SCK) | \
                             PIN_MODE_ALTERNATE(GPIOI_SPI2_MISO) | \
                             PIN_MODE_ALTERNATE(GPIOI_SPI2_MOSI) | \
                             PIN_MODE_OUTPUT(GPIOI_PIN4) | \
                             PIN_MODE_OUTPUT(GPIOI_ANALOG_BUS_ID) | \
                             PIN_MODE_OUTPUT(GPIOI_SERIAL_BUS_ID) | \
                             PIN_MODE_OUTPUT(GPIOI_GPIO_BUS_ID) | \
                             PIN_MODE_INPUT(GPIOI_PIN8) | \
                             PIN_MODE_ALTERNATE(GPIOI_CAN1_RX) | \
                             PIN_MODE_INPUT(GPIOI_PIN10) | \
                             PIN_MODE_INPUT(GPIOI_PIN11) | \
                             PIN_MODE_INPUT(GPIOI_PIN12) | \
                             PIN_MODE_INPUT(GPIOI_PIN13) | \
                             PIN_MODE_INPUT(GPIOI_PIN14) | \
                             PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOI_PIN0) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_SPI2_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_SPI2_MISO) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_SPI2_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN4) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_ANALOG_BUS_ID) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_SERIAL_BUS_ID) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_GPIO_BUS_ID) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN8) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_CAN1_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN10) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN11) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN12) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN13) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN14) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR   (PIN_OSPEED_100M(GPIOI_PIN0) | \
                             PIN_OSPEED_100M(GPIOI_SPI2_SCK) | \
                             PIN_OSPEED_100M(GPIOI_SPI2_MISO) | \
                             PIN_OSPEED_100M(GPIOI_SPI2_MOSI) | \
                             PIN_OSPEED_100M(GPIOI_PIN4) | \
                             PIN_OSPEED_100M(GPIOI_ANALOG_BUS_ID) | \
                             PIN_OSPEED_100M(GPIOI_SERIAL_BUS_ID) | \
                             PIN_OSPEED_100M(GPIOI_GPIO_BUS_ID) | \
                             PIN_OSPEED_100M(GPIOI_PIN8) | \
                             PIN_OSPEED_100M(GPIOI_CAN1_RX) | \
                             PIN_OSPEED_100M(GPIOI_PIN10) | \
                             PIN_OSPEED_100M(GPIOI_PIN11) | \
                             PIN_OSPEED_100M(GPIOI_PIN12) | \
                             PIN_OSPEED_100M(GPIOI_PIN13) | \
                             PIN_OSPEED_100M(GPIOI_PIN14) | \
                             PIN_OSPEED_100M(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR     (PIN_PUPDR_FLOATING(GPIOI_PIN0) | \
                             PIN_PUPDR_FLOATING(GPIOI_SPI2_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOI_SPI2_MISO) | \
                             PIN_PUPDR_FLOATING(GPIOI_SPI2_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN4) | \
                             PIN_PUPDR_FLOATING(GPIOI_ANALOG_BUS_ID) | \
                             PIN_PUPDR_FLOATING(GPIOI_SERIAL_BUS_ID) | \
                             PIN_PUPDR_FLOATING(GPIOI_GPIO_BUS_ID) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN8) | \
                             PIN_PUPDR_FLOATING(GPIOI_CAN1_RX) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN10) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN11) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN12) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN13) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN14) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN15))
#define VAL_GPIOI_ODR       (PIN_ODR_HIGH(GPIOI_PIN0) | \
                             PIN_ODR_HIGH(GPIOI_SPI2_SCK) | \
                             PIN_ODR_HIGH(GPIOI_SPI2_MISO) | \
                             PIN_ODR_HIGH(GPIOI_SPI2_MOSI) | \
                             PIN_ODR_HIGH(GPIOI_PIN4) | \
                             PIN_ODR_HIGH(GPIOI_ANALOG_BUS_ID) | \
                             PIN_ODR_HIGH(GPIOI_SERIAL_BUS_ID) | \
                             PIN_ODR_HIGH(GPIOI_GPIO_BUS_ID) | \
                             PIN_ODR_HIGH(GPIOI_PIN8) | \
                             PIN_ODR_HIGH(GPIOI_CAN1_RX) | \
                             PIN_ODR_HIGH(GPIOI_PIN10) | \
                             PIN_ODR_HIGH(GPIOI_PIN11) | \
                             PIN_ODR_HIGH(GPIOI_PIN12) | \
                             PIN_ODR_HIGH(GPIOI_PIN13) | \
                             PIN_ODR_HIGH(GPIOI_PIN14) | \
                             PIN_ODR_HIGH(GPIOI_PIN15))
#define VAL_GPIOI_AFRL      (PIN_AFIO_AF(GPIOI_PIN0, 0) | \
                             PIN_AFIO_AF(GPIOI_SPI2_SCK, 5) | \
                             PIN_AFIO_AF(GPIOI_SPI2_MISO, 5) | \
                             PIN_AFIO_AF(GPIOI_SPI2_MOSI, 5) | \
                             PIN_AFIO_AF(GPIOI_PIN4, 0) | \
                             PIN_AFIO_AF(GPIOI_ANALOG_BUS_ID, 0) | \
                             PIN_AFIO_AF(GPIOI_SERIAL_BUS_ID, 0) | \
                             PIN_AFIO_AF(GPIOI_GPIO_BUS_ID, 0))
#define VAL_GPIOI_AFRH      (PIN_AFIO_AF(GPIOI_PIN8, 0) | \
                             PIN_AFIO_AF(GPIOI_CAN1_RX, 9) | \
                             PIN_AFIO_AF(GPIOI_PIN10, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN11, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN12, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN13, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN14, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN15, 0))


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */

