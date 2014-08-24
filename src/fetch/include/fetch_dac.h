/*! \file fetch_dac.h
 *
 * @addtogroup fetch_dac
 * @{
 */

#ifndef FETCH_DAC_H_
#define FETCH_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dac.h"
		
typedef enum dac_channel_io
{
	DAC_CH1 = 0,
	DAC_CH2
} DACChannel;

typedef struct dac_output {
    const ioportid_t port;
    const uint32_t   pad;
} DAC_output;

typedef struct fetch_dac_state
{
	int32_t                     vref_mv;
	int32_t                     dc_mv_ch1;	
	int32_t                     dc_mv_ch2;	
	bool                        init;
	BaseSequentialStream    *   chp;
	DACConfig               *   config;
} FETCH_dac_state;


typedef enum FETCH_DAC_token
{
	DAC_CMD = 0,
	DAC_SCA,
	DAC_CONFIGURE,
	NO_DAC_GPIO_TOKEN=FETCH_MAX_TERMINALS-1
} FETCH_DAC_token;

bool fetch_dac_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[], Fetch_terminals * fetch_terms);


#ifdef __cplusplus
}
#endif

#endif


//! @}


