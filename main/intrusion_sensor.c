#include "intrusion_sensor.h"

#include "driver/gpio.h"




#define INTRUSION_GPIO GPIO_NUM_36

void intrusion_sensor_init(void)
{
    gpio_config_t io_conf =
    {
        .pin_bit_mask = (1ULL << INTRUSION_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
}

/*bool intrusion_sensor_is_triggered(void)
{
    return gpio_get_level(INTRUSION_GPIO) == 0;
}*/
bool intrusion_sensor_is_triggered(void)
{
    static int last_level = 1;

    int current_level =
        gpio_get_level(INTRUSION_GPIO);

    bool triggered = false;

    if ((last_level == 1) &&
        (current_level == 0))
    {
        triggered = true;
    }

    last_level = current_level;

    return triggered;
}