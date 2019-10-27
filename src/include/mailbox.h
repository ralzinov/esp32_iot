#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

QueueHandle_t xAppIncomingMailboxQueue;
QueueHandle_t xAppOutcomingMailboxQueue;
