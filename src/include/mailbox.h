#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define STATUS_OK                               0
#define STATUS_FAILED_TO_ALLOCATE_MEMORY        1
#define STATUS_MESSAGE_IS_UNHANDLED             2
#define STATUS_MESSAGE_FAILED_TO_DELIVER        3

/**
 * Note: Mailbox entity must be used by tasks only
 * Components mush remain clean of business logic
 */

QueueHandle_t xMailboxIncomingQueue;
QueueHandle_t xMailboxOutcomingQueue;

// Forward declaration
struct xMailboxMessage;
typedef struct xMailboxMessage xMailboxMessage;
typedef void (*xMailboxOnRecieve)(xMailboxMessage *pMessage, int status);
struct xMailboxMessage {
    xMailboxOnRecieve onRecieve;
    void *pData;
    int length;
    int id;
};

typedef void (*xMailboxMsgHandler)(xMailboxMessage *pMessage, void *params);
void vMailboxRecieve(int taskId, xMailboxMsgHandler vMailboxMsgHandler, void *params);
void vMailboxInit();
