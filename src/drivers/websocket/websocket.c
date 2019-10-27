#include "stdint.h"
#include "esp_log.h"
#include "websocket.h"

#define LOG_TAG "[ws]"

struct esp_websocket_client {
    esp_websocket_client_config_t   *config;
    EventGroupHandle_t              status_bits;
};

// TODO set waiting timeout and handle null response if write was unsuccessfull 

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)handler_args;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(LOG_TAG, "Connected");
            xEventGroupSetBits(client->status_bits, websocketCONNECTED_BIT);
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "Disconnected");
            xEventGroupClearBits(client->status_bits, websocketCONNECTED_BIT);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(LOG_TAG, "Error");
            // TODO handle error
            break;
    }
}

/**
 * @brief  Connect to websocket server
*/
esp_websocket_client_handle_t xWebsocketInitConnection(const esp_websocket_client_config_t *config)
{
    esp_websocket_client_handle_t client = esp_websocket_client_init(config);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    return client;
}

esp_err_t vWebsocketStart(esp_websocket_client_handle_t client)
{
    ESP_LOGI(LOG_TAG, "Connecting to: %s", client->config->uri);
    int result = esp_websocket_client_start(client);
    if (result != ESP_OK)
    {
        return ESP_FAIL;
    }

    // Set timeout
    int timeout = 100 / portTICK_PERIOD_MS;
    EventBits_t state = xEventGroupWaitBits(client->status_bits, websocketCONNECTED_BIT, false, true, timeout);

    if (!(state & websocketCONNECTED_BIT))
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}
