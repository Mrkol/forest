#include <dolphin/os/OSMessage.h>

extern "C" {

void OSInitMessageQueue(OSMessageQueue* mq, OSMessage* msgArray, int msgCount) {
    OSInitThreadQueue(&mq->queueSend);
    OSInitThreadQueue(&mq->queueReceive);
    mq->msgArray = msgArray;
    mq->msgCount = msgCount;
    mq->firstIndex = 0;
    mq->usedCount = 0;
}

BOOL OSSendMessage(OSMessageQueue* mq, OSMessage msg, int flags) {
    // BOOL enabled = OSDisableInterrupts();
    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            // OSRestoreInterrupts(enabled);
            OSYieldThread(); // Cooperate.
            return FALSE;
        }
        OSSleepThread(&mq->queueSend);
    }
    s32 lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
    mq->msgArray[lastIndex] = msg;
    mq->usedCount++;
    OSWakeupThread(&mq->queueReceive);
    // OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL OSReceiveMessage(OSMessageQueue* mq, OSMessage* msg, s32 flags) {
    // BOOL enabled = OSDisableInterrupts();

    while (mq->usedCount == 0) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            // OSRestoreInterrupts(enabled);
            OSYieldThread(); // Cooperate.
            return FALSE;
        }
        OSSleepThread(&mq->queueReceive);
    }
    if (msg != nullptr)
        *msg = mq->msgArray[mq->firstIndex];

    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;
    OSWakeupThread(&mq->queueSend);
    // OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL OSJamMessage(OSMessageQueue* mq, OSMessage msg, s32 flags) {
    // BOOL enabled = OSDisableInterrupts();

    while (mq->msgCount <= mq->usedCount) {
        if (!(flags & OS_MESSAGE_BLOCK)) {
            // OSRestoreInterrupts(enabled);
            OSYieldThread(); // Cooperate.
            return FALSE;
        }
        OSSleepThread(&mq->queueSend);
    }
    mq->firstIndex = (mq->firstIndex + mq->msgCount - 1) % mq->msgCount;
    mq->msgArray[mq->firstIndex] = msg;
    mq->usedCount++;
    OSWakeupThread(&mq->queueReceive);
    // OSRestoreInterrupts(enabled);
    return TRUE;
}
}
