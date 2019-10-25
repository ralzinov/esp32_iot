#include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "drivers/wifi_sta/wifi_sta.h"
#include "drivers/websocket/websocket.h"
#include "common/common.h"

#include "communication.task.h"

#define c11nSERVER_URI  "ws://192.168.1.8"
#define c11nSERVER_PORT 8080

#define c11nINCOMING_QUEUE_LENGTH 3

void vTaskCommunication(void *pvParameter)
{
    xConnectionStateGroup = xEventGroupCreate();
    // xServerIncomingQueue = xQueueCreate(c11nINCOMING_QUEUE_LENGTH, sizeof(uint32_t));

    char *ssid = "JSF";
    char *pass = "JM6R6DPK";
    vConnectWifi(ssid, pass);
    vWebsocketConnect(c11nSERVER_URI, c11nSERVER_PORT);

    while(1) {
        // uint32_t status = xEventGroupGetBits(xConnectionStateGroup);
        // if (!(status & c11nCONNECTED_SERVER_BIT)) {
        // }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
