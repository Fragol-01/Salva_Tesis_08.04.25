/*
 * tim.c
 *
 *  Created on: 17 mar. 2025
 *      Author: DANNY
 */

#include <tim.h>

/**
 * @Brief TIM6 as ADC MIC
 * Generate a 48kHz Sample Rate
 */
void tim_TIM6_MIC_config(void)
{
	//We are working with a SYSTEM Source of 16MHz
	//Enable TIM6 clock
	RCC->APB1ENR1 |=(RCC_APB1ENR1_TIM6EN);
	//Periodic
	TIM6->CR1 &=~ (TIM_CR1_OPM);
	//Mode-->TRGO
	TIM6->CR2 &=~ (TIM_CR2_MMS);
	TIM6->CR2 |= (TIM_CR2_MMS_1);
	//f=44.1kHz
	TIM6->PSC = 0;//Keep APB clock Frequency
	//Period
	TIM6->ARR=750-1;//72MHz/750= 96kHz
	//Update register on event
	TIM6->EGR=1;
//	//Start timer
//	TIM6->CR1 |= (TIM_CR1_CEN);
}
