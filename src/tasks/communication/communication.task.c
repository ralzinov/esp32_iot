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
static char *mailboxMessage;

static void sendDataToMailboxIncoming(esp_websocket_event_data_t *data)
{
    int length = data->data_len;
    char *prevMailBoxMessage = mailboxMessage;
    mailboxMessage = (char*)malloc(length + 1);
    if (mailboxMessage == NULL) {
        ESP_LOGE(LOG_TAG, "Failed to allocate memory for incoming mailbox message");
        // TODO send NACK "no memory"
        return;
    }

    memset(mailboxMessage, 0, length + 1);
    memcpy(mailboxMessage, data->data_ptr, length);
    if(xQueueSend(xMailboxIncomingQueue, &mailboxMessage, 100 / portTICK_PERIOD_MS)) {
        ESP_LOGI(LOG_TAG, "Incoming message handled");
        // send ACK
    } else {
        xQueueReset(xMailboxIncomingQueue);
        ESP_LOGE(LOG_TAG, "Incoming message is unhandled");
        // send NACK "message is unhandled"
    }
    free(prevMailBoxMessage);
}

static void vWebsocketEventHandler(void *xHandlerArgs, esp_event_base_t base, int32_t eventId, void *xEventData)
{
    switch (eventId) {
        case WEBSOCKET_EVENT_DATA:
            sendDataToMailboxIncoming((esp_websocket_event_data_t *)xEventData);
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
