#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LOG_TAG "[gpio]"

void vTaskGPIO(void *pvParameter)
{
    while(1) {
        // int length = uxQueueMessagesWaiting(xServerIncomingQueue);
        // if (length > 0)
        // {
        //     printf("Got %d messages", length);

        //     struct xMessage *data;
        //     BaseType_t xStatus = xQueueReceive(xServerIncomingQueue, &data, 0);
        //     if (xStatus)
        //     {
        //         ESP_LOGI(LOG_TAG, "Got data %d length\r\n", data->length);
        //         ESP_LOGW(LOG_TAG, "Received=%.*s\r\n", data->length, (char*)data->value);
        //     }
        // }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
