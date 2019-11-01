#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "include/mailbox.h"

#define LOG_TAG "[gpio]"
#define TASK_ID 'a'

void vMessageHandler(char *pcData)
{
    ESP_LOGW(LOG_TAG, "Received=%s\n", pcData);
}

void vTaskGPIO(void *pvParameter)
{
    while(1) {
        vMailboxRecieve(vMessageHandler, TASK_ID);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
