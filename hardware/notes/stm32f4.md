
# Notes on stm32f4

* Tue 15 July 2014 10:59:40 (PDT)
  * See stm32f4 data sheet

*Electrical characteristics for ADC are in Data sheet for stm32f407 See tables 67 and 68 p 129 in Doc ID 022152 Rev4 (June 2013)*

* t_stab (power up time) is 2-3 uS

* Mon 14 July 2014 14:01:56 (PDT)
  * See .../STM32F4xx/adc_lld.c

Sampling the internal temp sensor and internal ref voltage

```
/**
 * @brief   Enables the TSVREFE bit.
 * @details The TSVREFE bit is required in order to sample the internal
 *          temperature sensor and internal reference voltage.
 * @note    This is an STM32-only functionality.
 */
void adcSTM32EnableTSVREFE(void) {

  ADC->CCR |= ADC_CCR_TSVREFE;
}

/**
 * @brief   Disables the TSVREFE bit.
 * @details The TSVREFE bit is required in order to sample the internal
 *          temperature sensor and internal reference voltage.
 * @note    This is an STM32-only functionality.
 */
void adcSTM32DisableTSVREFE(void) {

  ADC->CCR &= ~ADC_CCR_TSVREFE;
}
```


