#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"
#define ENDPOINT_ID 1

#define GPIO_SET_INPUT      1
#define GPIO_SET_OUTPUT     2
#define GPIO_SET_LEVEL      3
#define GPIO_RESET          4

#define GPIO_NUMBER_INDEX   0
#define GPIO_LEVEL_INDEX    1

#define GPIO_NUMBER(pData) *(pData + GPIO_NUMBER_INDEX)
#define GPIO_LEVEL(pData)  *(pData + GPIO_LEVEL_INDEX)

/**
 * Data format:
 * |            DATA              |
 * |  GPIO_NUMBER |   GPIO_LEVEL  |
 * |    1 byte    |   0-1 bytes   |
 */

void vMessageHandler(xMailboxMessage *pMessage, void *params)
{
    char *pData = pMessage->pData;
    ESP_LOGI(LOG_TAG, "Pin: %d, level: %d", *pData, *(pData + 1));


    switch(pMessage->messageType) {
        case GPIO_SET_INPUT: {
            ESP_LOGI(LOG_TAG, "Set %d pin to input", GPIO_NUMBER(pData));
            break;
        }

        case GPIO_SET_OUTPUT: {
            ESP_LOGI(LOG_TAG, "Set %d pin to output", GPIO_NUMBER(pData));
            break;
        }

        case GPIO_SET_LEVEL: {
            ESP_LOGI(LOG_TAG, "Set %d pin to level %d", GPIO_NUMBER(pData), GPIO_LEVEL(pData));
            break;
        }

        case GPIO_RESET: {
            ESP_LOGI(LOG_TAG, "Reset %d pin", GPIO_NUMBER(pData));
            break;
        }

        default: {
            pMessage->onRecieve(pMessage, ERR_UNSUPORTED_MESSAGE_TYPE);
            return;
        }
    }

    pMessage->onRecieve(pMessage, MSG_OK);
}

void vEndpointTaskGPIO(void *pvParameter)
{
    while(1) {
        vMailboxRecieve(ENDPOINT_ID, vMessageHandler, NULL);
        vTaskDelay(1);
    }
}
