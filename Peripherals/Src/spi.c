/*
 * spi.c
 *
 *  Created on: 16 mar. 2025
 *      Author: DANNY
 */
#include <spi.h>

SPI_HandleTypeDef hspi1;

/**
 * @Brief SPI1 GPIO pins configuration
 * (PA5(A4)->SP1_SCK,PA6(A5)->SP1_MISO,PA7(A6)->SP1_MOSI)
 * (PA4(A3)->CS)
 */
void spi_GPIO_config(void)
{
	//SPI AF GPIO pins configuration
	GPIO_Handle_t spiPins;
	spiPins.pGPIOx=GPIOA;
	spiPins.pinMode=PIN_MODE_ALTFN;
	spiPins.pinOutputType=PIN_OP_TYPE_PP;
	spiPins.pinPUPDControl=PIN_NO_PUPD;
	spiPins.pinSpeed=PIN_SPEED_HIGH;
	spiPins.pinAltFunMode=AF5;
	spiPins.pinNumber=PIN_NO_7;//MOSI
	gpio_Init(&spiPins);
	spiPins.pinNumber=PIN_NO_6;//MISO
	spiPins.pinPUPDControl=PIN_PULL_UP;
	gpio_Init(&spiPins);
	spiPins.pinNumber=PIN_NO_5;//SCLK
	spiPins.pinPUPDControl=PIN_NO_PUPD;
	gpio_Init(&spiPins);
	spiPins.pinNumber=PIN_NO_4;//CS
	spiPins.pinMode=PIN_MODE_OUTPUT;
	gpio_Init(&spiPins);
	//Default value of CS is HIGH
	GPIOA->BSRR |= (GPIO_BSRR_BS4);
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @Brief SPI1 transmit
 */
bool spi_transmit(uint8_t *pointer_data,uint8_t len,uint32_t timeout)
{
	//Enable SPI1(if not)
	SPI1->CR1 |= (SPI_CR1_SPE);
	//Timeout initial ticks
	uint8_t dataIdx = 0;
	uint32_t startTick = HAL_GetTick();
	//While loop,TX data managing timeout
	while(dataIdx<len)
	{
		if(SPI1->SR & SPI_SR_TXE) //Tx buffer empty
		{
			SPI1->DR = pointer_data[dataIdx];
			dataIdx++;
		}
		else //Manage timeout
		{
			if((HAL_GetTick() - startTick)>= timeout) return false;
		}
	}
	//Wait for busy flag
	while((SPI1->SR & SPI_SR_BSY))
	{
		if((HAL_GetTick() - startTick)>= timeout) return false;
	}
	//Clear overrun conditions
	volatile uint32_t tempRead = SPI1->DR;
	tempRead = SPI1->SR;
	(void)tempRead ;
	return true;

}

/**
 * @Brief SPI1 receive
 */
bool spi_receive(uint8_t *pointer_data,uint8_t len,uint32_t timeout)
{
	//Enable SPI1(if not)
	SPI1->CR1 |= (SPI_CR1_SPE);
	//Timeout initial ticks
	uint8_t dataIdx = 0;
	uint32_t startTick = HAL_GetTick();
	bool isTransmit=1;
	//While loop: TX first, then RX, managing timeout
	while(dataIdx<len)
	{
		//TX dummy data
		if((SPI1->SR & SPI_SR_TXE) && isTransmit) //Tx buffer empty
		{
			SPI1->DR = 0xFF;
			isTransmit=false;
		}
		if (SPI1->SR & SPI_SR_RXNE)
		{
			pointer_data[dataIdx] = SPI1->DR;
			dataIdx++;
			isTransmit=true;

		}
		else //Manage timeout
		{
			if((HAL_GetTick() - startTick)>= timeout) return false;
		}
	}
	//Wait for busy flag
	while((SPI1->SR & SPI_SR_BSY))
	{
		if((HAL_GetTick() - startTick)>= timeout) return false;
	}
	//Clear overrun conditions
	volatile uint32_t tempRead = SPI1->DR;
	tempRead = SPI1->SR;
	(void)tempRead ;
	return true;

}

/**
 * @Brief SPI1 transmit/receive
 */
bool spi_transmitReceive(uint8_t *RX_buffer,uint8_t *TX_buffer,uint8_t len,uint32_t timeout)
{
	//Enable SPI1(if not)
	SPI1->CR1 |= (SPI_CR1_SPE);
	//Timeout initial ticks
	uint8_t dataIdx = 0;
	uint32_t startTick = HAL_GetTick();
	uint32_t actualTick;
	bool isTransmit=1;
	//While loop: TX first, then RX, managing timeout
	while(dataIdx<len)
	{
		//TX data
		if((SPI1->SR & SPI_SR_TXE) && isTransmit) //Tx buffer empty
		{
			SPI1->DR = TX_buffer[dataIdx];
			isTransmit=false;
		}
		//RX data
		if (SPI1->SR & SPI_SR_RXNE)
		{
			RX_buffer[dataIdx] = SPI1->DR;
			dataIdx++;
			isTransmit=true;

		}
		else //Manage timeout
		{
			actualTick=HAL_GetTick();
			if((actualTick - startTick)>= timeout) return false;
		}
	}
	//Wait for busy flag
	while((SPI1->SR & SPI_SR_BSY))
	{
		if((HAL_GetTick() - startTick)>= timeout) return false;
	}
	//Clear overrun conditions
	volatile uint32_t tempRead = SPI1->DR;
	tempRead = SPI1->SR;
	(void)tempRead ;
	return true;

}


/**
 * @Brief SD Chip Select Set/Reset
 */
void spi_cs_sd_write(bool state)
{
	if (state)
	{
		GPIOA->BSRR |= (GPIO_BSRR_BS4);
	}
	else
	{
		GPIOA->BSRR |= (GPIO_BSRR_BR4);
	}
}
