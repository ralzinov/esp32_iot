#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"
#define ENDPOINT_ID 1

void vMessageHandler(xMailboxMessage *pMessage, void *params)
{
    ESP_LOGI(LOG_TAG, "Received=%.*s\r\n", pMessage->length, (char *)pMessage->pData);
    pMessage->onRecieve(pMessage, MSG_OK);
}

void vEndpointTaskGPIO(void *pvParameter)
{
    while(1) {
        vMailboxRecieve(ENDPOINT_ID, vMessageHandler, NULL);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
