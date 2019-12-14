#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"
#define ENDPOINT_ID 1

#define GPIO_SET_INPUT      1
#define GPIO_SET_HIGH       2
#define GPIO_SET_LOW        3
#define GPIO_GET_STATE      4
#define GPIO_RESET          5

#define GPIO_NUMBER_INDEX   0

#define GPIO_NUMBER(pData) *(pData + GPIO_NUMBER_INDEX)

/**
 * Data format:
 * |      DATA    |
 * |  GPIO_NUMBER |
 * |    1 byte    |
 */

// GPIO_IS_VALID_GPIO

static void writeLevel(int pin, int level)
{
    gpio_reset_pin(pin);
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, level);
}

void vMessageHandler(xMailboxMessage *pMessage, void *params)
{
    char *pData = pMessage->pData;
    ESP_LOGI(LOG_TAG, "Pin: %d, level: %d", *pData, *(pData + 1));


    switch(pMessage->messageType) {
        case GPIO_SET_INPUT: {
            ESP_LOGI(LOG_TAG, "Set %d pin to input", GPIO_NUMBER(pData));
            break;
        }

        case GPIO_SET_HIGH: {
            ESP_LOGI(LOG_TAG, "Set %d pin HIGH", GPIO_NUMBER(pData));
            writeLevel(GPIO_NUMBER(pData), true);
            break;
        }

        case GPIO_SET_LOW: {
            ESP_LOGI(LOG_TAG, "Set %d pin LOW", GPIO_NUMBER(pData));
            writeLevel(GPIO_NUMBER(pData), false);
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
