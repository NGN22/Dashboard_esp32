#include "ir_remote.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#define IR_GPIO GPIO_NUM_23

#define FRAME_SIZE 70

static const char *TAG = "IR";

typedef struct
{
    uint32_t duration;

} edge_event_t;

static QueueHandle_t edge_queue;

static ir_command_t last_command = IR_CMD_NONE;

/*************************************************************
 * Utilidades
 *************************************************************/

static bool near(uint32_t value,
                 uint32_t target,
                 uint32_t tolerance)
{
    return (value > (target - tolerance)) &&
           (value < (target + tolerance));
}

/*************************************************************
 * Traducción NEC -> Evento
 *************************************************************/

static ir_command_t decode_command(uint8_t cmd)
{
    switch (cmd)
    {
        case 0x45:
            return IR_CMD_CH_MINUS;

        case 0x46:
            return IR_CMD_CH;

        case 0x47:
            return IR_CMD_CH_PLUS;

        case 0x43:
            return IR_CMD_PLAY;

        case 0x0C:
            return IR_CMD_1;

        case 0x18:
            return IR_CMD_2;

        case 0x5E:
            return IR_CMD_3;

        default:
            return IR_CMD_NONE;
    }
}

/*************************************************************
 * ISR
 *************************************************************/

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    static int64_t last_time = 0;

    int64_t now = esp_timer_get_time();

    if (last_time != 0)
    {
        edge_event_t evt;

        evt.duration = (uint32_t)(now - last_time);

        BaseType_t hp_task_woken = pdFALSE;

        xQueueSendFromISR(
            edge_queue,
            &evt,
            &hp_task_woken);

        if (hp_task_woken)
        {
            portYIELD_FROM_ISR();
        }
    }

    last_time = now;
}

/*************************************************************
 * Tarea decodificadora
 *************************************************************/

static void ir_decode_task(void *arg)
{
    edge_event_t evt;

    uint32_t frame[FRAME_SIZE];

    uint8_t index = 0;

    while (1)
    {
        if (xQueueReceive(
                edge_queue,
                &evt,
                portMAX_DELAY))
        {
            if (evt.duration > 8500 &&
                evt.duration < 10000)
            {
                index = 0;
            }

            if (index < FRAME_SIZE)
            {
                frame[index++] = evt.duration;
            }

            if (index >= 66)
            {
                if (near(frame[0], 9000, 1500) &&
                    near(frame[1], 4500, 1000))
                {
                    uint32_t code = 0;

                    for (int bit = 0; bit < 32; bit++)
                    {
                        uint32_t high_time =
                            frame[3 + bit * 2];

                        if (high_time > 1200)
                        {
                            code |= (1UL << bit);
                        }
                    }

                    uint8_t cmd =
                        (code >> 16) & 0xFF;

                    last_command =
                        decode_command(cmd);

                    if (last_command != IR_CMD_NONE)
                    {
                        ESP_LOGI(TAG,
                                 "Comando recibido: 0x%02X",
                                 cmd);
                    }
                }

                index = 0;
            }
        }
    }
}

/*************************************************************
 * API pública
 *************************************************************/

void ir_remote_init(void)
{
    edge_queue = xQueueCreate(
        128,
        sizeof(edge_event_t));

    gpio_config_t io_conf =
    {
        .pin_bit_mask = (1ULL << IR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    ESP_ERROR_CHECK(
        gpio_config(&io_conf));

    ESP_ERROR_CHECK(
        gpio_install_isr_service(
            ESP_INTR_FLAG_IRAM));

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            IR_GPIO,
            gpio_isr_handler,
            NULL));

    xTaskCreate(
        ir_decode_task,
        "ir_decode",
        4096,
        NULL,
        5,
        NULL);

    ESP_LOGI(TAG,
             "Receptor IR inicializado");
}

bool ir_remote_get_command(ir_command_t *cmd)
{
    if (last_command == IR_CMD_NONE)
    {
        return false;
    }

    *cmd = last_command;

    last_command = IR_CMD_NONE;

    return true;
}