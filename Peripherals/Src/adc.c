/*
 * adc.c
 *
 *  Created on: 17 mar. 2025
 *      Author: DANNY
 */
#include <adc.h>



/**
 * @Brief ADC MIC(with OPAMP output as input) configuration
 * PA3(A2)->ADC12_IN8 is the OPAMP output
 */
void adc_MIC_config(void)
{
	//Enable ADC clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_ADCEN);
	//Select HCLK/1 as ADC clock source
	ADC1_COMMON->CCR &=~(ADC_CCR_CKMODE);
	ADC1_COMMON->CCR |= (ADC_CCR_CKMODE_0);
	//Exit the Deep-power-down mode
	ADC1->CR &=~ (ADC_CR_DEEPPWD);
	//Enable the ADC internal voltage regulator
	ADC1->CR |= (ADC_CR_ADVREGEN);
	//Wait for the startup time of the ADC voltage regulator
	adc_ADCVREG_STUP_delay();
	////ADC Calibration
	//Single-Ended Input mode calibration
	ADC1->CR &=~ (ADC_CR_ADCALDIF);
	//Calibration start
	ADC1->CR |= (ADC_CR_ADCAL);
	//Wait until calibration is stopped
	while (ADC1->CR & ADC_CR_ADCAL);
	//Right Align Mode
	ADC1->CFGR &=~ (ADC_CFGR_ALIGN);
	//Single Conversion Mode
	ADC1->CFGR &=~ (ADC_CFGR_CONT);
	//-----Sample time to 24.5 ADC CLK Cycles-----//
	//Tconv= (24.5+12.5) ADC CLK Cycles = 37 ADC clock cycles = 513.88ns
	//Total Tconv= Tconv *4= 2.055us(with N=4 oversampling)
	//Oversampled max frequency=486.49kHz
	ADC1->SMPR1 &=~ (ADC_SMPR1_SMP8);
	ADC1->SMPR1 |= (0x3UL << ADC_SMPR1_SMP8_Pos);
	//Single-channel mode
	ADC1->SQR1 &=~(ADC_SQR1_L);
	ADC1->SQR1  &=~ (ADC_SQR1_SQ1);
	ADC1->SQR1  |= (0x8UL << (6U));
	//----------------Oversampling Code-----------------------------//
	//Enable Regular Oversampled Mode
	ADC1->CFGR2 |= (ADC_CFGR2_ROVSE);
	//Oversampling Ratio to 4 (no bit shifting)
	ADC1->CFGR2 &=~ (ADC_CFGR2_OVSR);
	ADC1->CFGR2 |= (ADC_CFGR2_OVSR_0);
	//-------------------End of Oversampling Code------------------//
	//TIM6 TRGO Trigger
	ADC1->CFGR &=~ (ADC_CFGR_EXTEN);
	ADC1->CFGR |= (ADC_CFGR_EXTEN_0);
	ADC1->CFGR &=~ (ADC_CFGR_EXTSEL);
	ADC1->CFGR |= (0xDUL << ADC_CFGR_EXTSEL_Pos);//Event 13->TIM6_TRGO selected
	//DMA Circular Mode
	ADC1->CFGR |= (ADC_CFGR_DMACFG);
	//Enable DMA-ADC registers
	ADC1->CFGR |= (ADC_CFGR_DMAEN);
	//Clear the ADC ready flag(It is cleared by writing 1 to it.)
	ADC1->ISR |=(ADC_ISR_ADRDY);
	//Enable ADC
	ADC1->CR |=(ADC_CR_ADEN);
	//Wait until ADC is ready
	while (!(ADC1->CR & ADC_CR_ADEN));
	//Clear the ADC ready flag again
	ADC1->ISR |=(ADC_ISR_ADRDY);
	//ADC begin
	ADC1->CR |=(ADC_CR_ADSTART);
}

/**
 * @Brief ADC MIC DMA configuration
 */
void adc_MIC_DMA_config(uint16_t *buffer, uint16_t size) {
    // Habilitar reloj para DMA1
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    
    // Desactivar DMA1 Channel 1
    DMA1_Channel1->CCR &= ~(DMA_CCR_EN);
    
    // Esperar a que DMA esté desactivado
    while(DMA1_Channel1->CCR & DMA_CCR_EN);
    
    // Configurar la dirección de periférico (registro de datos del ADC)
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    
    // Configurar la dirección de memoria (buffer)
    DMA1_Channel1->CMAR = (uint32_t)buffer;
    
    // Configurar cantidad de datos a transferir (la mitad porque ahora cada valor ocupa 4 bytes)
    DMA1_Channel1->CNDTR = size / 2;
    
    // Configurar DMA con la nueva configuración
    DMA1_Channel1->CCR = 0;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0  |  // Periférico: Half-word (16 bits)
                          DMA_CCR_MSIZE_1  |  // Memoria: Word (32 bits) - CAMBIO AQUÍ
                          DMA_CCR_MINC     |  // Incremento automático de dirección de memoria
                          DMA_CCR_CIRC     |  // Modo circular
                          DMA_CCR_PL_1     |  // Alta prioridad
                          DMA_CCR_HTIE     |  // Habilitar interrupción de media transferencia
                          DMA_CCR_TCIE;       // Habilitar interrupción de transferencia completa
    
    // Habilitar interrupción DMA en NVIC
    NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    
    // Activar DMA
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}

/*
 * @brief delay for ADC voltage regulator start-up time(20us)
 * A 80Mhz->CM=12.5ns->CM x 2000=25us>20us
 */
void adc_ADCVREG_STUP_delay(void)
{
	volatile uint32_t i;
	for (i=0;i<2000;i++) {}
}

