#include <cassert>
#include <dolphin/types.h>
#include <dolphin/os/OSMutex.h>

#include "__os.h"

extern "C" {

void OSInitMutex(struct OSMutex* mutex) {
    OSInitThreadQueue(&mutex->queue);
    mutex->thread = 0;
    mutex->count = 0;
}

void OSLockMutex(struct OSMutex* mutex) {
    // BOOL enabled = OSDisableInterrupts();
    struct OSThread* currentThread = OSGetCurrentThread();

    assert(currentThread && "OSLockMutex(): current thread does not exist.");
    assert(currentThread->state == OS_THREAD_STATE_RUNNING && "OSLockMutex(): current thread is not running.");

    while (true) {
        struct OSThread* ownerThread = mutex->thread;
        if (ownerThread == 0) {
            mutex->thread = currentThread;
            mutex->count++;
            ENQUEUE(mutex, &currentThread->queueMutex, link);
            break;
        } else if (ownerThread == currentThread) {
            mutex->count++;
            break;
        } else {
            currentThread->mutex = mutex;
            __OSPromoteThread(mutex->thread, currentThread->priority);
            OSSleepThread(&mutex->queue);
            currentThread->mutex = nullptr;
        }
    }
    // OSRestoreInterrupts(enabled);
}

void OSUnlockMutex(struct OSMutex* mutex) {
    // BOOL enabled = OSDisableInterrupts();
    struct OSThread* currentThread = OSGetCurrentThread();

    assert(currentThread && "OSUnlockMutex(): current thread does not exist.");
    assert(currentThread->state == 2 && "OSUnlockMutex(): current thread is not running.");
    assert(mutex->thread == currentThread && "OSUnlockMutex(): current thread %p is not the owner of mutex %p.");

    if (mutex->thread == currentThread) {
        if (!--mutex->count) {
            DEQUEUE(mutex, &currentThread->queueMutex, link);
            mutex->thread = 0;

            if (currentThread->priority < currentThread->base) {
                currentThread->priority = __OSGetEffectivePriority(currentThread);
            }
            OSWakeupThread(&mutex->queue);
        }
    }
    // OSRestoreInterrupts(enabled);
}

void __OSUnlockAllMutex(struct OSThread* thread) {
    struct OSMutex* mutex;

    while (thread->queueMutex.head) {
        mutex = thread->queueMutex.head;
        DEQUEUE_HEAD(mutex, &thread->queueMutex, link);
        assert(mutex->thread == thread);
        mutex->count = 0;
        mutex->thread = 0;
        OSWakeupThread(&mutex->queue);
    }
}

int OSTryLockMutex(struct OSMutex* mutex) {
    // BOOL enabled = OSDisableInterrupts();
    struct OSThread* currentThread = OSGetCurrentThread();
    int locked;

    assert(currentThread && "OSTryLockMutex(): current thread does not exist.");
    assert(currentThread->state == 2 && "OSTryLockMutex(): current thread is not running.");

    if (!mutex->thread) {
        mutex->thread = currentThread;
        mutex->count++;
        ENQUEUE(mutex, &currentThread->queueMutex, link);
        locked = 1;
    } else if (mutex->thread == currentThread) {
        mutex->count++;
        locked = 1;
    } else {
        locked = 0;
    }
    // OSRestoreInterrupts(enabled);
    
    return locked;
}

int __OSCheckMutex(struct OSMutex* mutex) {
    struct OSThread* thread;
    struct OSThreadQueue* queue;
    s32 priority;

    priority = 0;
    queue = &mutex->queue;

    if (queue->head != nullptr && queue->head->link.prev != nullptr)
        return 0;

    if (queue->tail != nullptr && queue->tail->link.next != nullptr)
        return 0;

    thread = queue->head;
    while (thread) {
        if (thread->link.next != nullptr && (thread != thread->link.next->link.prev)) {
            return 0;
        }
        if (thread->link.prev != nullptr && (thread != thread->link.prev->link.next)) {
            return 0;
        }
        if (thread->state != 4) {
            return 0;
        }
        if (thread->priority < priority) {
            return 0;
        }
        priority = thread->priority;
        thread = thread->link.next;
    }
    if (mutex->thread) {
        if (mutex->count <= 0)
            return 0;

    } else {
        if (mutex->count != 0)
            return 0;
    }
    return 1;
}

int __OSCheckDeadLock(struct OSThread* thread) {
    struct OSMutex* mutex = thread->mutex;

    while (mutex && mutex->thread) {
        if (mutex->thread == thread)
            return 1;

        mutex = mutex->thread->mutex;
    }
    return 0;
}

int __OSCheckMutexes(struct OSThread* thread) {
    struct OSMutex* mutex = thread->queueMutex.head;

    while (mutex) {
        if (mutex->thread != thread)
            return 0;

        if (__OSCheckMutex(mutex) == 0)
            return 0;

        mutex = mutex->link.next;
    }
    return 1;
}

} // extern "C"
