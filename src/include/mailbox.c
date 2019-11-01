#include "mailbox.h"

void vMailboxRecieve(xMailboxMsgHandler vMailboxMsgHandler, int taskId)
{
    char *pData;
    if (uxQueueMessagesWaiting(xMailboxIncomingQueue)) {
        BaseType_t xStatus = xQueuePeek(xMailboxIncomingQueue, &pData, 0);
        if (xStatus && *pData == taskId) {
            xQueueReset(xMailboxIncomingQueue);
            vMailboxMsgHandler(pData);
        }
    }
}
