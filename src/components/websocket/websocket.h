#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "esp_transport_ws.h"
// TODO replace with esp-idf implementation when platformio migrates on esp-idf v4
#include "protocols/websocket_client/websocket_client.h"

// Copied from websocket_client.c to be able to use esp_websocket_client_handle_t structure
typedef struct {
    int                         task_stack;
    int                         task_prio;
    char                        *uri;
    char                        *host;
    char                        *path;
    char                        *scheme;
    char                        *username;
    char                        *password;
    int                         port;
    bool                        auto_reconnect;
    void                        *user_context;
    int                         network_timeout_ms;
} websocket_config_storage_t;

// Copied from websocket_client.c to be able to use esp_websocket_client_handle_t structure
typedef enum {
    WEBSOCKET_STATE_ERROR = -1,
    WEBSOCKET_STATE_UNKNOW = 0,
    WEBSOCKET_STATE_INIT,
    WEBSOCKET_STATE_CONNECTED,
    WEBSOCKET_STATE_WAIT_TIMEOUT,
} websocket_client_state_t;

// Copied from websocket_client.c to be able to use esp_websocket_client_handle_t structure
// TODO make pull request after platformio esp-idf v4 migration
struct esp_websocket_client {
    esp_event_loop_handle_t     event_handle;
    esp_transport_list_handle_t transport_list;
    esp_transport_handle_t      transport;
    websocket_config_storage_t *config;
    websocket_client_state_t    state;
    uint64_t                    keepalive_tick_ms;
    uint64_t                    reconnect_tick_ms;
    uint64_t                    ping_tick_ms;
    int                         wait_timeout_ms;
    int                         auto_reconnect;
    bool                        run;
    EventGroupHandle_t          status_bits;
    xSemaphoreHandle            lock;
    char                        *rx_buffer;
    char                        *tx_buffer;
    int                         buffer_size;
};

esp_websocket_client_handle_t xWebsocketInitConnection(const esp_websocket_client_config_t *config);

esp_err_t xWebsocketStart(esp_websocket_client_handle_t client);

esp_err_t xWebsocketSend(esp_websocket_client_handle_t client, const char *data);
