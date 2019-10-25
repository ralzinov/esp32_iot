#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "tasks/tasks.h"

void app_main()
{
    xTaskCreate(&vTaskHeartbeat, "vTaskHeartbeat", 4096, NULL, 1, NULL);
    xTaskCreate(&vTaskCommunication, "vTaskCommunication", 4096, NULL, 5, NULL);
}
