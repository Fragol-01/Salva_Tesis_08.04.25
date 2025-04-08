/*
 * spi.h
 *
 *  Created on: 16 mar. 2025
 *      Author: DANNY
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_
#include <main.h>
#include <gpio.h>


/**
 * @Brief SPI1 GPIO pins configuration
 * (PA5(A4)->SP1_SCK,PA6(A5)->SP1_MISO,PA7(A6)->SP1_MOSI)
 * (PA4(A3)->CS)
 */

void spi_GPIO_config(void);
/*
 * @brief SPI init using HAL
 */
void MX_SPI1_Init(void);

/**
 * @Brief SPI1 transmit
 */
bool spi_transmit(uint8_t *pointer_data,uint8_t len,uint32_t timeout);

/**
 * @Brief SPI1 receive
 */
bool spi_receive(uint8_t *pointer_data,uint8_t len,uint32_t timeout);

/**
 * @Brief SPI1 transmit/receive
 */
bool spi_transmitReceive(uint8_t *RX_buffer,uint8_t *TX_buffer,uint8_t len,uint32_t timeout);

/**
 * @Brief SD Chip Select Set/Reset
 */
void spi_cs_sd_write(bool state);


#endif /* INC_SPI_H_ */
