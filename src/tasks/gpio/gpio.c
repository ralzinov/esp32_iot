#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"
#define TASK_ID 1

void vMessageHandler(xMailboxMessage *pMessage, void *params)
{
    ESP_LOGI(LOG_TAG, "Received=%.*s\r\n", pMessage->length, (char *)pMessage->pData);
    pMessage->onRecieve(pMessage, STATUS_OK);
}

void vTaskGPIO(void *pvParameter)
{
    while(1) {
        vMailboxRecieve(TASK_ID, vMessageHandler, NULL);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
