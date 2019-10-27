#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
// TODO replace with esp-idf implementation when platformio migrates on esp-idf v4
#include "protocols/websocket_client/websocket_client.h"

#define websocketSTOPPED_BIT    BIT0
#define websocketCONNECTED_BIT  BIT1

esp_websocket_client_handle_t xWebsocketInitConnection(const esp_websocket_client_config_t *config);
esp_err_t vWebsocketStart(esp_websocket_client_handle_t client);
