#ifndef DTHREAD_STATE_H
#define DTHREAD_STATE_H

#include <dolphin/types.h>
#include <dolphin/os/OSTime.h>
#include <libco.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSThread OSThread;
typedef struct OSThreadQueue OSThreadQueue;

typedef struct {
    cothread_t coro;
    void* (*entryFunc)(void*);
    void* entryParam;
    OSThread* debugPrev;
    OSThread* debugNext;
#ifdef FOREST_ASAN
    const void* stack;
    size_t stackSize;
#endif
} PrivateContext;

void __OSPromoteThread(OSThread* thread, s32 priority);
s32 __OSGetEffectivePriority(OSThread* thread);
int __OSCheckDeadLock(OSThread* thread);
int __OSCheckMutexes(struct OSThread* thread);
void __OSDumpDeadlockState(OSThread* allThreadsHead, u32 runQueueBits, int runQueueHint, s32 reschedule,
                           OSThread* currentThread);
OSTime __OSGetNextAlarmFireTime(void);
void __OSPollAlarms(void);

#define ENQUEUE(elem, queue, link)      \
    do {                                \
        auto* __prev = (queue)->tail;   \
        if (__prev == nullptr)          \
            (queue)->head = (elem);     \
        else                            \
            __prev->link.next = (elem); \
        (elem)->link.prev = __prev;     \
        (elem)->link.next = 0;          \
        (queue)->tail = (elem);         \
    } while (0)

#define DEQUEUE(elem, queue, link)        \
    do {                                  \
        auto* __next = (elem)->link.next; \
        auto* __prev = (elem)->link.prev; \
        if (__next == nullptr)            \
            (queue)->tail = __prev;       \
        else                              \
            __next->link.prev = __prev;   \
        if (__prev == nullptr)            \
            (queue)->head = __next;       \
        else                              \
            __prev->link.next = __next;   \
    } while (0)

#define ENQUEUE_PRIO(elem, queue, link)                                                                            \
    do {                                                                                                           \
        decltype((queue)->head) __prev;                                                                            \
        decltype((queue)->head) __next;                                                                            \
        for (__next = (queue)->head; __next && (__next->priority <= (elem)->priority); __next = __next->link.next) \
            ;                                                                                                      \
        if (__next == nullptr) {                                                                                   \
            ENQUEUE(elem, queue, link);                                                                            \
        } else {                                                                                                   \
            (elem)->link.next = __next;                                                                            \
            __prev = __next->link.prev;                                                                            \
            __next->link.prev = (elem);                                                                            \
            (elem)->link.prev = __prev;                                                                            \
            if (__prev == nullptr)                                                                                 \
                (queue)->head = (elem);                                                                            \
            else                                                                                                   \
                __prev->link.next = (elem);                                                                        \
        }                                                                                                          \
    } while (0)

#define DEQUEUE_HEAD(elem, queue, link) \
    do {                                \
        auto* __next = elem->link.next; \
        if (__next == nullptr)          \
            (queue)->tail = 0;          \
        else                            \
            __next->link.prev = 0;      \
        (queue)->head = __next;         \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
