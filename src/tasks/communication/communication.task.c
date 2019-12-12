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

/**
 * Message format:
 * |                        HEADER                      |    BODY     |
 * |   ENDPOINT_ID    |   MESSAGE_ID   |  MESSAGE_TYPE  |    DATA     |
 * |     1 byte       |     1byte      |     1byte      |   n bytes   |
 */
#define C11N_MESSAGE_HEADER_LENGTH 3
#define C11N_MESSAGE_HEADER_ENDPOINT_ID_INDEX   0
#define C11N_MESSAGE_HEADER_MESSAGE_ID_INDEX    1
#define C11N_MESSAGE_HEADER_MESSAGE_TYPE_INDEX  2

#define ENDPOINT_ID(v)   *(v->data_ptr + C11N_MESSAGE_HEADER_ENDPOINT_ID_INDEX)
#define MESSAGE_TYPE(v)  *(v->data_ptr + C11N_MESSAGE_HEADER_MESSAGE_TYPE_INDEX)
#define MESSAGE_ID(v)    *(v->data_ptr + C11N_MESSAGE_HEADER_MESSAGE_ID_INDEX)
#define DATA_BODY_PTR(v)  (char *)v->data_ptr + C11N_MESSAGE_HEADER_LENGTH
#define DATA_LENGTH(v)     v->data_len - C11N_MESSAGE_HEADER_LENGTH

static esp_websocket_client_handle_t xConnectionHandle;

static char *clone(char *dataPointer, int length)
{
    char *str = malloc(length);
    if (str != NULL) {
        memcpy(str, dataPointer, length);
    }
    return str;
}

static void sendStatus(int endpointId, int messageId, int status)
{
    const char messageBody[] = {endpointId, status, messageId};
    if (esp_websocket_client_send(xConnectionHandle, messageBody, C11N_MESSAGE_HEADER_LENGTH, 100) == ESP_FAIL) {
        ESP_LOGE(LOG_TAG, "Endpoint %d failed to respond with status %d to message #%d", endpointId, status, messageId);
        // TODO handle
    }
}

static void onMessageRecieved(xMailboxMessage *pMessage, int status)
{
    sendStatus(pMessage->endpointId, pMessage->messageId, status);
    free(pMessage->pData);
    free(pMessage);
}

static xMailboxMessage *parseMessage(esp_websocket_event_data_t *data)
{
    xMailboxMessage *pMessage = malloc(sizeof(xMailboxMessage));
    if (pMessage != NULL) {
        int dataLength = DATA_LENGTH(data);
        char *pDataBody = DATA_BODY_PTR(data);
        char *pData = clone(pDataBody, dataLength);

        if (pData != NULL) {
            (*pMessage).endpointId = ENDPOINT_ID(data);
            (*pMessage).messageType = MESSAGE_TYPE(data);
            (*pMessage).messageId = MESSAGE_ID(data);
            (*pMessage).pData = pData;
            (*pMessage).length = dataLength;
            (*pMessage).onRecieve = onMessageRecieved;
        }
    }
    return pMessage;
}

static void sendDataToMailboxIncoming(esp_websocket_event_data_t *data)
{
    xMailboxMessage *pMessage = parseMessage(data);
    if (!pMessage) {
        ESP_LOGE(LOG_TAG, "Failed to allocate memory for incoming mailbox message");
        sendStatus(ENDPOINT_ID(data), MESSAGE_ID(data), ERR_NOT_ENOUGH_MEMORY);
        return;
    }

    // ESP_LOGI(LOG_TAG, "Data: %.*s\r\n", pMessage->length, (char*)pMessage->pData);

    if (uxQueueMessagesWaiting(xMailboxIncomingQueue)) {
        xMailboxMessage *prevMessage;
        if (xQueueReceive(xMailboxIncomingQueue, &prevMessage, 0)) {
            prevMessage->onRecieve(prevMessage, ERR_NOT_RECIEVED);
        } else {
            xQueueReset(xMailboxIncomingQueue);
            // TODO handle case
        }
    }
    xQueueSend(xMailboxIncomingQueue, &pMessage, 0);
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
        vTaskDelay(1);
    }
}
