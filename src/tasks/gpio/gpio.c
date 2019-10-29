#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"

void vTaskGPIO(void *pvParameter)
{
    while(1) {
        int length = uxQueueMessagesWaiting(xMailboxIncomingQueue);
        if (length > 0)
        {
            printf("Got %d messages", length);

            struct xMailboxMessage *data;
            BaseType_t xStatus = xQueueReceive(xMailboxIncomingQueue, &data, 0);
            if (xStatus)
            {
                ESP_LOGW(LOG_TAG, "Received=%.*s\r\n", data->length, (char*)data->value);
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
