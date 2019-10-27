#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "drivers/wifi_sta/wifi_sta.h"
#include "drivers/websocket/websocket.h"

#include "communication.task.h"

#define LOG_TAG "[c11n]"
#define c11nSERVER_URI  "ws://192.168.1.8"
#define c11nSERVER_PORT 8080

static esp_websocket_client_handle_t xConnectionHandle;

static esp_err_t connect()
{
    char *ssid = "JSF";
    char *pass = "JM6R6DPK";
    vConnectWifi(ssid, pass);

    int port = c11nSERVER_PORT;
    char *uri = (char*)c11nSERVER_URI;
    const esp_websocket_client_config_t xConectionConfig = {
        .uri = uri,  
		.port = port
	};
    xConnectionHandle = xWebsocketInitConnection(&xConectionConfig);
    vWebsocketStart(xConnectionHandle);
    return ESP_OK;
}

// Server connection
void vTaskCommunication(void *pvParameter)
{
    connect();

    while(1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

        // case WEBSOCKET_EVENT_DATA:
        //     // data.length = ((esp_websocket_event_data_t *)event_data)->data_len;
        //     // data.value = (char*)((esp_websocket_event_data_t *)event_data)->data_ptr;
        //     // void *pxDataPointer = &data;
        //     // xQueueSend(xServerIncomingQueue, &pxDataPointer, 10);
        //     break;