///*
// * delete_sectors.c
// *
// *  Created on: 23 mar. 2025
// *      Author: DANNY
// */
//
//#include <main.h>
//#include <gpio.h>
//#include <spi.h>
//#include <exti.h>
//#include "fatfs_sd.h"
//#include "audio_recorder.h"
//#include "write_sector_0.h"
//
//uint32_t systemClock=72000000;
//volatile bool exti_flag = false;
//volatile bool isFileCreated=false;
//
///* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
//void write_sector_test(void);
//bool configurar_sector_base(uint8_t frecuencia_khz, uint16_t sectores_por_fichero, uint16_t num_ficheros);
//
//
//int main (void)
//{
//	//----------Setup------------------------------//
//	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//	HAL_Init();
//	/* Configure the system clock */
//	SystemClock_Config();
//	/* Initialize all configured peripherals */
//	//GPIO LED Config
//	gpio_LED_config();
//	//USART COnfig
//	uart_UART2_GPIO_config();
//	uart_UART2_config(systemClock);
//	//SPI CONFIG
//	spi_GPIO_config();
//	MX_SPI1_Init();
//
//	//Mount SD Card
//	bool sd_mounted = SD_disk_initialize(0);
//
//	if (sd_mounted != 0)
//	{
//		printf("Failed to mount SD card\r\n");
//	}
//	else
//	{
//		printf("Successfully Mounted SD card\r\n");
//
//		// Opción 1: Borrar solo los primeros sectores (más rápido)
//		borrar_sectores(50000, 100000); // Borra los primeros 9110 sectores
//
//		//		// Opción 2: Borrar todos los sectores que se usarán (más lento)
//		//		// Calcular cuántos sectores se necesitan para todos los archivos
//		//		uint16_t sectores_por_fichero = 33750;
//		//		uint16_t num_ficheros = 10; // O el número que desees probar
//		//		borrar_sectores(0, 1 + (sectores_por_fichero + 1) * num_ficheros);
//		//
//
//
//		while(1) {
//
//			gpio_LED_toggleGreen();
//			HAL_Delay(500);
//
//		}
//	}
//}
//
//
///**
// * @brief System Clock Configuration
// * @retval None
// */
//void SystemClock_Config(void)
//{
//	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//	/** Configure the main internal regulator output voltage
//	 */
//	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
//	{
//		Error_Handler();
//	}
//
//	/** Initializes the RCC Oscillators according to the specified parameters
//	 * in the RCC_OscInitTypeDef structure.
//	 */
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
//	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
//	RCC_OscInitStruct.MSICalibrationValue = 0;
//	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
//	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
//	RCC_OscInitStruct.PLL.PLLM = 1;
//	RCC_OscInitStruct.PLL.PLLN = 36;
//	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
//	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
//	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//	{
//		Error_Handler();
//	}
//
//	/** Initializes the CPU, AHB and APB buses clocks
//	 */
//	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//
//	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
//	{
//		Error_Handler();
//	}
//
//}
//
///**
// * @brief GPIO Initialization Function
// * @param None
// * @retval None
// */
//
//
//void Error_Handler(void)
//{
//	/* USER CODE BEGIN Error_Handler_Debug */
//	/* User can add his own implementation to report the HAL error return state */
//	__disable_irq();
//	while (1)
//	{
//	}
//	/* USER CODE END Error_Handler_Debug */
//}
