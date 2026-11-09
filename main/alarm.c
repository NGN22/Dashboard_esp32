#include "alarm.h"
#include "app_state.h"

static alarma_estado_t current_state;

void alarm_init(void)
{
    current_state = ALARMA_DESACTIVADA;

    app_state_get()->alarma = current_state;
}

void alarm_toggle(void)
{
    switch(current_state)
    {
        case ALARMA_DESACTIVADA:
            current_state = ALARMA_ACTIVADA;
            break;

        case ALARMA_ACTIVADA:
            current_state = ALARMA_DESACTIVADA;
            break;

        case ALARMA_DISPARADA:
            current_state = ALARMA_DESACTIVADA;
            break;
    }

    app_state_get()->alarma =
        current_state;
}

void alarm_trigger(void)
{
    if(current_state == ALARMA_ACTIVADA)
    {
        current_state = ALARMA_DISPARADA;

        app_state_get()->alarma =
            current_state;
    }
}


alarma_estado_t alarm_get_state(void)
{
    return current_state;
}

const char* alarm_state_to_string(alarma_estado_t state)
{
    switch(state)
    {
        case ALARMA_DESACTIVADA:
            return "DESACTIVADA";

        case ALARMA_ACTIVADA:
            return "ACTIVADA";

        case ALARMA_DISPARADA:
            return "DISPARADA";

        default:
            return "DESCONOCIDA";
    }
}