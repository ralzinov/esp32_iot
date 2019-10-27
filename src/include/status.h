#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define statusCONNECTED_TO_NETWORK   BIT0
#define statusCONNECTED_TO_SERVER    BIT1

EventGroupHandle_t xAppStateEventGroup;