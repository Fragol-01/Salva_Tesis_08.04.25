/*
 * raw_test.c
 *
 *  Created on: 20 mar. 2025
 *      Author: DANNY
 */

#include <main.h>
#include <gpio.h>
#include <spi.h>
#include <exti.h>
#include "uart.h"
#include "fatfs_sd.h"
#include "audio_recorder.h"
#include "write_sector_0.h"

uint32_t systemClock=76000000;
volatile bool exti_flag = false;
volatile bool isFileCreated=false;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void write_sector_test(void);
bool configurar_sector_base(uint8_t frecuencia_khz, uint16_t sectores_por_fichero, uint16_t num_ficheros);


int main (void)
{
	//----------Setup------------------------------//
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	//GPIO LED Config
	gpio_LED_config();
	//USART COnfig
	uart_UART2_GPIO_config();
	uart_UART2_config(systemClock);
	//SPI CONFIG
	spi_GPIO_config();
	MX_SPI1_Init();

	// Configurar el botón con interrupción
	exti_buttonConfig();  // Configuracion del boton (PA12->D2)

	//Mount SD Card
	bool sd_mounted = SD_disk_initialize(0);

	if (sd_mounted != 0)
	{
		printf("Failed to mount SD card\r\n");
	}
	else
	{
		printf("Successfully Mounted SD card\r\n");

		// Configurar el sector base
		uint8_t frecuencia_khz = 48;                 // 48 kHz
		uint16_t sectores_por_fichero = 45000;       // 45000 sectores para 2 minutos a 48 kHz
		uint16_t num_ficheros = 0;                   // Se calculará automáticamente según el espacio disponible

		if (configurar_sector_base(frecuencia_khz, sectores_por_fichero, num_ficheros)) {
			leer_sector_base();

			// Inicializar el grabador de audio
			if (audio_recorder_init()) {
				printf("Presione el boton para iniciar la grabacion\r\n");

				bool recording = false;

				while(1) {
					// Verificar si el botón fue presionado
					if (exti_flag) {
						exti_flag = false; // Resetear la bandera

						if (!recording && !memoriaLlena) {
							// Comenzar grabación
							recording = true;
							printf("Grabacion iniciada...\n\rPresione el boton para detener\r\n");
							audio_recorder_start();
						} else if (recording) {
							// Detener grabación
							recording = false;
							printf("Grabacion detenida por usuario\r\n");
							audio_recorder_stop();
						} else if (memoriaLlena) {
							printf("No se puede iniciar grabacion: memoria SD llena\r\n");
							HAL_Delay(500);
						}
					}

					// Verificar si la grabación se detuvo automáticamente
					if (recording && audio_recorder_get_state() == RECORDER_STOPPED) {
						recording = false;
						printf("Grabacion detenida automaticamente (limite alcanzado)\r\n");
					}

					// Procesar grabación (ahora es seguro llamar esto repetidamente)
					if (recording) {
						audio_recorder_process();
					}
                    else {
						// Parpadeo lento cuando no está grabando
						gpio_LED_toggleGreen();
						HAL_Delay(500);
					}
				}
			}
		}
	}

	return 0;
}


/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 38;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */


void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

// Implementar el manejador de interrupción para el botón con anti-rebote
void EXTI15_10_IRQHandler(void)
{
    // Limpiar flags de interrupción pendientes
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    EXTI->PR1 |= (EXTI_PR1_PIF12);

    // Pequeño retardo para anti-rebote
    for(volatile uint32_t i = 0; i < 500000; i++) {
        __NOP(); // No Operation - previene optimización del bucle
    }

    // Activar la bandera para que el bucle principal la detecte
    exti_flag = true;
}
