/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_SPI_H_
#define FETCH_FETCH_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_spi_init(void);
bool fetch_spi_reset(BaseSequentialStream * chp);

bool fetch_spi_config_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_spi_exchange_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_spi_reset_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_spi_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_spi_clock_div_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
