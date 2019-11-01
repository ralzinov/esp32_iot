#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * Mailbox entity must be used by tasks only
 * Components mush remain clean of business logic
 */

QueueHandle_t xMailboxIncomingQueue;
QueueHandle_t xMailboxOutcomingQueue;

typedef void (*xMailboxMsgHandler)(char* pcEventData);

void vMailboxRecieve(xMailboxMsgHandler vMailboxMsgHandler, int taskId);
