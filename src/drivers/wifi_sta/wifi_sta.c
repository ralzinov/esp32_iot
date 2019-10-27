#include "string.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "wifi_sta.h"
#include "nvs_flash.h"
// #include "common/common.h"

#define LOG_TAG "[wifi]"

static esp_err_t xEventHandler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            // xEventGroupSetBits(xConnectionStateGroup, c11nCONNECTED_NETWORK_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
            esp_wifi_connect();
            // xEventGroupClearBits(xConnectionStateGroup, c11nCONNECTED_NETWORK_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void vInitNVM()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void vConnectWifi(char *ssid, char *password)
{
    vInitNVM();
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(xEventHandler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t xWifiConfig = {.sta = {}};
        for(int i = 0; i < strlen(ssid); i++) {
        xWifiConfig.sta.ssid[i] = ssid[i];
    }

    for(int i = 0; i < strlen(password); i++) {
        xWifiConfig.sta.password[i] = password[i];
    }

    ESP_LOGI(LOG_TAG, "Setting WiFi configuration SSID %s...", ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &xWifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
    // xEventGroupWaitBits(xConnectionStateGroup, c11nCONNECTED_NETWORK_BIT, false, true, portMAX_DELAY);
}
