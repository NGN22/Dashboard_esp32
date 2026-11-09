#include "led_status.h"

#include "driver/gpio.h"

#define LED_VERDE      GPIO_NUM_25
#define LED_AMARILLO   GPIO_NUM_27
#define LED_ROJO       GPIO_NUM_26

static void apagar_todos(void)
{
    gpio_set_level(LED_VERDE, 0);
    gpio_set_level(LED_AMARILLO, 0);
    gpio_set_level(LED_ROJO, 0);
}

void led_status_init(void)
{
    gpio_reset_pin(LED_VERDE);
    gpio_reset_pin(LED_AMARILLO);
    gpio_reset_pin(LED_ROJO);

    gpio_set_direction(LED_VERDE, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_AMARILLO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_ROJO, GPIO_MODE_OUTPUT);

    apagar_todos();
}

void led_status_update(alarma_estado_t state)
{
    apagar_todos();

    switch(state)
    {
        case ALARMA_DESACTIVADA:
            gpio_set_level(LED_VERDE, 1);
            break;

        case ALARMA_ACTIVADA:
            gpio_set_level(LED_AMARILLO, 1);
            break;

        case ALARMA_DISPARADA:
            gpio_set_level(LED_ROJO, 1);
            break;

        default:
            break;
    }
}