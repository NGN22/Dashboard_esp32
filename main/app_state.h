#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>

typedef enum
{
    ALARMA_DESACTIVADA = 0,
    ALARMA_ACTIVADA,
    ALARMA_DISPARADA

} alarma_estado_t;

typedef enum
{
    WIFI_DESCONECTADO = 0,
    WIFI_CONECTANDO,
    WIFI_CONECTADO

} wifi_estado_t;

typedef struct
{
    float temperatura;

    float humedad;

    bool iluminacion;

    bool aire_acondicionado;

    bool tomacorrientes;

    alarma_estado_t alarma;

    wifi_estado_t wifi;

} sector_estado_t;

void app_state_init(void);

sector_estado_t* app_state_get(void);

#endif