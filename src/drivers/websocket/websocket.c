#include "esp_log.h"
#include "protocols/websocket_client/websocket_client.h"
#include "common/common.h"

#define LOG_TAG "[ws]"
static esp_websocket_client_handle_t websocket_client;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(LOG_TAG, "Connected");
            xEventGroupSetBits(xConnectionStateGroup, c11nCONNECTED_SERVER_BIT);
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(LOG_TAG, "Disconnected");
            xEventGroupClearBits(xConnectionStateGroup, c11nCONNECTED_SERVER_BIT);
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(LOG_TAG, "Data");
            ESP_LOGW(LOG_TAG, "Received=%.*s\r\n", data->data_len, (char*)data->data_ptr);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(LOG_TAG, "Error");
            break;
    }
}

/**
 * @brief  Connect to websocket server
*/
void vWebsocketConnect(char *uri, int port)
{
    ESP_LOGI(LOG_TAG, "Connecting to %s...", uri);
    const esp_websocket_client_config_t websocket_cfg = {
        .uri = uri,  
		.port = port,
	};

    websocket_client = esp_websocket_client_init(&websocket_cfg);

    esp_websocket_register_events(websocket_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)websocket_client);
    esp_websocket_client_start(websocket_client);
    xEventGroupWaitBits(xConnectionStateGroup, c11nCONNECTED_SERVER_BIT, false, true, portMAX_DELAY);
}
