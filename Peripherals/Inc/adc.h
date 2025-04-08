  /*
 * adc.h
 *
 *  Created on: 17 mar. 2025
 *      Author: DANNY
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <main.h>


/**
 * @Brief ADC MIC(with OPAMP output as input) configuration
 */
void adc_MIC_config(void);

/**
 * @Brief ADC MIC DMA configuration
 */
void adc_MIC_DMA_config(uint16_t *buffer, uint16_t size);
/*
 * @brief delay for ADC voltage regulator start-up time(20us)
 * A 80Mhz->CM=12.5ns->CM x 2000=25us>20us
 */
void adc_ADCVREG_STUP_delay(void);


#endif /* INC_ADC_H_ */



