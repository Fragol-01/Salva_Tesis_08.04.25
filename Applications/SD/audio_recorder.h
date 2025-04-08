#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include "main.h"
#include "fatfs_sd.h"
#include "write_sector_0.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include <stdbool.h>


#define AUDIO_BUFFER_SIZE 4096

// Flag para indicar si la memoria está llena
extern volatile bool memoriaLlena; // Debe ser volatile en ambos lugares

// Contadores de diagnóstico
extern volatile uint32_t halfBuffersLost;
extern volatile uint32_t fullBuffersLost;

// Estados del grabador
typedef enum {
    RECORDER_IDLE,
    RECORDER_RECORDING,
    RECORDER_PAUSED,
    RECORDER_STOPPED
} RecorderState;

// Estructura para fecha/hora
typedef struct {
    uint8_t segundos;
    uint8_t minutos;
    uint8_t horas;
    uint8_t dia_semana;
    uint8_t dia_mes;
    uint8_t mes;
    uint8_t año;
} RTC_DateTime;

// Función para obtener la fecha/hora actual del RTC (sólo declaración)
RTC_DateTime obtener_fecha_hora(void);

// Inicializa el grabador de audio
bool audio_recorder_init(void);

// Inicia la grabación
bool audio_recorder_start(void);

// Pausa la grabación
void audio_recorder_pause(void);

// Continúa la grabación
void audio_recorder_resume(void);

// Detiene la grabación
void audio_recorder_stop(void);

// Proceso principal (debe llamarse en el loop principal)
void audio_recorder_process(void);

// Devuelve el estado actual del grabador
RecorderState audio_recorder_get_state(void);

// Función para obtener estadísticas de grabación
void audio_recorder_get_stats(uint32_t* half_lost, uint32_t* full_lost);

#endif // AUDIO_RECORDER_H
