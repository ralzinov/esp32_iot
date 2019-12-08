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

#define C11N_MESSAGE_HEADER_LENGTH  2

static esp_websocket_client_handle_t xConnectionHandle;

/**
 * Message format:
 * [   TASK_ID    ][  STATUS  ][    BODY    ]
 * |   1 byte     ||  1byte   ||   n bytes  |
 */

static void *clone(void *dataPointer, int length)
{
    char *str = malloc(length);
    if (str != NULL) {
        memcpy(str, dataPointer, length);
    }
    return str;
}

static void sendStatus(int taskId, int status)
{
    ESP_LOGE(LOG_TAG, "Send status %d Task %d", status, taskId);

    const char messageBody[] = {taskId, status};
    if (esp_websocket_client_send(xConnectionHandle, messageBody, C11N_MESSAGE_HEADER_LENGTH, 100) == ESP_FAIL) {
        ESP_LOGE(LOG_TAG, "Task %d failed to respond with status %d", taskId, status);
        // TODO handle
    }
}

static void onMessageRecieved(xMailboxMessage *pMessage, int status)
{
    sendStatus(pMessage->id, status);
    free(pMessage->pData);
    free(pMessage);
}

static xMailboxMessage *getMessage(esp_websocket_event_data_t *data)
{
    xMailboxMessage *pMessage = malloc(sizeof(xMailboxMessage));
    if (pMessage != NULL) {
        char *pDataBody = ((char *)data->data_ptr) + C11N_MESSAGE_HEADER_LENGTH;
        int dataLength = data->data_len - C11N_MESSAGE_HEADER_LENGTH;
        // ESP_LOGI(LOG_TAG, "body: [%.*s]: %d\r\n", data->data_len, (char*)data->data_ptr, data->data_len);

        void *pData = clone(pDataBody, dataLength);
        if (pData != NULL) {
            (*pMessage).id = *data->data_ptr;
            (*pMessage).pData = pData;
            (*pMessage).length = dataLength;
            (*pMessage).onRecieve = onMessageRecieved;
        }
    }
    return pMessage;
}

static void sendDataToMailboxIncoming(esp_websocket_event_data_t *data)
{
    xMailboxMessage *pMessage = getMessage(data);
    if (!pMessage) {
        ESP_LOGE(LOG_TAG, "Failed to allocate memory for incoming mailbox message");
        sendStatus(*(int *)data->data_ptr, STATUS_FAILED_TO_ALLOCATE_MEMORY);
        return;
    }

    // ESP_LOGI(LOG_TAG, "Id: %d\r\n", pMessage->id);
    // ESP_LOGI(LOG_TAG, "Data: %.*s\r\n", pMessage->length, (char*)pMessage->pData);

    if (uxQueueMessagesWaiting(xMailboxIncomingQueue)) {
        xMailboxMessage *prevMessage;
        if (xQueueReceive(xMailboxIncomingQueue, &prevMessage, 0)) {
            prevMessage->onRecieve(prevMessage, STATUS_MESSAGE_IS_UNHANDLED);
        } else {
            xQueueReset(xMailboxIncomingQueue);
            // TODO handle case
        }
    }

    if(!xQueueSend(xMailboxIncomingQueue, &pMessage, 100 / portTICK_PERIOD_MS)) {
        pMessage->onRecieve(pMessage, STATUS_MESSAGE_FAILED_TO_DELIVER);
    }
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
    return xWebsocketStart(xConnectionHandle);
}

// Server connection
void vTaskCommunication(void *pvParameters)
{
    ESP_ERROR_CHECK(xConnect((xC11nTaskParameters *)pvParameters));

    while(1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
