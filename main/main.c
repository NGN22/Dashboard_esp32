#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_state.h"
#include "alarm.h"
#include "led_status.h"
#include "intrusion_sensor.h"
#include "ir_remote.h"
#include "wifi_manager.h"

#include "spiffs_manager.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    /* Inicialización del estado global */
    app_state_init();

    /* Inicialización de SPIFFS */
    spiffs_init();

    /* Inicialización de módulos */
    alarm_init();
    led_status_init();
    intrusion_sensor_init();
    ir_remote_init();
    wifi_manager_init();

    /* Referencia al estado global */
    sector_estado_t *estado = app_state_get();

    /* Reflejar estado inicial en LEDs */
    led_status_update(alarm_get_state());

    ESP_LOGI(TAG,
             "Estado inicial: %s",
             alarm_state_to_string(
                 estado->alarma));

    while (1)
    {
        ir_command_t cmd;

        /************************************************
         * Control remoto IR
         ************************************************/
        if (ir_remote_get_command(&cmd))
        {
            switch (cmd)
            {
                case IR_CMD_2:

                    alarm_toggle();

                    led_status_update(
                        alarm_get_state());

                    break;

                default:
                    break;
            }
        }

        /************************************************
         * Sensor de intrusión
         ************************************************/
        if (alarm_get_state() == ALARMA_ACTIVADA)
        {
            if (intrusion_sensor_is_triggered())
            {
                alarm_trigger();

                led_status_update(
                    alarm_get_state());

                ESP_LOGI(TAG,
                         "!!! INTRUSION DETECTADA !!!");

            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}