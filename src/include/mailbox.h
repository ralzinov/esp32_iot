#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define MSG_OK                       0
#define ERR_NOT_ENOUGH_MEMORY        1
#define ERR_NOT_RECIEVED             2
#define ERR_UNSUPORTED_MESSAGE_TYPE  3

/**
 * Note: Mailbox entity must be used by endpoint tasks only
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
    char *pData;
    int length;
    int messageId;
    int messageType;
    int endpointId;
};

typedef void (*xMailboxMsgHandler)(xMailboxMessage *pMessage, void *params);
void vMailboxRecieve(int endpointId, xMailboxMsgHandler vMailboxMsgHandler, void *params);
void vMailboxInit();
