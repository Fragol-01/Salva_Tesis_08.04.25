/**
 * @brief Configura el sector base (sector 0) con la información del sistema
 * @param frecuencia_muestreo Frecuencia de muestreo en kHz (se almacena freq/1000)
 * @param sectores_por_fichero Número de sectores por fichero
 * @param num_ficheros Número de ficheros en la SD
 * @return true si la operación fue exitosa, false en caso contrario
 */

#include "write_sector_0.h"
#include <stdio.h>
#include <string.h>

// Añadir esta definición al inicio del archivo
DWORD total_sd_sectors = 0;

/*
Cálculo de muestras totales:
48000 muestras/segundo × 60 segundos/minuto × 2 minutos = 5760000 muestras

Cálculo de bytes totales:
Cada muestra usa 4 bytes (32 bits para ADC->DR con resolución de 12 bits)
5760000 muestras × 4 bytes/muestra = 23040000  bytes

Cálculo de sectores necesarios:
Cada sector es de 512 bytes (definido por la SD)
23040000 bytes ÷ 512 bytes/sector = 45000 sectores
 */

bool configurar_sector_base(uint8_t frecuencia_khz, uint16_t sectores_por_fichero, uint16_t num_ficheros) {
    uint8_t buffer[512];
    memset(buffer, 0, sizeof(buffer));

    // Obtener el número total de sectores de la SD una sola vez
    if (SD_disk_ioctl(0, GET_SECTOR_COUNT, &total_sd_sectors) == RES_OK) {
        printf("Capacidad total de la SD: %lu sectores\r\n", total_sd_sectors);
        printf("Un archivo ocupa %u sectores\r\n", sectores_por_fichero);

        // Calcular máximos archivos posibles basado en el espacio total
        if (total_sd_sectors > sectores_por_fichero) {
            num_ficheros = (uint16_t)((total_sd_sectors - 1) / sectores_por_fichero);
            printf("Número máximo de archivos posibles: %u\r\n", num_ficheros);
        }
    } else {
        printf("Error al obtener capacidad total de la SD\r\n");
        return false;
    }
    
    // IMPORTANTE: Dividir sectores_por_fichero por 100 como espera LectorAudio.py
    uint16_t sectores_para_guardar = sectores_por_fichero / 100;
    
    // Configurar los valores según el formato exacto
    buffer[0] = frecuencia_khz;                          // Frecuencia
    buffer[1] = (sectores_para_guardar) & 0xFF;          // LOW-T_Pack
    buffer[2] = ((sectores_para_guardar) >> 8) & 0xFF;   // HIGH-T_Pack
    buffer[3] = 0;                                        // LOW-N_Pack (archivos escritos = 0)
    buffer[4] = 0;                                        // HIGH-N_Pack (archivos escritos = 0)
    buffer[7] = 0x89;                                     // Llave que identifica el sector base
    
    // Inicializar contador de sectores en 1 (después del sector 0)
    buffer[8] = 1;   // Comenzamos a escribir en el sector 1
    buffer[9] = 0;
    buffer[10] = 0;
    buffer[11] = 0;

    printf("Configurando sector base (sector 0)...\r\n");
    printf("Frecuencia: %d kHz\r\n", frecuencia_khz);
    printf("Sectores por fichero: %d (guardado como: %d)\r\n",
           sectores_por_fichero, sectores_para_guardar);
    printf("Numero de ficheros: %d\r\n", num_ficheros);
    
    // Escribir en el sector 0
    DRESULT write_result = SD_disk_write(0, buffer, 0, 1);
    
    if (write_result == RES_OK) {
        printf("Sector base configurado correctamente\r\n");
        
        // Sincronizar para asegurar que la escritura se completó físicamente
        SD_disk_ioctl(0, CTRL_SYNC, 0);
        return true;
    } else {
        printf("Error al configurar sector base: %d\r\n", write_result);
        return false;
    }
}

/**
 * @brief Lee el sector base (sector 0) y muestra su información
 * @return true si la operación fue exitosa, false en caso contrario
 */
bool leer_sector_base(void) {
    uint8_t buffer[512];
    memset(buffer, 0, sizeof(buffer));
    
    printf("Leyendo sector base (sector 0)...\r\n");
    
    // Leer el sector 0
    DRESULT read_result = SD_disk_read(0, buffer, 0, 1);
    
    if (read_result == RES_OK) {
        uint8_t frecuencia_khz = buffer[0];
        
        // Leer correctamente los 2 bytes de sectores_por_fichero
        uint16_t sectores_guardados = buffer[1] | ((uint16_t)buffer[2] << 8);
        
        // IMPORTANTE: Multiplicar por 100 para obtener el valor real
        uint16_t sectores_por_fichero = sectores_guardados * 100;
        
        uint16_t num_ficheros = buffer[3] | ((uint16_t)buffer[4] << 8);
        uint8_t llave = buffer[7];
        
        printf("Datos del sector base:\r\n");
        printf("Frecuencia: %d kHz\r\n", frecuencia_khz);
        printf("Sectores por fichero: %d (valor guardado: %d)\r\n", 
               sectores_por_fichero, sectores_guardados);
        printf("Numero de ficheros: %d\r\n", num_ficheros);
        printf("Llave: 0x%02X (Esperado: 0x89)\r\n", llave);
        
        // Calcular el tiempo total disponible (en minutos)
        float tiempo_minutos = (float)sectores_por_fichero * (float)num_ficheros * 512.0f / 2.0f / 60.0f / (frecuencia_khz * 1000.0f);
        printf("Tiempo total disponible: %.1f minutos (%.1f horas)\r\n", 
               tiempo_minutos, tiempo_minutos/60.0f);
        
        if (llave == 0x89) {
            printf("Formato de sector base valido\r\n");
            return true;
        } else {
            printf("¡Advertencia! La llave no coincide con el valor esperado 0x89\r\n");
            return false;
        }
    } else {
        printf("Error al leer el sector base: %d\r\n", read_result);
        return false;
    }
}

// Borrar sectores de memoria
bool borrar_sectores(uint32_t sector_inicio, uint32_t num_sectores) {
    const uint32_t MAX_BATCH = 10; // Número de sectores a borrar por lote
    uint8_t buffer[512];
    memset(buffer, 0, sizeof(buffer)); // Buffer con todos ceros

    printf("Borrando sectores %lu a %lu...\r\n", sector_inicio, sector_inicio + num_sectores - 1);

    // Borrar en lotes para no consumir demasiada memoria
    for (uint32_t i = 0; i < num_sectores; i += MAX_BATCH) {
        uint32_t batch_size = (i + MAX_BATCH <= num_sectores) ? MAX_BATCH : num_sectores - i;
        uint32_t current_sector = sector_inicio + i;

        // Hacer escrituras repetidas del buffer de ceros
        for (uint32_t j = 0; j < batch_size; j++) {
            DRESULT write_result = SD_disk_write(0, buffer, current_sector + j, 1);
            if (write_result != RES_OK) {
                printf("Error al borrar sector %lu: %d\r\n", current_sector + j, write_result);
                return false;
            }
        }

        // Mostrar progreso cada 100 lotes (1000 sectores)
        if ((i / MAX_BATCH) % 100 == 0) {
            printf("Progreso: %lu/%lu sectores\r\n", i, num_sectores);
        }
    }

    // Sincronizar para asegurar que todas las escrituras se completaron
    SD_disk_ioctl(0, CTRL_SYNC, 0);
    printf("Borrado completado: %lu sectores\r\n", num_sectores);
    return true;
}
