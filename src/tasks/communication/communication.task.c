#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "include/state.h"
#include "include/mailbox.h"
#include "components/websocket/websocket.h"
#include "components/wifi_sta/wifi_sta.h"
#include "communication.task.h"

#define LOG_TAG "[c11n]"

static esp_websocket_client_handle_t xConnectionHandle;
static struct xMailboxMessage message;
static void *pxMailboxMessagePointer = &message;

static void vWebsocketEventHandler(void *xHandlerArgs, esp_event_base_t base, int32_t eventId, void *xEventData)
{
    // esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)xHandlerArgs;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)xEventData;

    switch (eventId) {
        case WEBSOCKET_EVENT_DATA:
            message.length = data->data_len;
            message.value = (char*)data->data_ptr;
            xQueueSend(xMailboxIncomingQueue, &pxMailboxMessagePointer, portMAX_DELAY);
            break;

        case WEBSOCKET_EVENT_CONNECTED:
            xEventGroupSetBits(xAppStateEventGroup, statusCONNECTED_TO_SERVER);
            break;

        case WEBSOCKET_EVENT_DISCONNECTED:
            xEventGroupClearBits(xAppStateEventGroup, statusCONNECTED_TO_SERVER);
            break;
    }
}

static esp_err_t xConnect(xC11nTaskParameters *config)
{
    vConnectWifi(config->ssid, config->password);
    esp_websocket_client_config_t xConectionConfig = {};
    xConectionConfig.uri = config->uri;
    xConectionConfig.port = config->port;
    xConnectionHandle = xWebsocketInitConnection(&xConectionConfig);
    esp_websocket_register_events(xConnectionHandle, WEBSOCKET_EVENT_ANY, vWebsocketEventHandler, (void *)xConnectionHandle);
    return vWebsocketStart(xConnectionHandle);
}

// Server connection
void vTaskCommunication(void *pvParameters)
{
    ESP_ERROR_CHECK(xConnect((xC11nTaskParameters *)pvParameters));

    while(1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
