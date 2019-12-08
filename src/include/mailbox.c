#include "mailbox.h"

void vMailboxInit()
{
    xMailboxIncomingQueue = xQueueCreate(1, sizeof(xMailboxMessage*));
    xMailboxOutcomingQueue = xQueueCreate(1, sizeof(xMailboxMessage*));
}

void vMailboxRecieve(int taskId, xMailboxMsgHandler vMailboxMsgHandler, void *params)
{
    xMailboxMessage *pMessage;
    if (uxQueueMessagesWaiting(xMailboxIncomingQueue)) {
        BaseType_t xStatus = xQueuePeek(xMailboxIncomingQueue, &pMessage, 0);
        if (xStatus && pMessage->taskId == taskId) {
            xQueueReset(xMailboxIncomingQueue);
            vMailboxMsgHandler(pMessage, params);
        }
    }
}
