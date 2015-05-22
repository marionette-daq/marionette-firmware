/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef FETCH_FETCH_SPI_H_
#define FETCH_FETCH_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif


bool dac124_config_cmd(void);
bool dac124_write_cmd(int dac_channel,int dac_output_type, uint32_t tx_data);
bool dac124_reset_cmd(void);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
