#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/status.h"
#include "tasks/tasks.h"

void app_main()
{
    xAppStateEventGroup = xEventGroupCreate();

    xTaskCreate(&vTaskHeartbeat, "vTaskHeartbeat", 4096, NULL, 1, NULL);
    xTaskCreate(&vTaskCommunication, "vTaskCommunication", 4096, NULL, 5, NULL);
    xTaskCreate(&vTaskGPIO, "vTaskGPIO", 4096, NULL, 3, NULL);
}
