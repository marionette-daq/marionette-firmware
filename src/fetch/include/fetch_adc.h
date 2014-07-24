/*! \file fetch_adc.h
 *
 * @addtogroup fetch_adc
 * @{
 */

#ifndef FETCH_ADC_H_
#define FETCH_ADC_H_

#include <stdbool.h>

typedef struct adc_input {
    ioportid_t port;
    uint16_t   pin;
} ADC_input;
/* Get ADC1 working first. */
/*
const struct ADC_input ADC2_IN0;
const struct ADC_input ADC2_IN1;
const struct ADC_input ADC2_IN2;
const struct ADC_input ADC2_IN3;
const struct ADC_input ADC2_IN4;
const struct ADC_input ADC2_IN5;
const struct ADC_input ADC2_IN6;
const struct ADC_input ADC2_IN7;
const struct ADC_input ADC2_IN8;
const struct ADC_input ADC2_IN9;
const struct ADC_input ADC2_IN10;
const struct ADC_input ADC2_IN11;
const struct ADC_input ADC2_IN12;
const struct ADC_input ADC2_IN13;
const struct ADC_input ADC2_IN14;
const struct ADC_input ADC2_IN15;

const struct ADC_input ADC3_IN0;
const struct ADC_input ADC3_IN1;
const struct ADC_input ADC3_IN2;
const struct ADC_input ADC3_IN3;
const struct ADC_input ADC3_IN4;
const struct ADC_input ADC3_IN5;
const struct ADC_input ADC3_IN6;
const struct ADC_input ADC3_IN7;
const struct ADC_input ADC3_IN8;
const struct ADC_input ADC3_IN9;
const struct ADC_input ADC3_IN10;
const struct ADC_input ADC3_IN11;
const struct ADC_input ADC3_IN12;
const struct ADC_input ADC3_IN13;
const struct ADC_input ADC3_IN14;
const struct ADC_input ADC3_IN15;
*/

bool fetch_adc_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[],
                         Fetch_terminals * fetch_terms);

#endif

/*! @} */
