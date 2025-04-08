#include "audio_recorder.h"
#include <string.h>
#include <stdio.h>
#include "opamp.h"

// Buffer de audio para el DMA (recibe datos de 16 bits del ADC)
volatile static uint32_t audioBuffer[AUDIO_BUFFER_SIZE/2]; // 2048 elementos de 32 bits = 8192 bytes

// Control de estado
volatile static RecorderState recorderState = RECORDER_IDLE;
volatile static uint32_t currentSector = 1; // Empezamos desde el sector 1 (el 0 es para metadatos)
volatile static uint16_t currentFileIndex = 0;
volatile static uint32_t sectoresGrabados = 0;
volatile static uint16_t sectorPrincipalActual = 0;

// Flags para el procesamiento del buffer
static volatile bool halfBufferReady = false;
static volatile bool fullBufferReady = false;
static volatile bool stopRequested = false;

// Flag para indicar si la memoria está llena
volatile bool memoriaLlena = false;

// Contadores de diagnóstico
volatile uint32_t halfBuffersLost = 0;
volatile uint32_t fullBuffersLost = 0;

// Inicializa el grabador de audio
bool audio_recorder_init(void) {
    // Configurar el ADC y el DMA para la captura
    tim_TIM6_MIC_config(); // Configura el timer para muestreo a 48kHz
    opamp_config();        // Configura el OPAMP para el micrófono PA0->A0
    adc_MIC_config();      // Configura el ADC
    
    // Obtener valores del sector base
    uint8_t buffer[512];
    if (SD_disk_read(0, buffer, 0, 1) == RES_OK) {
        // Leer current file index del sector base
        currentFileIndex = buffer[3] | ((uint16_t)buffer[4] << 8);

        // Leer sector counter
        uint32_t sector_counter = buffer[8] | ((uint32_t)buffer[9] << 8) |
                                 ((uint32_t)buffer[10] << 16) | ((uint32_t)buffer[11] << 24);

        // Verificar si hay espacio disponible
        if (sector_counter + 45000 > total_sd_sectors) {
            printf("¡ADVERTENCIA! Memoria SD llena. No se pueden grabar mas archivos.\r\n");
            memoriaLlena = true;
        }
    }

    // Resetear estados
    recorderState = RECORDER_IDLE;
    currentSector = 1;
    sectoresGrabados = 0;
    halfBufferReady = false;
    fullBufferReady = false;
    stopRequested = false;
    
    printf("Grabador de audio inicializado\r\n");
    return true;
}

// Inicia la grabación
bool audio_recorder_start(void) {
    // Verificar si podemos iniciar grabación
    if (recorderState != RECORDER_IDLE && recorderState != RECORDER_STOPPED) {
        printf("Error: No se puede iniciar grabacion, estado incorrecto\r\n");
        return false;
    }
    
    // Verificar si memoria está llena
    if (memoriaLlena) {
        printf("Error: No se puede iniciar grabacion, memoria SD llena\r\n");
        return false;
    }

    // Actualizar sector donde comienza el archivo
    if (currentFileIndex > 0) {
        currentSector = 1 + (currentFileIndex * (45000)); // +1 por sector principal
    } else {
        currentSector = 1;
    }

    // Inicializar y configurar el buffer DMA
    memset((void*)audioBuffer, 0, sizeof(audioBuffer));
    
    // Iniciar DMA y timer para comenzar a recolectar datos
    adc_MIC_DMA_config((uint16_t*)audioBuffer, AUDIO_BUFFER_SIZE);
    TIM6->CR1 |= (TIM_CR1_CEN); // Iniciar el timer

    // Configurar el sector principal del archivo actual
    sectorPrincipalActual = currentSector;
    
    // Iniciar la grabación antes de escribir
    recorderState = RECORDER_RECORDING;
    sectoresGrabados = 0;
    halfBufferReady = false;
    fullBufferReady = false;
    stopRequested = false;

    // Esperar a que el DMA llene al menos la mitad del buffer
    uint32_t timeout = HAL_GetTick() + 100; // 100ms timeout
    while (!halfBufferReady && HAL_GetTick() < timeout) {
        // Esperar a que lleguen datos
    }

    if (!halfBufferReady) {
        printf("Error: Timeout esperando datos de audio\r\n");
        TIM6->CR1 &= ~(TIM_CR1_CEN); // Detener timer
        DMA1_Channel1->CCR &= ~(DMA_CCR_EN); // Detener DMA
        recorderState = RECORDER_STOPPED;
        return false;
    }

    // Crear un buffer temporal para los primeros 4 sectores (combina metadatos + datos)
    uint8_t primerSector[2048]; // 4 sectores = 2048 bytes
    memset(primerSector, 0, sizeof(primerSector));

    // Primeros 12 bytes: metadatos del archivo
    primerSector[0] = 0;                     // segundos
    primerSector[1] = 0;                     // minutos
    primerSector[2] = 0;                     // horas
    primerSector[3] = 1;                     // día semana
    primerSector[4] = 1;                     // día mes
    primerSector[5] = 1;                     // mes
    primerSector[6] = 23;                    // año
    primerSector[7] = 0x89;                  // Llave identificadora (0x89)
    primerSector[8] = currentFileIndex & 0xFF;        // índice bajo
    primerSector[9] = (currentFileIndex >> 8) & 0xFF; // índice alto
    // bytes 10-11 quedan en 0
    
    // A partir del byte 12, copiar datos del buffer de audio (2036 bytes)
    memcpy(primerSector + 12, (const void*)audioBuffer, 2048 - 12);
    
    // Escribir estos 4 sectores combinados
    DRESULT write_result = SD_disk_write(0, primerSector, sectorPrincipalActual, 4);
    if (write_result != RES_OK) {
        printf("Error al escribir sector principal+datos: %d\r\n", write_result);
        TIM6->CR1 &= ~(TIM_CR1_CEN);
        DMA1_Channel1->CCR &= ~(DMA_CCR_EN);
        recorderState = RECORDER_STOPPED;
        return false;
    }
    
    // Marcar que usamos parte del buffer y avanzar al siguiente sector
    halfBufferReady = false; // Ya usamos estos datos
    currentSector += 4;      // Avanzar 4 sectores
    sectoresGrabados += 4;   // Incrementar contador - Ya grabamos 4 sectores
    
    printf("Grabacion iniciada - Archivo #%d\r\n", currentFileIndex);
    return true;
}

// Pausa la grabación
void audio_recorder_pause(void) {
    if (recorderState == RECORDER_RECORDING) {
        // Detener el timer pero conservar el estado del DMA
        TIM6->CR1 &= ~(TIM_CR1_CEN);
        recorderState = RECORDER_PAUSED;
        printf("Grabacion pausada\r\n");
    }
}

// Continúa la grabación
void audio_recorder_resume(void) {
    if (recorderState == RECORDER_PAUSED) {
        // Reiniciar el timer
        TIM6->CR1 |= (TIM_CR1_CEN);
        recorderState = RECORDER_RECORDING;
        printf("Grabacion reanudada\r\n");
    }
}

// Modificar la función audio_recorder_stop()

void audio_recorder_stop(void) {
    if (recorderState == RECORDER_RECORDING || recorderState == RECORDER_PAUSED) {
        // Detener el timer y el DMA
        TIM6->CR1 &= ~(TIM_CR1_CEN);
        DMA1_Channel1->CCR &= ~(DMA_CCR_EN);
        
        // Leer sector base
        uint8_t buffer[512];
        DRESULT read_result = SD_disk_read(0, buffer, 0, 1);
        if (read_result == RES_OK) {
            // Si completamos un archivo (casi completo también cuenta)
            if (sectoresGrabados >= 45000) {
                // Incrementar el contador de archivos escritos
                currentFileIndex++;

                // Actualizar en el sector base
                buffer[3] = currentFileIndex & 0xFF;
                buffer[4] = (currentFileIndex >> 8) & 0xFF;

                // Calcular sector del próximo archivo
                uint32_t sector_counter = 1 + (currentFileIndex * 45000);
                buffer[8] = sector_counter & 0xFF;
                buffer[9] = (sector_counter >> 8) & 0xFF;
                buffer[10] = (sector_counter >> 16) & 0xFF;
                buffer[11] = (sector_counter >> 24) & 0xFF;

                // Verificar si hay espacio para el próximo archivo usando el valor global
                if (sector_counter + 45000 > total_sd_sectors) {
                    printf("¡ADVERTENCIA! Memoria SD llena. No se pueden grabar mas archivos.\r\n");
                    memoriaLlena = true;
                }
            }
            
            // Escribir sector actualizado
            SD_disk_write(0, buffer, 0, 1);
            SD_disk_ioctl(0, CTRL_SYNC, 0); // Asegurar que se escribió físicamente
        }
        
        printf("Grabacion detenida - Sectores grabados: %lu\r\n", sectoresGrabados);

        // IMPORTANTE: Resetear contadores y flags para el próximo archivo
        sectoresGrabados = 0;
        stopRequested = false;



        // Y solo después intentamos iniciar una nueva grabación si corresponde
        if (!memoriaLlena && currentFileIndex > 0) {
            printf("Iniciando siguiente grabación automáticamente...\r\n");
            // Primero cambiamos el estado a STOPPED
            recorderState = RECORDER_STOPPED;
            audio_recorder_start(); // Comenzar siguiente grabación
        } else {
            recorderState = RECORDER_STOPPED;
        }
    }
}

// Proceso principal (debe llamarse en el loop principal)
void audio_recorder_process(void) {
    // Verificar si hay datos listos para procesar
    if (recorderState == RECORDER_RECORDING) {
        // Procesar la primera mitad del buffer si está lista
        if (halfBufferReady) {
            // Escribir 4 sectores en una sola operación (2048 bytes)
            uint8_t* writeBuffer = (uint8_t*)&audioBuffer[0];
            DRESULT write_result = SD_disk_write(0, writeBuffer, currentSector, 4);
            
            if (write_result == RES_OK) {
                currentSector += 4;         // Avanzar 4 sectores
                sectoresGrabados += 4;      // Incrementar contador total
                halfBufferReady = false;
                
                // Si alcanzamos el límite de sectores para un archivo
                if (sectoresGrabados >= 45000) {
                    stopRequested = true;
                }
            } else {
                printf("Error al escribir datos (half): %d\r\n", write_result);
                halfBuffersLost++;
            }
        }
        
        // Procesar la segunda mitad del buffer si está lista
        if (fullBufferReady) {
            // Escribir 4 sectores en una sola operación (2048 bytes)
            // El offset es 512 bytes (128 valores de 32 bits) después del primer bloque de 4 sectores
            uint8_t* writeBuffer = (uint8_t*)&audioBuffer[512];
            DRESULT write_result = SD_disk_write(0, writeBuffer, currentSector, 4);
            
            if (write_result == RES_OK) {
                currentSector += 4;
                sectoresGrabados += 4;
                fullBufferReady = false;
                
                if (sectoresGrabados >= 45000) {
                    stopRequested = true;
                }
            } else {
                printf("Error al escribir datos (full): %d\r\n", write_result);
                fullBuffersLost++;
            }
        }
        
        // Verificación para detener grabación
        if (stopRequested) {
            audio_recorder_stop();
        }
    }
}

// Devuelve el estado actual del grabador
RecorderState audio_recorder_get_state(void) {
    return recorderState;
}

// Devuelve las estadísticas de buffers perdidos
void audio_recorder_get_stats(uint32_t* half_lost, uint32_t* full_lost) {
    *half_lost = halfBuffersLost;
    *full_lost = fullBuffersLost;
}

/**
 * @Brief DMA1 Channel 1 Interrupt Handler
 */
void DMA1_Channel1_IRQHandler(void) {
    // Half Transfer Complete
    if (DMA1->ISR & DMA_ISR_HTIF1) {
        // Limpiar flag
        DMA1->IFCR |= DMA_IFCR_CHTIF1;

        // Si estamos grabando y no hay una transferencia previa pendiente
        if (recorderState == RECORDER_RECORDING && !halfBufferReady) {
            halfBufferReady = true;
        } else {
            // Si hay un buffer pendiente, registrar pérdida de datos
            if (halfBufferReady) {
                // Incrementar contador de buffers perdidos (variable global)
                halfBuffersLost++;
            }
        }
    }

    // Transfer Complete
    if (DMA1->ISR & DMA_ISR_TCIF1) {
        // Limpiar flag
        DMA1->IFCR |= DMA_IFCR_CTCIF1;

        // Si estamos grabando y no hay una transferencia previa pendiente
        if (recorderState == RECORDER_RECORDING && !fullBufferReady) {
            fullBufferReady = true;
        } else {
            // Si hay un buffer pendiente, registrar pérdida de datos
            if (fullBufferReady) {
                fullBuffersLost++;
            }
        }
    }

    // Limpiar flag de interrupción pendiente global
    NVIC_ClearPendingIRQ(DMA1_Channel1_IRQn);
}

// Implementación de la función obtener_fecha_hora
RTC_DateTime obtener_fecha_hora(void) {
    RTC_DateTime dt;

    // Aquí implementar la lectura del RTC del STM32L4
    // Si no tienes RTC configurado, puedes usar valores fijos

    dt.segundos = 0;    // Valores de ejemplo
    dt.minutos = 30;
    dt.horas = 12;
    dt.dia_semana = 3;
    dt.dia_mes = 20;
    dt.mes = 3;
    dt.año = 25;

    return dt;
}

