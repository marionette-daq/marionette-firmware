
#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Board identifier.
 */
#define BOARD_MARIONETTE_PCB_REV2_2
#define BOARD_NAME                  "Marionette v2.2"

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

#define GPIOA_PA0_UART4_TX             0 
#define GPIOA_PA1_UART4_RX             1 
#define GPIOA_ADC2_CH2_FLT             2 
#define GPIOA_ULPI_D0                  3 
#define GPIOA_STM_DAC                  4 
#define GPIOA_ULPI_CK                  5 
#define GPIOA_ADC2_CH6_FLT             6 
#define GPIOA_ADC2_CH7_FLT             7 
#define GPIOA_MCO1                     8 //26MHz to ULPI PHY
#define GPIOA_VBUS_FILTERED            9 
#define GPIOA_PA10_SDIO_PWR            10 //active low
#define GPIOA_PA11_USB_FS_N            11 
#define GPIOA_PA12_USB_FS_P            12 
#define GPIOA_SWDIO                    13 
#define GPIOA_SWDCK                    14 
#define GPIOA_PA15_TIM2_CH1_ETR        15 

#define GPIOB_ULPI_D1                  0 
#define GPIOB_ULPI_D2                  1 
#define GPIOB_BOOT1                    2 
#define GPIOB_SWO                      3 
#define GPIOB_ULPI_RST_B               4 //active low reset
#define GPIOB_ULPI_D7                  5 
#define GPIOB_PB6_I2C1_SCL             6 
#define GPIOB_PB7_I2C1_SDA             7 
#define GPIOB_PB8_TIM4_CH3             8 
#define GPIOB_PB9_TIM4_CH4             9 
#define GPIOB_ULPI_D3                  10 
#define GPIOB_ULPI_D4                  11 
#define GPIOB_ULPI_D5                  12 
#define GPIOB_ULPI_D6                  13 
#define GPIOB_PB14                     14 
#define GPIOB_PB15                     15 

#define GPIOC_ULPI_STP                 0 
#define GPIOC_ADC2_CH11_FLT            1 
#define GPIOC_ULPI_DIR                 2 
#define GPIOC_ADC2_CH13_FLT            3 
#define GPIOC_ADC2_CH14_FLT            4 
#define GPIOC_ADC2_CH15_FLT            5 
#define GPIOC_PC6_DEBUG_USART6_TX      6 
#define GPIOC_PC7_DEBUG_USART6_RX      7 
#define GPIOC_PC8_SDIO_D0              8 
#define GPIOC_PC9_SDIO_D1              9 
#define GPIOC_PC10_SDIO_D2             10 
#define GPIOC_PC11_SDIO_D3             11 
#define GPIOC_PC12_SDIO_CK             12 
#define GPIOC_PC13                     13 
#define GPIOC_OSC32_IN                 14 
#define GPIOC_OSC32_OUT                15 

#define GPIOD_PD0                      0 
#define GPIOD_PD1                      1 
#define GPIOD_PD2_SDIO_CMD             2 
#define GPIOD_PD3_USART2_CTS           3 
#define GPIOD_PD4_USART2_RTS           4 
#define GPIOD_PD5_USART2_TX            5 
#define GPIOD_PD6_USART2_RX            6 
#define GPIOD_PD7                      7 
#define GPIOD_PD8_USART3_TX            8 
#define GPIOD_PD9_USART3_RX            9 
#define GPIOD_PD10_SD_DETECT           10 //SD socket switch
#define GPIOD_PD11_USART3_CTS          11 
#define GPIOD_PD12_USART3_RTS          12 
#define GPIOD_PD13_LED_STATUS_R        13 //active low
#define GPIOD_PD14_LED_STATUS_G        14 //active low
#define GPIOD_PD15_LED_STATUS_B        15 //active low

#define GPIOE_PE0_TIM4_ETR             0 
#define GPIOE_PE1                      1 
#define GPIOE_PE2                      2 
#define GPIOE_PE3                      3 
#define GPIOE_PE4                      4 
#define GPIOE_PE5_TIM9_CH1             5 
#define GPIOE_PE6_TIM9_CH2             6 
#define GPIOE_PE7_TIM1_ETR             7 
#define GPIOE_PE8                      8 
#define GPIOE_PE9_TIM1_CH1             9 
#define GPIOE_PE10                     10 
#define GPIOE_PE11_DAC_SPI4_NSS        11 
#define GPIOE_PE12_DAC_SPI4_SCK        12 
#define GPIOE_PE13_TIM1_CH3            13 
#define GPIOE_PE14_DAC_SPI4_MOSI       14 
#define GPIOE_PE15                     15 

#define GPIOF_PF0_I2C2_SDA             0 
#define GPIOF_PF1_I2C2_SCL             1 
#define GPIOF_PF2_CAN_SHDN             2 
#define GPIOF_ADC3_CH9_FLT             3 
#define GPIOF_ADC3_CH14_FLT            4 
#define GPIOF_ADC3_CH15_FLT            5 
#define GPIOF_PF6                      6 
#define GPIOF_ADC3_CH5_FLT             7 
#define GPIOF_ADC3_CH6_FLT             8 
#define GPIOF_ADC3_CH7_FLT             9 
#define GPIOF_ADC3_CH8_FLT             10 
#define GPIOF_PF11                     11 
#define GPIOF_PF12                     12 
#define GPIOF_PF13                     13 
#define GPIOF_PF14                     14 
#define GPIOF_PF15                     15 

#define GPIOG_PG0                      0 
#define GPIOG_PG1                      1 
#define GPIOG_PG2                      2 
#define GPIOG_PG3                      3 
#define GPIOG_PG4                      4 
#define GPIOG_PG5                      5 
#define GPIOG_PG6                      6 
#define GPIOG_PG7                      7 
#define GPIOG_PG8_SPI6_NSS             8 
#define GPIOG_PG9                      9 
#define GPIOG_PG10                     10 
#define GPIOG_PG11                     11 
#define GPIOG_PG12_SPI6_MISO           12 
#define GPIOG_PG13_SPI6_SCK            13 
#define GPIOG_PG14_SPI6_MOSI           14 
#define GPIOG_PG15                     15 

#define GPIOH_OSC_IN                   0 
#define GPIOH_OSC_OUT                  1 
#define GPIOH_PH2                      2 
#define GPIOH_PH3                      3 
#define GPIOH_ULPI_NXT                 4 
#define GPIOH_PH5                      5 
#define GPIOH_PH6                      6 
#define GPIOH_PH7_ADC3_CH4             7 
#define GPIOH_PH8_ADC3_CH8             8 
#define GPIOH_PH9                      9 
#define GPIOH_PH10_TIM5_CH1            10 
#define GPIOH_PH11_TIM5_CH2            11 
#define GPIOH_PH12_TIM5_CH3            12 
#define GPIOH_PH13_CAN1_TX             13 
#define GPIOH_PH14                     14 
#define GPIOH_PH15                     15 

#define GPIOI_PI0_SPI2_NSS             0 
#define GPIOI_PI1_SPI2_SCK             1 
#define GPIOI_PI2_SPI2_MISO            2 
#define GPIOI_PI3_SPI2_MOSI            3 
#define GPIOI_PI4                      4 
#define GPIOI_PI5_MBUS_ANA_SELECT      5 //active low
#define GPIOI_PI6_MBUS_SER_SELECT      6 //active low
#define GPIOI_PI7_MBUS_GPIO_SELECT     7 //active low
#define GPIOI_PI8                      8 
#define GPIOI_PI9_CAN1_RX              9 
#define GPIOI_PI10                     10 
#define GPIOI_PI11                     11 
#define GPIOI_PIN_12                   12 
#define GPIOI_PIN_13                   13 
#define GPIOI_PIN_14                   14 
#define GPIOI_PIN_15                   15 

/*
 * GPIOA setup:
 *
*/
#define VAL_GPIOA_MODER     (PIN_MODE_INPUT(GPIOA_PA0_UART4_TX) | \
                             PIN_MODE_INPUT(GPIOA_PA1_UART4_RX) | \
                             PIN_MODE_ANALOG(GPIOA_ADC2_CH2_FLT) | \
                             PIN_MODE_ALTERNATE(GPIOA_ULPI_D0) | \
                             PIN_MODE_ANALOG(GPIOA_STM_DAC) | \
                             PIN_MODE_ALTERNATE(GPIOA_ULPI_CK) | \
                             PIN_MODE_ANALOG(GPIOA_ADC2_CH6_FLT) | \
                             PIN_MODE_ANALOG(GPIOA_ADC2_CH7_FLT) | \
                             PIN_MODE_ALTERNATE(GPIOA_MCO1) | \
                             PIN_MODE_INPUT(GPIOA_VBUS_FILTERED) | \
                             PIN_MODE_OUTPUT(GPIOA_PA10_SDIO_PWR) | \
                             PIN_MODE_ALTERNATE(GPIOA_PA11_USB_FS_N) | \
                             PIN_MODE_ALTERNATE(GPIOA_PA12_USB_FS_P) | \
                             PIN_MODE_ALTERNATE(GPIOA_SWDIO) | \
                             PIN_MODE_ALTERNATE(GPIOA_SWDCK) | \
                             PIN_MODE_INPUT(GPIOA_PA15_TIM2_CH1_ETR))
#define VAL_GPIOA_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOA_PA0_UART4_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PA1_UART4_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC2_CH2_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ULPI_D0) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_STM_DAC) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ULPI_CK) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC2_CH6_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_ADC2_CH7_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_MCO1) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_VBUS_FILTERED) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PA10_SDIO_PWR) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PA11_USB_FS_N) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PA12_USB_FS_P) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_SWDCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOA_PA15_TIM2_CH1_ETR))
#define VAL_GPIOA_OSPEEDR   (PIN_OSPEED_100M(GPIOA_PA0_UART4_TX) | \
                             PIN_OSPEED_100M(GPIOA_PA1_UART4_RX) | \
                             PIN_OSPEED_100M(GPIOA_ADC2_CH2_FLT) | \
                             PIN_OSPEED_100M(GPIOA_ULPI_D0) | \
                             PIN_OSPEED_100M(GPIOA_STM_DAC) | \
                             PIN_OSPEED_100M(GPIOA_ULPI_CK) | \
                             PIN_OSPEED_100M(GPIOA_ADC2_CH6_FLT) | \
                             PIN_OSPEED_100M(GPIOA_ADC2_CH7_FLT) | \
                             PIN_OSPEED_100M(GPIOA_MCO1) | \
                             PIN_OSPEED_100M(GPIOA_VBUS_FILTERED) | \
                             PIN_OSPEED_100M(GPIOA_PA10_SDIO_PWR) | \
                             PIN_OSPEED_100M(GPIOA_PA11_USB_FS_N) | \
                             PIN_OSPEED_100M(GPIOA_PA12_USB_FS_P) | \
                             PIN_OSPEED_100M(GPIOA_SWDIO) | \
                             PIN_OSPEED_100M(GPIOA_SWDCK) | \
                             PIN_OSPEED_100M(GPIOA_PA15_TIM2_CH1_ETR))
#define VAL_GPIOA_PUPDR     (PIN_PUPDR_FLOATING(GPIOA_PA0_UART4_TX) | \
                             PIN_PUPDR_FLOATING(GPIOA_PA1_UART4_RX) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC2_CH2_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOA_ULPI_D0) | \
                             PIN_PUPDR_FLOATING(GPIOA_STM_DAC) | \
                             PIN_PUPDR_FLOATING(GPIOA_ULPI_CK) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC2_CH6_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOA_ADC2_CH7_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOA_MCO1) | \
                             PIN_PUPDR_FLOATING(GPIOA_VBUS_FILTERED) | \
                             PIN_PUPDR_FLOATING(GPIOA_PA10_SDIO_PWR) | \
                             PIN_PUPDR_FLOATING(GPIOA_PA11_USB_FS_N) | \
                             PIN_PUPDR_FLOATING(GPIOA_PA12_USB_FS_P) | \
                             PIN_PUPDR_FLOATING(GPIOA_SWDIO) | \
                             PIN_PUPDR_FLOATING(GPIOA_SWDCK) | \
                             PIN_PUPDR_FLOATING(GPIOA_PA15_TIM2_CH1_ETR))
#define VAL_GPIOA_ODR       (PIN_ODR_HIGH(GPIOA_PA0_UART4_TX) | \
                             PIN_ODR_HIGH(GPIOA_PA1_UART4_RX) | \
                             PIN_ODR_HIGH(GPIOA_ADC2_CH2_FLT) | \
                             PIN_ODR_HIGH(GPIOA_ULPI_D0) | \
                             PIN_ODR_HIGH(GPIOA_STM_DAC) | \
                             PIN_ODR_HIGH(GPIOA_ULPI_CK) | \
                             PIN_ODR_HIGH(GPIOA_ADC2_CH6_FLT) | \
                             PIN_ODR_HIGH(GPIOA_ADC2_CH7_FLT) | \
                             PIN_ODR_HIGH(GPIOA_MCO1) | \
                             PIN_ODR_HIGH(GPIOA_VBUS_FILTERED) | \
                             PIN_ODR_HIGH(GPIOA_PA10_SDIO_PWR) | \
                             PIN_ODR_HIGH(GPIOA_PA11_USB_FS_N) | \
                             PIN_ODR_HIGH(GPIOA_PA12_USB_FS_P) | \
                             PIN_ODR_HIGH(GPIOA_SWDIO) | \
                             PIN_ODR_HIGH(GPIOA_SWDCK) | \
                             PIN_ODR_HIGH(GPIOA_PA15_TIM2_CH1_ETR))
#define VAL_GPIOA_AFRL      (PIN_AFIO_AF(GPIOA_PA0_UART4_TX, 0) | \
                             PIN_AFIO_AF(GPIOA_PA1_UART4_RX, 0) | \
                             PIN_AFIO_AF(GPIOA_ADC2_CH2_FLT, 0) | \
                             PIN_AFIO_AF(GPIOA_ULPI_D0, 10) | \
                             PIN_AFIO_AF(GPIOA_STM_DAC, 0) | \
                             PIN_AFIO_AF(GPIOA_ULPI_CK, 10) | \
                             PIN_AFIO_AF(GPIOA_ADC2_CH6_FLT, 0) | \
                             PIN_AFIO_AF(GPIOA_ADC2_CH7_FLT, 0))
#define VAL_GPIOA_AFRH      (PIN_AFIO_AF(GPIOA_MCO1, 0) | \
                             PIN_AFIO_AF(GPIOA_VBUS_FILTERED, 0) | \
                             PIN_AFIO_AF(GPIOA_PA10_SDIO_PWR, 0) | \
                             PIN_AFIO_AF(GPIOA_PA11_USB_FS_N, 10) | \
                             PIN_AFIO_AF(GPIOA_PA12_USB_FS_P, 10) | \
                             PIN_AFIO_AF(GPIOA_SWDIO, 0) | \
                             PIN_AFIO_AF(GPIOA_SWDCK, 0) | \
                             PIN_AFIO_AF(GPIOA_PA15_TIM2_CH1_ETR, 0))
/*
 * GPIOB setup:
 *
*/
#define VAL_GPIOB_MODER     (PIN_MODE_ALTERNATE(GPIOB_ULPI_D1) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D2) | \
                             PIN_MODE_INPUT(GPIOB_BOOT1) | \
                             PIN_MODE_ALTERNATE(GPIOB_SWO) | \
                             PIN_MODE_OUTPUT(GPIOB_ULPI_RST_B) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D7) | \
                             PIN_MODE_INPUT(GPIOB_PB6_I2C1_SCL) | \
                             PIN_MODE_INPUT(GPIOB_PB7_I2C1_SDA) | \
                             PIN_MODE_INPUT(GPIOB_PB8_TIM4_CH3) | \
                             PIN_MODE_INPUT(GPIOB_PB9_TIM4_CH4) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D3) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D4) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D5) | \
                             PIN_MODE_ALTERNATE(GPIOB_ULPI_D6) | \
                             PIN_MODE_INPUT(GPIOB_PB14) | \
                             PIN_MODE_INPUT(GPIOB_PB15))
#define VAL_GPIOB_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D1) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D2) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_SWO) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_RST_B) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D7) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB6_I2C1_SCL) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB7_I2C1_SDA) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB8_TIM4_CH3) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB9_TIM4_CH4) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D3) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D4) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D5) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D6) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB14) | \
                             PIN_OTYPE_PUSHPULL(GPIOB_PB15))
#define VAL_GPIOB_OSPEEDR   (PIN_OSPEED_100M(GPIOB_ULPI_D1) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D2) | \
                             PIN_OSPEED_100M(GPIOB_BOOT1) | \
                             PIN_OSPEED_100M(GPIOB_SWO) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_RST_B) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D7) | \
                             PIN_OSPEED_100M(GPIOB_PB6_I2C1_SCL) | \
                             PIN_OSPEED_100M(GPIOB_PB7_I2C1_SDA) | \
                             PIN_OSPEED_100M(GPIOB_PB8_TIM4_CH3) | \
                             PIN_OSPEED_100M(GPIOB_PB9_TIM4_CH4) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D3) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D4) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D5) | \
                             PIN_OSPEED_100M(GPIOB_ULPI_D6) | \
                             PIN_OSPEED_100M(GPIOB_PB14) | \
                             PIN_OSPEED_100M(GPIOB_PB15))
#define VAL_GPIOB_PUPDR     (PIN_PUPDR_FLOATING(GPIOB_ULPI_D1) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D2) | \
                             PIN_PUPDR_FLOATING(GPIOB_BOOT1) | \
                             PIN_PUPDR_FLOATING(GPIOB_SWO) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_RST_B) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D7) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB6_I2C1_SCL) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB7_I2C1_SDA) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB8_TIM4_CH3) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB9_TIM4_CH4) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D3) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D4) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D5) | \
                             PIN_PUPDR_FLOATING(GPIOB_ULPI_D6) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB14) | \
                             PIN_PUPDR_FLOATING(GPIOB_PB15))
#define VAL_GPIOB_ODR       (PIN_ODR_HIGH(GPIOB_ULPI_D1) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D2) | \
                             PIN_ODR_HIGH(GPIOB_BOOT1) | \
                             PIN_ODR_HIGH(GPIOB_SWO) | \
                             PIN_ODR_LOW(GPIOB_ULPI_RST_B) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D7) | \
                             PIN_ODR_HIGH(GPIOB_PB6_I2C1_SCL) | \
                             PIN_ODR_HIGH(GPIOB_PB7_I2C1_SDA) | \
                             PIN_ODR_HIGH(GPIOB_PB8_TIM4_CH3) | \
                             PIN_ODR_HIGH(GPIOB_PB9_TIM4_CH4) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D3) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D4) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D5) | \
                             PIN_ODR_HIGH(GPIOB_ULPI_D6) | \
                             PIN_ODR_HIGH(GPIOB_PB14) | \
                             PIN_ODR_HIGH(GPIOB_PB15))
#define VAL_GPIOB_AFRL      (PIN_AFIO_AF(GPIOB_ULPI_D1, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D2, 10) | \
                             PIN_AFIO_AF(GPIOB_BOOT1, 0) | \
                             PIN_AFIO_AF(GPIOB_SWO, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_RST_B, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D7, 10) | \
                             PIN_AFIO_AF(GPIOB_PB6_I2C1_SCL, 0) | \
                             PIN_AFIO_AF(GPIOB_PB7_I2C1_SDA, 0))
#define VAL_GPIOB_AFRH      (PIN_AFIO_AF(GPIOB_PB8_TIM4_CH3, 0) | \
                             PIN_AFIO_AF(GPIOB_PB9_TIM4_CH4, 0) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D3, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D4, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D5, 10) | \
                             PIN_AFIO_AF(GPIOB_ULPI_D6, 10) | \
                             PIN_AFIO_AF(GPIOB_PB14, 0) | \
                             PIN_AFIO_AF(GPIOB_PB15, 0))
/*
 * GPIOC setup:
 *
*/
#define VAL_GPIOC_MODER     (PIN_MODE_ALTERNATE(GPIOC_ULPI_STP) | \
                             PIN_MODE_ANALOG(GPIOC_ADC2_CH11_FLT) | \
                             PIN_MODE_ALTERNATE(GPIOC_ULPI_DIR) | \
                             PIN_MODE_ANALOG(GPIOC_ADC2_CH13_FLT) | \
                             PIN_MODE_ANALOG(GPIOC_ADC2_CH14_FLT) | \
                             PIN_MODE_ANALOG(GPIOC_ADC2_CH15_FLT) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC6_DEBUG_USART6_TX) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC7_DEBUG_USART6_RX) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC8_SDIO_D0) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC9_SDIO_D1) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC10_SDIO_D2) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC11_SDIO_D3) | \
                             PIN_MODE_ALTERNATE(GPIOC_PC12_SDIO_CK) | \
                             PIN_MODE_INPUT(GPIOC_PC13) | \
                             PIN_MODE_ALTERNATE(GPIOC_OSC32_IN) | \
                             PIN_MODE_ALTERNATE(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOC_ULPI_STP) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC2_CH11_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ULPI_DIR) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC2_CH13_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC2_CH14_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_ADC2_CH15_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC6_DEBUG_USART6_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC7_DEBUG_USART6_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC8_SDIO_D0) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC9_SDIO_D1) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC10_SDIO_D2) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC11_SDIO_D3) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC12_SDIO_CK) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_PC13) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) | \
                             PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR   (PIN_OSPEED_100M(GPIOC_ULPI_STP) | \
                             PIN_OSPEED_100M(GPIOC_ADC2_CH11_FLT) | \
                             PIN_OSPEED_100M(GPIOC_ULPI_DIR) | \
                             PIN_OSPEED_100M(GPIOC_ADC2_CH13_FLT) | \
                             PIN_OSPEED_100M(GPIOC_ADC2_CH14_FLT) | \
                             PIN_OSPEED_100M(GPIOC_ADC2_CH15_FLT) | \
                             PIN_OSPEED_100M(GPIOC_PC6_DEBUG_USART6_TX) | \
                             PIN_OSPEED_100M(GPIOC_PC7_DEBUG_USART6_RX) | \
                             PIN_OSPEED_100M(GPIOC_PC8_SDIO_D0) | \
                             PIN_OSPEED_100M(GPIOC_PC9_SDIO_D1) | \
                             PIN_OSPEED_100M(GPIOC_PC10_SDIO_D2) | \
                             PIN_OSPEED_100M(GPIOC_PC11_SDIO_D3) | \
                             PIN_OSPEED_100M(GPIOC_PC12_SDIO_CK) | \
                             PIN_OSPEED_100M(GPIOC_PC13) | \
                             PIN_OSPEED_100M(GPIOC_OSC32_IN) | \
                             PIN_OSPEED_100M(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR     (PIN_PUPDR_FLOATING(GPIOC_ULPI_STP) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC2_CH11_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOC_ULPI_DIR) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC2_CH13_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC2_CH14_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOC_ADC2_CH15_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC6_DEBUG_USART6_TX) | \
                             PIN_PUPDR_PULLUP(GPIOC_PC7_DEBUG_USART6_RX) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC8_SDIO_D0) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC9_SDIO_D1) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC10_SDIO_D2) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC11_SDIO_D3) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC12_SDIO_CK) | \
                             PIN_PUPDR_FLOATING(GPIOC_PC13) | \
                             PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) | \
                             PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR       (PIN_ODR_HIGH(GPIOC_ULPI_STP) | \
                             PIN_ODR_HIGH(GPIOC_ADC2_CH11_FLT) | \
                             PIN_ODR_HIGH(GPIOC_ULPI_DIR) | \
                             PIN_ODR_HIGH(GPIOC_ADC2_CH13_FLT) | \
                             PIN_ODR_HIGH(GPIOC_ADC2_CH14_FLT) | \
                             PIN_ODR_HIGH(GPIOC_ADC2_CH15_FLT) | \
                             PIN_ODR_HIGH(GPIOC_PC6_DEBUG_USART6_TX) | \
                             PIN_ODR_HIGH(GPIOC_PC7_DEBUG_USART6_RX) | \
                             PIN_ODR_LOW(GPIOC_PC8_SDIO_D0) | \
                             PIN_ODR_LOW(GPIOC_PC9_SDIO_D1) | \
                             PIN_ODR_LOW(GPIOC_PC10_SDIO_D2) | \
                             PIN_ODR_LOW(GPIOC_PC11_SDIO_D3) | \
                             PIN_ODR_LOW(GPIOC_PC12_SDIO_CK) | \
                             PIN_ODR_HIGH(GPIOC_PC13) | \
                             PIN_ODR_HIGH(GPIOC_OSC32_IN) | \
                             PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL      (PIN_AFIO_AF(GPIOC_ULPI_STP, 10) | \
                             PIN_AFIO_AF(GPIOC_ADC2_CH11_FLT, 0) | \
                             PIN_AFIO_AF(GPIOC_ULPI_DIR, 10) | \
                             PIN_AFIO_AF(GPIOC_ADC2_CH13_FLT, 0) | \
                             PIN_AFIO_AF(GPIOC_ADC2_CH14_FLT, 0) | \
                             PIN_AFIO_AF(GPIOC_ADC2_CH15_FLT, 0) | \
                             PIN_AFIO_AF(GPIOC_PC6_DEBUG_USART6_TX, 8) | \
                             PIN_AFIO_AF(GPIOC_PC7_DEBUG_USART6_RX, 8))
#define VAL_GPIOC_AFRH      (PIN_AFIO_AF(GPIOC_PC8_SDIO_D0, 12) | \
                             PIN_AFIO_AF(GPIOC_PC9_SDIO_D1, 12) | \
                             PIN_AFIO_AF(GPIOC_PC10_SDIO_D2, 12) | \
                             PIN_AFIO_AF(GPIOC_PC11_SDIO_D3, 12) | \
                             PIN_AFIO_AF(GPIOC_PC12_SDIO_CK, 12) | \
                             PIN_AFIO_AF(GPIOC_PC13, 0) | \
                             PIN_AFIO_AF(GPIOC_OSC32_IN, 0) | \
                             PIN_AFIO_AF(GPIOC_OSC32_OUT, 0))
/*
 * GPIOD setup:
 *
*/
#define VAL_GPIOD_MODER     (PIN_MODE_INPUT(GPIOD_PD0) | \
                             PIN_MODE_INPUT(GPIOD_PD1) | \
                             PIN_MODE_ALTERNATE(GPIOD_PD2_SDIO_CMD) | \
                             PIN_MODE_INPUT(GPIOD_PD3_USART2_CTS) | \
                             PIN_MODE_INPUT(GPIOD_PD4_USART2_RTS) | \
                             PIN_MODE_INPUT(GPIOD_PD5_USART2_TX) | \
                             PIN_MODE_INPUT(GPIOD_PD6_USART2_RX) | \
                             PIN_MODE_INPUT(GPIOD_PD7) | \
                             PIN_MODE_INPUT(GPIOD_PD8_USART3_TX) | \
                             PIN_MODE_INPUT(GPIOD_PD9_USART3_RX) | \
                             PIN_MODE_INPUT(GPIOD_PD10_SD_DETECT) | \
                             PIN_MODE_INPUT(GPIOD_PD11_USART3_CTS) | \
                             PIN_MODE_INPUT(GPIOD_PD12_USART3_RTS) | \
                             PIN_MODE_OUTPUT(GPIOD_PD13_LED_STATUS_R) | \
                             PIN_MODE_OUTPUT(GPIOD_PD14_LED_STATUS_G) | \
                             PIN_MODE_OUTPUT(GPIOD_PD15_LED_STATUS_B))
#define VAL_GPIOD_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOD_PD0) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD1) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD2_SDIO_CMD) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD3_USART2_CTS) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD4_USART2_RTS) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD5_USART2_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD6_USART2_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD7) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD8_USART3_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD9_USART3_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD10_SD_DETECT) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD11_USART3_CTS) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD12_USART3_RTS) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD13_LED_STATUS_R) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD14_LED_STATUS_G) | \
                             PIN_OTYPE_PUSHPULL(GPIOD_PD15_LED_STATUS_B))
#define VAL_GPIOD_OSPEEDR   (PIN_OSPEED_100M(GPIOD_PD0) | \
                             PIN_OSPEED_100M(GPIOD_PD1) | \
                             PIN_OSPEED_100M(GPIOD_PD2_SDIO_CMD) | \
                             PIN_OSPEED_100M(GPIOD_PD3_USART2_CTS) | \
                             PIN_OSPEED_100M(GPIOD_PD4_USART2_RTS) | \
                             PIN_OSPEED_100M(GPIOD_PD5_USART2_TX) | \
                             PIN_OSPEED_100M(GPIOD_PD6_USART2_RX) | \
                             PIN_OSPEED_100M(GPIOD_PD7) | \
                             PIN_OSPEED_100M(GPIOD_PD8_USART3_TX) | \
                             PIN_OSPEED_100M(GPIOD_PD9_USART3_RX) | \
                             PIN_OSPEED_100M(GPIOD_PD10_SD_DETECT) | \
                             PIN_OSPEED_100M(GPIOD_PD11_USART3_CTS) | \
                             PIN_OSPEED_100M(GPIOD_PD12_USART3_RTS) | \
                             PIN_OSPEED_100M(GPIOD_PD13_LED_STATUS_R) | \
                             PIN_OSPEED_100M(GPIOD_PD14_LED_STATUS_G) | \
                             PIN_OSPEED_100M(GPIOD_PD15_LED_STATUS_B))
#define VAL_GPIOD_PUPDR     (PIN_PUPDR_FLOATING(GPIOD_PD0) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD1) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD2_SDIO_CMD) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD3_USART2_CTS) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD4_USART2_RTS) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD5_USART2_TX) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD6_USART2_RX) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD7) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD8_USART3_TX) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD9_USART3_RX) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD10_SD_DETECT) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD11_USART3_CTS) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD12_USART3_RTS) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD13_LED_STATUS_R) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD14_LED_STATUS_G) | \
                             PIN_PUPDR_FLOATING(GPIOD_PD15_LED_STATUS_B))
#define VAL_GPIOD_ODR       (PIN_ODR_HIGH(GPIOD_PD0) | \
                             PIN_ODR_HIGH(GPIOD_PD1) | \
                             PIN_ODR_LOW(GPIOD_PD2_SDIO_CMD) | \
                             PIN_ODR_HIGH(GPIOD_PD3_USART2_CTS) | \
                             PIN_ODR_HIGH(GPIOD_PD4_USART2_RTS) | \
                             PIN_ODR_HIGH(GPIOD_PD5_USART2_TX) | \
                             PIN_ODR_HIGH(GPIOD_PD6_USART2_RX) | \
                             PIN_ODR_HIGH(GPIOD_PD7) | \
                             PIN_ODR_HIGH(GPIOD_PD8_USART3_TX) | \
                             PIN_ODR_HIGH(GPIOD_PD9_USART3_RX) | \
                             PIN_ODR_HIGH(GPIOD_PD10_SD_DETECT) | \
                             PIN_ODR_HIGH(GPIOD_PD11_USART3_CTS) | \
                             PIN_ODR_HIGH(GPIOD_PD12_USART3_RTS) | \
                             PIN_ODR_HIGH(GPIOD_PD13_LED_STATUS_R) | \
                             PIN_ODR_HIGH(GPIOD_PD14_LED_STATUS_G) | \
                             PIN_ODR_HIGH(GPIOD_PD15_LED_STATUS_B))
#define VAL_GPIOD_AFRL      (PIN_AFIO_AF(GPIOD_PD0, 0) | \
                             PIN_AFIO_AF(GPIOD_PD1, 0) | \
                             PIN_AFIO_AF(GPIOD_PD2_SDIO_CMD, 12) | \
                             PIN_AFIO_AF(GPIOD_PD3_USART2_CTS, 0) | \
                             PIN_AFIO_AF(GPIOD_PD4_USART2_RTS, 0) | \
                             PIN_AFIO_AF(GPIOD_PD5_USART2_TX, 0) | \
                             PIN_AFIO_AF(GPIOD_PD6_USART2_RX, 0) | \
                             PIN_AFIO_AF(GPIOD_PD7, 0))
#define VAL_GPIOD_AFRH      (PIN_AFIO_AF(GPIOD_PD8_USART3_TX, 0) | \
                             PIN_AFIO_AF(GPIOD_PD9_USART3_RX, 0) | \
                             PIN_AFIO_AF(GPIOD_PD10_SD_DETECT, 0) | \
                             PIN_AFIO_AF(GPIOD_PD11_USART3_CTS, 0) | \
                             PIN_AFIO_AF(GPIOD_PD12_USART3_RTS, 0) | \
                             PIN_AFIO_AF(GPIOD_PD13_LED_STATUS_R, 0) | \
                             PIN_AFIO_AF(GPIOD_PD14_LED_STATUS_G, 0) | \
                             PIN_AFIO_AF(GPIOD_PD15_LED_STATUS_B, 0))
/*
 * GPIOE setup:
 *
*/
#define VAL_GPIOE_MODER     (PIN_MODE_INPUT(GPIOE_PE0_TIM4_ETR) | \
                             PIN_MODE_INPUT(GPIOE_PE1) | \
                             PIN_MODE_INPUT(GPIOE_PE2) | \
                             PIN_MODE_INPUT(GPIOE_PE3) | \
                             PIN_MODE_INPUT(GPIOE_PE4) | \
                             PIN_MODE_INPUT(GPIOE_PE5_TIM9_CH1) | \
                             PIN_MODE_INPUT(GPIOE_PE6_TIM9_CH2) | \
                             PIN_MODE_INPUT(GPIOE_PE7_TIM1_ETR) | \
                             PIN_MODE_INPUT(GPIOE_PE8) | \
                             PIN_MODE_INPUT(GPIOE_PE9_TIM1_CH1) | \
                             PIN_MODE_INPUT(GPIOE_PE10) | \
                             PIN_MODE_OUTPUT(GPIOE_PE11_DAC_SPI4_NSS) | \
                             PIN_MODE_ALTERNATE(GPIOE_PE12_DAC_SPI4_SCK) | \
                             PIN_MODE_INPUT(GPIOE_PE13_TIM1_CH3) | \
                             PIN_MODE_ALTERNATE(GPIOE_PE14_DAC_SPI4_MOSI) | \
                             PIN_MODE_INPUT(GPIOE_PE15))
#define VAL_GPIOE_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOE_PE0_TIM4_ETR) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE1) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE2) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE3) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE4) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE5_TIM9_CH1) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE6_TIM9_CH2) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE7_TIM1_ETR) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE8) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE9_TIM1_CH1) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE10) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE11_DAC_SPI4_NSS) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE12_DAC_SPI4_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE13_TIM1_CH3) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE14_DAC_SPI4_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOE_PE15))
#define VAL_GPIOE_OSPEEDR   (PIN_OSPEED_100M(GPIOE_PE0_TIM4_ETR) | \
                             PIN_OSPEED_100M(GPIOE_PE1) | \
                             PIN_OSPEED_100M(GPIOE_PE2) | \
                             PIN_OSPEED_100M(GPIOE_PE3) | \
                             PIN_OSPEED_100M(GPIOE_PE4) | \
                             PIN_OSPEED_100M(GPIOE_PE5_TIM9_CH1) | \
                             PIN_OSPEED_100M(GPIOE_PE6_TIM9_CH2) | \
                             PIN_OSPEED_100M(GPIOE_PE7_TIM1_ETR) | \
                             PIN_OSPEED_100M(GPIOE_PE8) | \
                             PIN_OSPEED_100M(GPIOE_PE9_TIM1_CH1) | \
                             PIN_OSPEED_100M(GPIOE_PE10) | \
                             PIN_OSPEED_100M(GPIOE_PE11_DAC_SPI4_NSS) | \
                             PIN_OSPEED_100M(GPIOE_PE12_DAC_SPI4_SCK) | \
                             PIN_OSPEED_100M(GPIOE_PE13_TIM1_CH3) | \
                             PIN_OSPEED_100M(GPIOE_PE14_DAC_SPI4_MOSI) | \
                             PIN_OSPEED_100M(GPIOE_PE15))
#define VAL_GPIOE_PUPDR     (PIN_PUPDR_FLOATING(GPIOE_PE0_TIM4_ETR) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE1) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE2) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE3) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE4) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE5_TIM9_CH1) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE6_TIM9_CH2) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE7_TIM1_ETR) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE8) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE9_TIM1_CH1) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE10) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE11_DAC_SPI4_NSS) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE12_DAC_SPI4_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE13_TIM1_CH3) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE14_DAC_SPI4_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOE_PE15))
#define VAL_GPIOE_ODR       (PIN_ODR_HIGH(GPIOE_PE0_TIM4_ETR) | \
                             PIN_ODR_LOW(GPIOE_PE1) | \
                             PIN_ODR_HIGH(GPIOE_PE2) | \
                             PIN_ODR_HIGH(GPIOE_PE3) | \
                             PIN_ODR_HIGH(GPIOE_PE4) | \
                             PIN_ODR_HIGH(GPIOE_PE5_TIM9_CH1) | \
                             PIN_ODR_HIGH(GPIOE_PE6_TIM9_CH2) | \
                             PIN_ODR_HIGH(GPIOE_PE7_TIM1_ETR) | \
                             PIN_ODR_HIGH(GPIOE_PE8) | \
                             PIN_ODR_HIGH(GPIOE_PE9_TIM1_CH1) | \
                             PIN_ODR_HIGH(GPIOE_PE10) | \
                             PIN_ODR_HIGH(GPIOE_PE11_DAC_SPI4_NSS) | \
                             PIN_ODR_HIGH(GPIOE_PE12_DAC_SPI4_SCK) | \
                             PIN_ODR_HIGH(GPIOE_PE13_TIM1_CH3) | \
                             PIN_ODR_HIGH(GPIOE_PE14_DAC_SPI4_MOSI) | \
                             PIN_ODR_HIGH(GPIOE_PE15))
#define VAL_GPIOE_AFRL      (PIN_AFIO_AF(GPIOE_PE0_TIM4_ETR, 0) | \
                             PIN_AFIO_AF(GPIOE_PE1, 0) | \
                             PIN_AFIO_AF(GPIOE_PE2, 0) | \
                             PIN_AFIO_AF(GPIOE_PE3, 0) | \
                             PIN_AFIO_AF(GPIOE_PE4, 0) | \
                             PIN_AFIO_AF(GPIOE_PE5_TIM9_CH1, 0) | \
                             PIN_AFIO_AF(GPIOE_PE6_TIM9_CH2, 0) | \
                             PIN_AFIO_AF(GPIOE_PE7_TIM1_ETR, 0))
#define VAL_GPIOE_AFRH      (PIN_AFIO_AF(GPIOE_PE8, 0) | \
                             PIN_AFIO_AF(GPIOE_PE9_TIM1_CH1, 0) | \
                             PIN_AFIO_AF(GPIOE_PE10, 0) | \
                             PIN_AFIO_AF(GPIOE_PE11_DAC_SPI4_NSS, 0) | \
                             PIN_AFIO_AF(GPIOE_PE12_DAC_SPI4_SCK, 5) | \
                             PIN_AFIO_AF(GPIOE_PE13_TIM1_CH3, 0) | \
                             PIN_AFIO_AF(GPIOE_PE14_DAC_SPI4_MOSI, 5) | \
                             PIN_AFIO_AF(GPIOE_PE15, 0))
/*
 * GPIOF setup:
 *
*/
#define VAL_GPIOF_MODER     (PIN_MODE_INPUT(GPIOF_PF0_I2C2_SDA) | \
                             PIN_MODE_INPUT(GPIOF_PF1_I2C2_SCL) | \
                             PIN_MODE_OUTPUT(GPIOF_PF2_CAN_SHDN) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH9_FLT) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH14_FLT) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH15_FLT) | \
                             PIN_MODE_INPUT(GPIOF_PF6) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH5_FLT) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH6_FLT) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH7_FLT) | \
                             PIN_MODE_ANALOG(GPIOF_ADC3_CH8_FLT) | \
                             PIN_MODE_INPUT(GPIOF_PF11) | \
                             PIN_MODE_INPUT(GPIOF_PF12) | \
                             PIN_MODE_INPUT(GPIOF_PF13) | \
                             PIN_MODE_INPUT(GPIOF_PF14) | \
                             PIN_MODE_INPUT(GPIOF_PF15))
#define VAL_GPIOF_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOF_PF0_I2C2_SDA) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF1_I2C2_SCL) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF2_CAN_SHDN) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH9_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH14_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH15_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF6) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH5_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH6_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH7_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_ADC3_CH8_FLT) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF11) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF12) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF13) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF14) | \
                             PIN_OTYPE_PUSHPULL(GPIOF_PF15))
#define VAL_GPIOF_OSPEEDR   (PIN_OSPEED_100M(GPIOF_PF0_I2C2_SDA) | \
                             PIN_OSPEED_100M(GPIOF_PF1_I2C2_SCL) | \
                             PIN_OSPEED_100M(GPIOF_PF2_CAN_SHDN) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH9_FLT) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH14_FLT) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH15_FLT) | \
                             PIN_OSPEED_100M(GPIOF_PF6) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH5_FLT) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH6_FLT) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH7_FLT) | \
                             PIN_OSPEED_100M(GPIOF_ADC3_CH8_FLT) | \
                             PIN_OSPEED_100M(GPIOF_PF11) | \
                             PIN_OSPEED_100M(GPIOF_PF12) | \
                             PIN_OSPEED_100M(GPIOF_PF13) | \
                             PIN_OSPEED_100M(GPIOF_PF14) | \
                             PIN_OSPEED_100M(GPIOF_PF15))
#define VAL_GPIOF_PUPDR     (PIN_PUPDR_FLOATING(GPIOF_PF0_I2C2_SDA) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF1_I2C2_SCL) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF2_CAN_SHDN) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH9_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH14_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH15_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF6) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH5_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH6_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH7_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_ADC3_CH8_FLT) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF11) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF12) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF13) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF14) | \
                             PIN_PUPDR_FLOATING(GPIOF_PF15))
#define VAL_GPIOF_ODR       (PIN_ODR_HIGH(GPIOF_PF0_I2C2_SDA) | \
                             PIN_ODR_HIGH(GPIOF_PF1_I2C2_SCL) | \
                             PIN_ODR_HIGH(GPIOF_PF2_CAN_SHDN) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH9_FLT) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH14_FLT) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH15_FLT) | \
                             PIN_ODR_HIGH(GPIOF_PF6) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH5_FLT) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH6_FLT) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH7_FLT) | \
                             PIN_ODR_HIGH(GPIOF_ADC3_CH8_FLT) | \
                             PIN_ODR_HIGH(GPIOF_PF11) | \
                             PIN_ODR_HIGH(GPIOF_PF12) | \
                             PIN_ODR_HIGH(GPIOF_PF13) | \
                             PIN_ODR_HIGH(GPIOF_PF14) | \
                             PIN_ODR_HIGH(GPIOF_PF15))
#define VAL_GPIOF_AFRL      (PIN_AFIO_AF(GPIOF_PF0_I2C2_SDA, 0) | \
                             PIN_AFIO_AF(GPIOF_PF1_I2C2_SCL, 0) | \
                             PIN_AFIO_AF(GPIOF_PF2_CAN_SHDN, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH9_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH14_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH15_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_PF6, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH5_FLT, 0))
#define VAL_GPIOF_AFRH      (PIN_AFIO_AF(GPIOF_ADC3_CH6_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH7_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_ADC3_CH8_FLT, 0) | \
                             PIN_AFIO_AF(GPIOF_PF11, 0) | \
                             PIN_AFIO_AF(GPIOF_PF12, 0) | \
                             PIN_AFIO_AF(GPIOF_PF13, 0) | \
                             PIN_AFIO_AF(GPIOF_PF14, 0) | \
                             PIN_AFIO_AF(GPIOF_PF15, 0))
/*
 * GPIOG setup:
 *
*/
#define VAL_GPIOG_MODER     (PIN_MODE_INPUT(GPIOG_PG0) | \
                             PIN_MODE_INPUT(GPIOG_PG1) | \
                             PIN_MODE_INPUT(GPIOG_PG2) | \
                             PIN_MODE_INPUT(GPIOG_PG3) | \
                             PIN_MODE_INPUT(GPIOG_PG4) | \
                             PIN_MODE_INPUT(GPIOG_PG5) | \
                             PIN_MODE_INPUT(GPIOG_PG6) | \
                             PIN_MODE_INPUT(GPIOG_PG7) | \
                             PIN_MODE_INPUT(GPIOG_PG8_SPI6_NSS) | \
                             PIN_MODE_INPUT(GPIOG_PG9) | \
                             PIN_MODE_INPUT(GPIOG_PG10) | \
                             PIN_MODE_INPUT(GPIOG_PG11) | \
                             PIN_MODE_INPUT(GPIOG_PG12_SPI6_MISO) | \
                             PIN_MODE_INPUT(GPIOG_PG13_SPI6_SCK) | \
                             PIN_MODE_INPUT(GPIOG_PG14_SPI6_MOSI) | \
                             PIN_MODE_INPUT(GPIOG_PG15))
#define VAL_GPIOG_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOG_PG0) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG1) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG2) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG3) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG4) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG5) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG6) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG7) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG8_SPI6_NSS) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG9) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG10) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG11) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG12_SPI6_MISO) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG13_SPI6_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG14_SPI6_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOG_PG15))
#define VAL_GPIOG_OSPEEDR   (PIN_OSPEED_100M(GPIOG_PG0) | \
                             PIN_OSPEED_100M(GPIOG_PG1) | \
                             PIN_OSPEED_100M(GPIOG_PG2) | \
                             PIN_OSPEED_100M(GPIOG_PG3) | \
                             PIN_OSPEED_100M(GPIOG_PG4) | \
                             PIN_OSPEED_100M(GPIOG_PG5) | \
                             PIN_OSPEED_100M(GPIOG_PG6) | \
                             PIN_OSPEED_100M(GPIOG_PG7) | \
                             PIN_OSPEED_100M(GPIOG_PG8_SPI6_NSS) | \
                             PIN_OSPEED_100M(GPIOG_PG9) | \
                             PIN_OSPEED_100M(GPIOG_PG10) | \
                             PIN_OSPEED_100M(GPIOG_PG11) | \
                             PIN_OSPEED_100M(GPIOG_PG12_SPI6_MISO) | \
                             PIN_OSPEED_100M(GPIOG_PG13_SPI6_SCK) | \
                             PIN_OSPEED_100M(GPIOG_PG14_SPI6_MOSI) | \
                             PIN_OSPEED_100M(GPIOG_PG15))
#define VAL_GPIOG_PUPDR     (PIN_PUPDR_FLOATING(GPIOG_PG0) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG1) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG2) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG3) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG4) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG5) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG6) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG7) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG8_SPI6_NSS) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG9) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG10) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG11) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG12_SPI6_MISO) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG13_SPI6_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG14_SPI6_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOG_PG15))
#define VAL_GPIOG_ODR       (PIN_ODR_HIGH(GPIOG_PG0) | \
                             PIN_ODR_HIGH(GPIOG_PG1) | \
                             PIN_ODR_HIGH(GPIOG_PG2) | \
                             PIN_ODR_HIGH(GPIOG_PG3) | \
                             PIN_ODR_HIGH(GPIOG_PG4) | \
                             PIN_ODR_HIGH(GPIOG_PG5) | \
                             PIN_ODR_HIGH(GPIOG_PG6) | \
                             PIN_ODR_HIGH(GPIOG_PG7) | \
                             PIN_ODR_HIGH(GPIOG_PG8_SPI6_NSS) | \
                             PIN_ODR_HIGH(GPIOG_PG9) | \
                             PIN_ODR_HIGH(GPIOG_PG10) | \
                             PIN_ODR_HIGH(GPIOG_PG11) | \
                             PIN_ODR_HIGH(GPIOG_PG12_SPI6_MISO) | \
                             PIN_ODR_HIGH(GPIOG_PG13_SPI6_SCK) | \
                             PIN_ODR_HIGH(GPIOG_PG14_SPI6_MOSI) | \
                             PIN_ODR_HIGH(GPIOG_PG15))
#define VAL_GPIOG_AFRL      (PIN_AFIO_AF(GPIOG_PG0, 0) | \
                             PIN_AFIO_AF(GPIOG_PG1, 0) | \
                             PIN_AFIO_AF(GPIOG_PG2, 0) | \
                             PIN_AFIO_AF(GPIOG_PG3, 0) | \
                             PIN_AFIO_AF(GPIOG_PG4, 0) | \
                             PIN_AFIO_AF(GPIOG_PG5, 0) | \
                             PIN_AFIO_AF(GPIOG_PG6, 0) | \
                             PIN_AFIO_AF(GPIOG_PG7, 0))
#define VAL_GPIOG_AFRH      (PIN_AFIO_AF(GPIOG_PG8_SPI6_NSS, 0) | \
                             PIN_AFIO_AF(GPIOG_PG9, 0) | \
                             PIN_AFIO_AF(GPIOG_PG10, 0) | \
                             PIN_AFIO_AF(GPIOG_PG11, 0) | \
                             PIN_AFIO_AF(GPIOG_PG12_SPI6_MISO, 0) | \
                             PIN_AFIO_AF(GPIOG_PG13_SPI6_SCK, 0) | \
                             PIN_AFIO_AF(GPIOG_PG14_SPI6_MOSI, 0) | \
                             PIN_AFIO_AF(GPIOG_PG15, 0))
/*
 * GPIOH setup:
 *
*/
#define VAL_GPIOH_MODER     (PIN_MODE_ALTERNATE(GPIOH_OSC_IN) | \
                             PIN_MODE_ALTERNATE(GPIOH_OSC_OUT) | \
                             PIN_MODE_INPUT(GPIOH_PH2) | \
                             PIN_MODE_INPUT(GPIOH_PH3) | \
                             PIN_MODE_ALTERNATE(GPIOH_ULPI_NXT) | \
                             PIN_MODE_INPUT(GPIOH_PH5) | \
                             PIN_MODE_INPUT(GPIOH_PH6) | \
                             PIN_MODE_ANALOG(GPIOH_PH7_ADC3_CH4) | \
                             PIN_MODE_ANALOG(GPIOH_PH8_ADC3_CH8) | \
                             PIN_MODE_INPUT(GPIOH_PH9) | \
                             PIN_MODE_INPUT(GPIOH_PH10_TIM5_CH1) | \
                             PIN_MODE_INPUT(GPIOH_PH11_TIM5_CH2) | \
                             PIN_MODE_INPUT(GPIOH_PH12_TIM5_CH3) | \
                             PIN_MODE_INPUT(GPIOH_PH13_CAN1_TX) | \
                             PIN_MODE_INPUT(GPIOH_PH14) | \
                             PIN_MODE_INPUT(GPIOH_PH15))
#define VAL_GPIOH_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH2) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH3) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_ULPI_NXT) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH5) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH6) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH7_ADC3_CH4) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH8_ADC3_CH8) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH9) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH10_TIM5_CH1) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH11_TIM5_CH2) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH12_TIM5_CH3) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH13_CAN1_TX) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH14) | \
                             PIN_OTYPE_PUSHPULL(GPIOH_PH15))
#define VAL_GPIOH_OSPEEDR   (PIN_OSPEED_100M(GPIOH_OSC_IN) | \
                             PIN_OSPEED_100M(GPIOH_OSC_OUT) | \
                             PIN_OSPEED_100M(GPIOH_PH2) | \
                             PIN_OSPEED_100M(GPIOH_PH3) | \
                             PIN_OSPEED_100M(GPIOH_ULPI_NXT) | \
                             PIN_OSPEED_100M(GPIOH_PH5) | \
                             PIN_OSPEED_100M(GPIOH_PH6) | \
                             PIN_OSPEED_100M(GPIOH_PH7_ADC3_CH4) | \
                             PIN_OSPEED_100M(GPIOH_PH8_ADC3_CH8) | \
                             PIN_OSPEED_100M(GPIOH_PH9) | \
                             PIN_OSPEED_100M(GPIOH_PH10_TIM5_CH1) | \
                             PIN_OSPEED_100M(GPIOH_PH11_TIM5_CH2) | \
                             PIN_OSPEED_100M(GPIOH_PH12_TIM5_CH3) | \
                             PIN_OSPEED_100M(GPIOH_PH13_CAN1_TX) | \
                             PIN_OSPEED_100M(GPIOH_PH14) | \
                             PIN_OSPEED_100M(GPIOH_PH15))
#define VAL_GPIOH_PUPDR     (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) | \
                             PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH2) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH3) | \
                             PIN_PUPDR_FLOATING(GPIOH_ULPI_NXT) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH5) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH6) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH7_ADC3_CH4) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH8_ADC3_CH8) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH9) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH10_TIM5_CH1) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH11_TIM5_CH2) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH12_TIM5_CH3) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH13_CAN1_TX) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH14) | \
                             PIN_PUPDR_FLOATING(GPIOH_PH15))
#define VAL_GPIOH_ODR       (PIN_ODR_HIGH(GPIOH_OSC_IN) | \
                             PIN_ODR_HIGH(GPIOH_OSC_OUT) | \
                             PIN_ODR_HIGH(GPIOH_PH2) | \
                             PIN_ODR_HIGH(GPIOH_PH3) | \
                             PIN_ODR_HIGH(GPIOH_ULPI_NXT) | \
                             PIN_ODR_HIGH(GPIOH_PH5) | \
                             PIN_ODR_HIGH(GPIOH_PH6) | \
                             PIN_ODR_HIGH(GPIOH_PH7_ADC3_CH4) | \
                             PIN_ODR_HIGH(GPIOH_PH8_ADC3_CH8) | \
                             PIN_ODR_HIGH(GPIOH_PH9) | \
                             PIN_ODR_HIGH(GPIOH_PH10_TIM5_CH1) | \
                             PIN_ODR_HIGH(GPIOH_PH11_TIM5_CH2) | \
                             PIN_ODR_HIGH(GPIOH_PH12_TIM5_CH3) | \
                             PIN_ODR_HIGH(GPIOH_PH13_CAN1_TX) | \
                             PIN_ODR_HIGH(GPIOH_PH14) | \
                             PIN_ODR_HIGH(GPIOH_PH15))
#define VAL_GPIOH_AFRL      (PIN_AFIO_AF(GPIOH_OSC_IN, 0) | \
                             PIN_AFIO_AF(GPIOH_OSC_OUT, 0) | \
                             PIN_AFIO_AF(GPIOH_PH2, 0) | \
                             PIN_AFIO_AF(GPIOH_PH3, 0) | \
                             PIN_AFIO_AF(GPIOH_ULPI_NXT, 10) | \
                             PIN_AFIO_AF(GPIOH_PH5, 0) | \
                             PIN_AFIO_AF(GPIOH_PH6, 0) | \
                             PIN_AFIO_AF(GPIOH_PH7_ADC3_CH4, 0))
#define VAL_GPIOH_AFRH      (PIN_AFIO_AF(GPIOH_PH8_ADC3_CH8, 0) | \
                             PIN_AFIO_AF(GPIOH_PH9, 0) | \
                             PIN_AFIO_AF(GPIOH_PH10_TIM5_CH1, 0) | \
                             PIN_AFIO_AF(GPIOH_PH11_TIM5_CH2, 0) | \
                             PIN_AFIO_AF(GPIOH_PH12_TIM5_CH3, 0) | \
                             PIN_AFIO_AF(GPIOH_PH13_CAN1_TX, 0) | \
                             PIN_AFIO_AF(GPIOH_PH14, 0) | \
                             PIN_AFIO_AF(GPIOH_PH15, 0))
/*
 * GPIOI setup:
 *
*/
#define VAL_GPIOI_MODER     (PIN_MODE_INPUT(GPIOI_PI0_SPI2_NSS) | \
                             PIN_MODE_INPUT(GPIOI_PI1_SPI2_SCK) | \
                             PIN_MODE_INPUT(GPIOI_PI2_SPI2_MISO) | \
                             PIN_MODE_INPUT(GPIOI_PI3_SPI2_MOSI) | \
                             PIN_MODE_INPUT(GPIOI_PI4) | \
                             PIN_MODE_OUTPUT(GPIOI_PI5_MBUS_ANA_SELECT) | \
                             PIN_MODE_OUTPUT(GPIOI_PI6_MBUS_SER_SELECT) | \
                             PIN_MODE_OUTPUT(GPIOI_PI7_MBUS_GPIO_SELECT) | \
                             PIN_MODE_INPUT(GPIOI_PI8) | \
                             PIN_MODE_INPUT(GPIOI_PI9_CAN1_RX) | \
                             PIN_MODE_INPUT(GPIOI_PI10) | \
                             PIN_MODE_INPUT(GPIOI_PI11) | \
                             PIN_MODE_INPUT(GPIOI_PIN_12) | \
                             PIN_MODE_INPUT(GPIOI_PIN_13) | \
                             PIN_MODE_INPUT(GPIOI_PIN_14) | \
                             PIN_MODE_INPUT(GPIOI_PIN_15))
#define VAL_GPIOI_OTYPER    (PIN_OTYPE_PUSHPULL(GPIOI_PI0_SPI2_NSS) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI1_SPI2_SCK) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI2_SPI2_MISO) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI3_SPI2_MOSI) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI4) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI5_MBUS_ANA_SELECT) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI6_MBUS_SER_SELECT) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI7_MBUS_GPIO_SELECT) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI8) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI9_CAN1_RX) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI10) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PI11) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN_12) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN_13) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN_14) | \
                             PIN_OTYPE_PUSHPULL(GPIOI_PIN_15))
#define VAL_GPIOI_OSPEEDR   (PIN_OSPEED_100M(GPIOI_PI0_SPI2_NSS) | \
                             PIN_OSPEED_100M(GPIOI_PI1_SPI2_SCK) | \
                             PIN_OSPEED_100M(GPIOI_PI2_SPI2_MISO) | \
                             PIN_OSPEED_100M(GPIOI_PI3_SPI2_MOSI) | \
                             PIN_OSPEED_100M(GPIOI_PI4) | \
                             PIN_OSPEED_100M(GPIOI_PI5_MBUS_ANA_SELECT) | \
                             PIN_OSPEED_100M(GPIOI_PI6_MBUS_SER_SELECT) | \
                             PIN_OSPEED_100M(GPIOI_PI7_MBUS_GPIO_SELECT) | \
                             PIN_OSPEED_100M(GPIOI_PI8) | \
                             PIN_OSPEED_100M(GPIOI_PI9_CAN1_RX) | \
                             PIN_OSPEED_100M(GPIOI_PI10) | \
                             PIN_OSPEED_100M(GPIOI_PI11) | \
                             PIN_OSPEED_100M(GPIOI_PIN_12) | \
                             PIN_OSPEED_100M(GPIOI_PIN_13) | \
                             PIN_OSPEED_100M(GPIOI_PIN_14) | \
                             PIN_OSPEED_100M(GPIOI_PIN_15))
#define VAL_GPIOI_PUPDR     (PIN_PUPDR_FLOATING(GPIOI_PI0_SPI2_NSS) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI1_SPI2_SCK) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI2_SPI2_MISO) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI3_SPI2_MOSI) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI4) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI5_MBUS_ANA_SELECT) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI6_MBUS_SER_SELECT) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI7_MBUS_GPIO_SELECT) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI8) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI9_CAN1_RX) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI10) | \
                             PIN_PUPDR_FLOATING(GPIOI_PI11) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN_12) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN_13) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN_14) | \
                             PIN_PUPDR_FLOATING(GPIOI_PIN_15))
#define VAL_GPIOI_ODR       (PIN_ODR_HIGH(GPIOI_PI0_SPI2_NSS) | \
                             PIN_ODR_HIGH(GPIOI_PI1_SPI2_SCK) | \
                             PIN_ODR_HIGH(GPIOI_PI2_SPI2_MISO) | \
                             PIN_ODR_HIGH(GPIOI_PI3_SPI2_MOSI) | \
                             PIN_ODR_HIGH(GPIOI_PI4) | \
                             PIN_ODR_HIGH(GPIOI_PI5_MBUS_ANA_SELECT) | \
                             PIN_ODR_HIGH(GPIOI_PI6_MBUS_SER_SELECT) | \
                             PIN_ODR_HIGH(GPIOI_PI7_MBUS_GPIO_SELECT) | \
                             PIN_ODR_HIGH(GPIOI_PI8) | \
                             PIN_ODR_HIGH(GPIOI_PI9_CAN1_RX) | \
                             PIN_ODR_HIGH(GPIOI_PI10) | \
                             PIN_ODR_HIGH(GPIOI_PI11) | \
                             PIN_ODR_HIGH(GPIOI_PIN_12) | \
                             PIN_ODR_HIGH(GPIOI_PIN_13) | \
                             PIN_ODR_HIGH(GPIOI_PIN_14) | \
                             PIN_ODR_HIGH(GPIOI_PIN_15))
#define VAL_GPIOI_AFRL      (PIN_AFIO_AF(GPIOI_PI0_SPI2_NSS, 0) | \
                             PIN_AFIO_AF(GPIOI_PI1_SPI2_SCK, 0) | \
                             PIN_AFIO_AF(GPIOI_PI2_SPI2_MISO, 0) | \
                             PIN_AFIO_AF(GPIOI_PI3_SPI2_MOSI, 0) | \
                             PIN_AFIO_AF(GPIOI_PI4, 0) | \
                             PIN_AFIO_AF(GPIOI_PI5_MBUS_ANA_SELECT, 0) | \
                             PIN_AFIO_AF(GPIOI_PI6_MBUS_SER_SELECT, 0) | \
                             PIN_AFIO_AF(GPIOI_PI7_MBUS_GPIO_SELECT, 0))
#define VAL_GPIOI_AFRH      (PIN_AFIO_AF(GPIOI_PI8, 0) | \
                             PIN_AFIO_AF(GPIOI_PI9_CAN1_RX, 0) | \
                             PIN_AFIO_AF(GPIOI_PI10, 0) | \
                             PIN_AFIO_AF(GPIOI_PI11, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN_12, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN_13, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN_14, 0) | \
                             PIN_AFIO_AF(GPIOI_PIN_15, 0))


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

