#include "http_server.h"

#include <stdio.h>
#include <string.h>

#include "esp_http_server.h"
#include "esp_log.h"

#include "app_state.h"
#include "alarm.h"

static const char *TAG = "HTTP";

/************************************************
 * Content-Type
 ************************************************/

static const char *get_content_type(
    const char *path)
{
    if (strstr(path, ".html"))
        return "text/html";

    if (strstr(path, ".css"))
        return "text/css";

    if (strstr(path, ".js"))
        return "application/javascript";

    if (strstr(path, ".png"))
        return "image/png";

    if (strstr(path, ".jpg"))
        return "image/jpeg";

    if (strstr(path, ".ico"))
        return "image/x-icon";

    return "text/plain";
}

/************************************************
 * Envío de archivos SPIFFS por chunks
 ************************************************/

static esp_err_t send_file(
    httpd_req_t *req,
    const char *path)
{
    FILE *f = fopen(path, "rb");

    if (f == NULL)
    {
        ESP_LOGE(TAG,
                 "No se pudo abrir %s",
                 path);

        httpd_resp_send_err(
            req,
            HTTPD_404_NOT_FOUND,
            path);

        return ESP_FAIL;
    }

    httpd_resp_set_type(
        req,
        get_content_type(path));

    char buffer[512];

    size_t bytes_read;

    do
    {
        bytes_read = fread(
            buffer,
            1,
            sizeof(buffer),
            f);

        if (bytes_read > 0)
        {
            esp_err_t err =
                httpd_resp_send_chunk(
                    req,
                    buffer,
                    bytes_read);

            if (err != ESP_OK)
            {
                fclose(f);

                ESP_LOGE(TAG,
                         "Error enviando chunk");

                return err;
            }
        }

    }
    while (bytes_read > 0);

    fclose(f);

    httpd_resp_send_chunk(
        req,
        NULL,
        0);

    return ESP_OK;
}

/************************************************
 * Archivos estáticos SPIFFS
 ************************************************/

static esp_err_t static_file_handler(
    httpd_req_t *req)
{
    char path[128];

    if (strcmp(req->uri, "/") == 0)
    {
        strcpy(
            path,
            "/spiffs/index.html");
    }
    else
    {
        int ret = snprintf(
            path,
            sizeof(path),
            "/spiffs%s",
            req->uri);

        if (ret < 0 ||
            ret >= sizeof(path))
        {
            httpd_resp_send_err(
                req,
                HTTPD_414_URI_TOO_LONG,
                "URI demasiado larga");

            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG,
             "Sirviendo %s",
             path);

    return send_file(
        req,
        path);
}

/************************************************
 * API JSON
 ************************************************/

static esp_err_t api_state_handler(
    httpd_req_t *req)
{
    sector_estado_t *estado =
        app_state_get();

    const char *wifi_str;
    const char *alarma_str;

    switch (estado->wifi)
    {
        case WIFI_CONECTADO:
            wifi_str = "CONECTADO";
            break;

        case WIFI_CONECTANDO:
            wifi_str = "CONECTANDO";
            break;

        default:
            wifi_str = "DESCONECTADO";
            break;
    }

    alarma_str =
        alarm_state_to_string(
            estado->alarma);

    char json[256];

    snprintf(
        json,
        sizeof(json),
        "{"
        "\"wifi\":\"%s\","
        "\"alarma\":\"%s\","
        "\"temperatura\":%.1f,"
        "\"humedad\":%.1f,"
        "\"iluminacion\":%s,"
        "\"aire_acondicionado\":%s,"
        "\"tomacorrientes\":%s"
        "}",
        wifi_str,
        alarma_str,
        estado->temperatura,
        estado->humedad,
        estado->iluminacion ? "true" : "false",
        estado->aire_acondicionado ? "true" : "false",
        estado->tomacorrientes ? "true" : "false");

    httpd_resp_set_type(
        req,
        "application/json");

    httpd_resp_send(
        req,
        json,
        HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/************************************************
 * URI API
 ************************************************/

static const httpd_uri_t api_state_uri =
{
    .uri = "/api/state",
    .method = HTTP_GET,
    .handler = api_state_handler,
    .user_ctx = NULL
};

/************************************************
 * URI estática
 ************************************************/

static const httpd_uri_t static_uri =
{
    .uri = "/*",
    .method = HTTP_GET,
    .handler = static_file_handler,
    .user_ctx = NULL
};

/************************************************
 * Registro de rutas
 ************************************************/

static void register_handlers(
    httpd_handle_t server)
{
    httpd_register_uri_handler(
        server,
        &api_state_uri);

    httpd_register_uri_handler(
        server,
        &static_uri);
}

/************************************************
 * Servidor HTTP
 ************************************************/

void start_webserver(void)
{
    httpd_config_t config =
        HTTPD_DEFAULT_CONFIG();

    config.uri_match_fn =
        httpd_uri_match_wildcard;

    httpd_handle_t server = NULL;

    if (httpd_start(
            &server,
            &config) == ESP_OK)
    {
        register_handlers(server);

        ESP_LOGI(TAG,
                 "Servidor HTTP iniciado");
    }
}