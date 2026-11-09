#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "nvs_flash.h"

#include "led_manager.h"
#include "http_server.h"

#define WIFI_SSID "wifissid"
#define WIFI_PASS "pass1234"


static const char *TAG = "WIFI_INIT";

typedef enum
{
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED

} wifi_state_t;

static volatile wifi_state_t wifi_state =
    WIFI_STATE_DISCONNECTED;


static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        wifi_state = WIFI_STATE_CONNECTING;

        ESP_LOGI(TAG, "Conectando al WiFi...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_state = WIFI_STATE_CONNECTING;

        ESP_LOGW(TAG, "WiFi desconectado");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        
        ip_event_got_ip_t *event =
        (ip_event_got_ip_t *)event_data;
        
        wifi_state = WIFI_STATE_CONNECTED;
        
        start_webserver();

        ESP_LOGI(
            TAG,
            "IP obtenida: " IPSTR,
            IP2STR(&event->ip_info.ip));
    }
}


static void wifi_status_task(void *pvParameters)
{
    while (1)
    {
        switch (wifi_state)
        {
            case WIFI_STATE_CONNECTING:

                led_all_off();
                led_yellow_toggle();

                vTaskDelay(pdMS_TO_TICKS(500));
                break;

            case WIFI_STATE_CONNECTED:

                led_all_off();
                led_green_on();

                vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case WIFI_STATE_DISCONNECTED:

                led_all_off();
                led_red_on();

                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
        }
    }
}



void app_main(void)
{



    led_init();

  


    wifi_state = WIFI_STATE_DISCONNECTED;
    xTaskCreate(
    wifi_status_task,
    "wifi_status",
    2048,
    NULL,
    5,
    NULL);

   
    esp_err_t ret;

    ESP_LOGI(TAG, "Inicializando NVS");

    ret = nvs_flash_init();
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Inicializando Netif");

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(TAG, "Inicializando Event Loop");

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "Creando interfaz WiFi STA");

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_LOGI(TAG, "Inicializando driver WiFi");

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            NULL));

    ESP_LOGI(TAG, "Driver WiFi inicializado correctamente");

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config));

    ESP_ERROR_CHECK(
        esp_wifi_start());

    ESP_LOGI(TAG, "WiFi iniciado");
    
   

}