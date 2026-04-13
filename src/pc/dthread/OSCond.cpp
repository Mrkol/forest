#include <cassert>
#include <dolphin/os/OSMutex.h>

#include "__os.h"

extern "C" {

void OSInitCond(struct OSCond* cond) {
    OSInitThreadQueue(&cond->queue);
}

void OSWaitCond(struct OSCond* cond, struct OSMutex* mutex) {
    // BOOL enabled = OSDisableInterrupts();
    struct OSThread* currentThread = OSGetCurrentThread();

    assert(currentThread && "OSWaitCond(): current thread does not exist.");
    assert(currentThread->state == 2 && "OSWaitCond(): current thread is not running.");
    assert(mutex->thread == currentThread && "OSWaitCond(): current thread %p is not the owner of mutex %p.");

    if (mutex->thread == currentThread) {
        s32 count = mutex->count;
        mutex->count = 0;
        DEQUEUE(mutex, &currentThread->queueMutex, link);
        mutex->thread = 0;
        if (currentThread->priority < currentThread->base) {
            currentThread->priority = __OSGetEffectivePriority(currentThread);
        }
        OSDisableScheduler();
        OSWakeupThread(&mutex->queue);
        OSEnableScheduler();
        OSSleepThread(&cond->queue);
        OSLockMutex(mutex);
        mutex->count = count;
    }
    // OSRestoreInterrupts(enabled);
}

void OSSignalCond(struct OSCond* cond) {
    OSWakeupThread(&cond->queue);
}
}
