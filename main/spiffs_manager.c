#include "spiffs_manager.h"

#include "esp_spiffs.h"
#include "esp_log.h"

static const char *TAG = "SPIFFS";

void spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf =
    {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ESP_ERROR_CHECK(
        esp_vfs_spiffs_register(&conf));

    size_t total = 0;
    size_t used = 0;

    ESP_ERROR_CHECK(
        esp_spiffs_info(
            NULL,
            &total,
            &used));

    ESP_LOGI(TAG,
             "SPIFFS Total: %d",
             total);

    ESP_LOGI(TAG,
             "SPIFFS Used : %d",
             used);
}