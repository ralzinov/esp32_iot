#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "tasks/communication/communication.task.h"
#include "tasks/heartbeat/heartbeat.task.h"
#include "tasks/gpio/gpio.h"

void app_main()
{
    xTaskCreate(&vTaskHeartbeat, "vTaskHeartbeat", 4096, NULL, 1, NULL);
    xTaskCreate(&vTaskCommunication, "vTaskCommunication", 4096, NULL, 5, NULL);
    xTaskCreate(&vTaskGPIO, "vTaskGPIO", 4096, NULL, 3, NULL);
}
