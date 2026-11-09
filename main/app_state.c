#include "app_state.h"

static sector_estado_t g_sector_state;


void app_state_init(void)
{
    g_sector_state.temperatura = 24.5f;

    g_sector_state.humedad = 60.0f;

    g_sector_state.iluminacion = true;

    g_sector_state.aire_acondicionado = false;

    g_sector_state.tomacorrientes = false;

    g_sector_state.alarma = ALARMA_DESACTIVADA;

    g_sector_state.wifi = WIFI_DESCONECTADO;
}

sector_estado_t* app_state_get(void)
{
    return &g_sector_state;
}
