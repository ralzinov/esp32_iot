#include "stdint.h"
#include "esp_log.h"
#include "websocket.h"

#define LOG_TAG "[ws]"
#define websocketCONNECTION_TIMEOUT_MS 100

static void xWebsocketEventHandler(void *xHandlerArgs, esp_event_base_t base, int32_t eventId, void *xEventData)
{
    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)xHandlerArgs;
    switch (eventId) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(LOG_TAG, "Connected to %s", client->config->uri);
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "Disconnected to %s", client->config->uri);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(LOG_TAG, "Got unhandled error on %s", client->config->uri);
            break;
    }
}

/**
 * @brief  Connect to websocket server
*/
esp_websocket_client_handle_t xWebsocketInitConnection(const esp_websocket_client_config_t *config)
{
    esp_websocket_client_handle_t client = esp_websocket_client_init(config);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, xWebsocketEventHandler, (void *)client);
    return client;
}

esp_err_t vWebsocketStart(esp_websocket_client_handle_t client)
{
    ESP_LOGI(LOG_TAG, "Connecting to: %s", client->config->uri);
    return esp_websocket_client_start(client);
}
