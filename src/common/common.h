// #include "freertos/queue.h"
#include "freertos/event_groups.h"

#define c11nCONNECTED_NETWORK_BIT   BIT0
#define c11nCONNECTED_SERVER_BIT    BIT1

EventGroupHandle_t xConnectionStateGroup;

// QueueHandle_t xServerIncomingQueue;