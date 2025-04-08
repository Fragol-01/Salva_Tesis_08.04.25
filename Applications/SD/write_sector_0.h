/*
 * write_sector_0.h
 *
 *  Created on: 21 mar. 2025
 *      Author: DANNY
 */

#ifndef SD_WRITE_SECTOR_0_H_
#define SD_WRITE_SECTOR_0_H_

#include "main.h"
#include "fatfs_sd.h"
#include <stdbool.h>

// Añadir esta declaración
extern DWORD total_sd_sectors;

/**
 * @brief Configura el sector base (sector 0) con la información del sistema
 * @param frecuencia_muestreo Frecuencia de muestreo en kHz (se almacena freq/1000)
 * @param sectores_por_fichero Número de sectores por fichero
 * @param num_ficheros Número de ficheros en la SD
 * @return true si la operación fue exitosa, false en caso contrario
 */
bool configurar_sector_base(uint8_t frecuencia_khz, uint16_t sectores_por_fichero, uint16_t num_ficheros);

/**
 * @brief Lee el sector base (sector 0) y muestra su información
 * @return true si la operación fue exitosa, false en caso contrario
 */
bool leer_sector_base(void);

// Borrar sectores de memoria
bool borrar_sectores(uint32_t sector_inicio, uint32_t num_sectores);

#endif /* SD_WRITE_SECTOR_0_H_ */
