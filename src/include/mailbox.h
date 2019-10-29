#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * Mailbox entity must be used by tasks only
 * Components mush remain clean of business logic
 */
typedef struct xMailboxMessage {
    int length;
    char *value;
} xMailboxMessage;

QueueHandle_t xMailboxIncomingQueue;
QueueHandle_t xMailboxOutcomingQueue;
