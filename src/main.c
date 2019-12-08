#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/state.h"
#include "include/mailbox.h"
#include "tasks/tasks.h"

void app_main()
{
    vMailboxInit();
    xAppStateEventGroup = xEventGroupCreate();

    static xC11nTaskParameters xC11nTaskParams = {
        .port = 8080,
        .ssid = "JSF",
        .password = "JM6R6DPK",
        .uri = "ws://192.168.1.8",
    };

    xTaskCreate(&vTaskCommunication, "vTaskCommunication", 4096, (void *)&xC11nTaskParams, 5, NULL);
    xTaskCreate(&vTaskHeartbeat, "vTaskHeartbeat", 4096, NULL, 1, NULL);
    xTaskCreate(&vTaskGPIO, "vTaskGPIO", 4096, NULL, 3, NULL);
}

/**
 * TODO
 * pass task ids to params
 * pass task log tag to params
 */