#include <cassert>
#include <chrono>
#include <thread>
#include <dolphin/types.h>
#include <dolphin/os/OSAlarm.h>
#include <dolphin/os/OSThread.h>
#include <dolphin/os/OSMutex.h>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#endif

#if defined(_WIN32)
#include <excpt.h>
#endif

#include "__os.h"

extern "C" {

#define IsSuspended(suspend) (suspend > 0)

// which header should these go in?
extern u8 _stack_end[];
extern char _stack_addr[];

static OSThreadQueue RunQueue[32];
static OSThread IdleThread;
static OSThread DefaultThread;
static volatile u32 RunQueueBits;
static volatile int RunQueueHint;
static s32 Reschedule;
static OSThread* __gCurrentThread;
static OSThreadQueue __OSActiveThreadQueue;
static OSThreadQueue __OSAllThreadsDebug;

#define ALIGN4(val) (((val) + 0x3) & ~0x3)
#define ALIGN8(val) (((val) + 0x7) & ~0x7)

// functions
static void __OSSwitchThread(OSThread* nextThread);
static BOOL __OSIsThreadActive(OSThread* thread);
static void SetRun(OSThread* thread);
static void UnsetRun(OSThread* thread);
static OSThread* SetEffectivePriority(OSThread* thread, s32 priority);
static void UpdatePriority(OSThread* thread);
static OSThread* SelectThread(int yield);
static PrivateContext* ctx(OSThread* thread);

static void sleep_for_alarm_ticks(OSTime ticks) {
    if (ticks <= 0)
        return;

    OSTime micros = OSTicksToMicroseconds(ticks);
    if (micros < 1)
        micros = 1;

    std::this_thread::sleep_for(std::chrono::microseconds(micros));
}

static size_t get_host_page_size() {
#if defined(_WIN32)
    SYSTEM_INFO info = {};
    GetSystemInfo(&info);
    return info.dwPageSize != 0 ? static_cast<size_t>(info.dwPageSize) : 4096u;
#else
    return 4096u;
#endif
}

static void init_main_thread_stack_bounds(OSThread* mainThread) {
    if (mainThread == nullptr) {
        return;
    }

#if defined(_WIN32)
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 != nullptr) {
        using GetCurrentThreadStackLimitsFn = void(WINAPI*)(PULONG_PTR, PULONG_PTR);
        auto getLimits =
            reinterpret_cast<GetCurrentThreadStackLimitsFn>(GetProcAddress(kernel32, "GetCurrentThreadStackLimits"));
        if (getLimits != nullptr) {
            ULONG_PTR low = 0;
            ULONG_PTR high = 0;
            getLimits(&low, &high);
            mainThread->stackEnd = reinterpret_cast<u8*>(low);
            mainThread->stackBase = reinterpret_cast<u8*>(high);
            return;
        }
    }

    NT_TIB* tib = reinterpret_cast<NT_TIB*>(NtCurrentTeb());
    if (tib != nullptr) {
        mainThread->stackEnd = reinterpret_cast<u8*>(tib->StackLimit);
        mainThread->stackBase = reinterpret_cast<u8*>(tib->StackBase);
        return;
    }
#endif

    mainThread->stackBase = nullptr;
    mainThread->stackEnd = nullptr;
}

static_assert(sizeof(PrivateContext) <= sizeof(((OSContext*)0)->storage),
              "PrivateContext must fit in OSThread's context storage");
static PrivateContext* ctx(OSThread* thread) {
    return reinterpret_cast<PrivateContext*>(thread->context.storage);
}

inline void link_debug(OSThread* thread) {
    auto* prev = __OSAllThreadsDebug.tail;
    if (!prev)
        __OSAllThreadsDebug.head = thread;
    else
        ctx(prev)->debugNext = thread;
    auto* context = ctx(thread);
    context->debugPrev = prev;
    context->debugNext = nullptr;
    __OSAllThreadsDebug.tail = thread;
}

static void unlink_debug(OSThread* thread) {
    auto* next = ctx(thread)->debugNext;
    auto* prev = ctx(thread)->debugPrev;

    if (next == nullptr)
        __OSAllThreadsDebug.tail = prev;
    else
        ctx(next)->debugPrev = prev;

    if (prev == nullptr)
        __OSAllThreadsDebug.head = next;
    else
        ctx(prev)->debugNext = next;

    ctx(thread)->debugPrev = nullptr;
    ctx(thread)->debugNext = nullptr;
}

void __OSThreadInit() {
    OSThread* mainThread = &DefaultThread;

    mainThread->state = OS_THREAD_STATE_RUNNING;
    mainThread->attr = OS_THREAD_ATTR_DETACH;
    mainThread->priority = 0x10;
    mainThread->base = 0x10;
    mainThread->suspend = 0;
    mainThread->val = (void*)-1;
    mainThread->mutex = 0;

    OSInitThreadQueue(&mainThread->queueJoin);

    mainThread->queueMutex.head = mainThread->queueMutex.tail = nullptr;

    PrivateContext* ctx = reinterpret_cast<PrivateContext*>(mainThread->context.storage);
    ctx->coro = co_active();
    ctx->entryFunc = nullptr;
    ctx->entryParam = nullptr;
    ctx->debugPrev = nullptr;
    ctx->debugNext = nullptr;

    init_main_thread_stack_bounds(mainThread);

    // *(u32*)mainThread->stackEnd = OS_THREAD_STACK_MAGIC;

    __gCurrentThread = mainThread;
    RunQueueBits = 0;
    RunQueueHint = 0;

    for (int prio = 0; prio <= 31; prio++)
        OSInitThreadQueue(&RunQueue[prio]);

    OSInitThreadQueue(&__OSActiveThreadQueue);

    ENQUEUE(mainThread, &__OSActiveThreadQueue, linkActive);

    OSInitThreadQueue(&__OSAllThreadsDebug);
    link_debug(mainThread);

    Reschedule = 0;
}

void OSInitThreadQueue(OSThreadQueue* queue) {
    queue->head = queue->tail = nullptr;
}

OSThread* OSGetCurrentThread() {
    return __gCurrentThread;
}

#ifdef FOREST_ASAN
void __sanitizer_start_switch_fiber(void** fake_stack_save, const void* bottom, size_t size);
void __sanitizer_finish_switch_fiber(void* fake_stack_save, const void** bottom_old, size_t* size_old);
#endif

static void __OSSwitchThread(OSThread* nextThread) {
    __gCurrentThread = nextThread;
    assert(ctx(nextThread)->coro);
#ifdef FOREST_ASAN
    void* fakeStackSave;
    __sanitizer_start_switch_fiber(&fakeStackSave, ctx(nextThread)->stack, ctx(nextThread)->stackSize);
#endif
    co_switch(ctx(nextThread)->coro);
#ifdef FOREST_ASAN
    __sanitizer_finish_switch_fiber(&fakeStackSave, &ctx(nextThread)->stack, &ctx(nextThread)->stackSize);
#endif
}

BOOL OSIsThreadTerminated(OSThread* thread) {
    return (thread->state == OS_THREAD_STATE_MORIBUND || thread->state == 0) ? TRUE : FALSE;
}

static BOOL __OSIsThreadActive(OSThread* thread) {
    OSThread* active;

    if (thread->state == 0)
        return FALSE;

    for (active = __OSActiveThreadQueue.head; active; active = active->linkActive.next)
        if (thread == active)
            return TRUE;

    return FALSE;
}

s32 OSDisableScheduler(void) {
    // BOOL enabled = OSDisableInterrupts();
    s32 count = Reschedule;
    Reschedule = count + 1;
    // OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler(void) {
    // BOOL enabled = OSDisableInterrupts();
    s32 count = Reschedule;
    Reschedule = count - 1;
    // OSRestoreInterrupts(enabled);
    return count;
}

static void SetRun(OSThread* thread) {
    assert(!IsSuspended(thread->suspend));
    assert(thread->state == OS_THREAD_STATE_READY);

    assert(OS_PRIORITY_MIN <= thread->priority && thread->priority <= OS_PRIORITY_MAX);

    thread->queue = &RunQueue[thread->priority];

    ENQUEUE(thread, thread->queue, link);

    RunQueueBits |= 1 << (0x1F - thread->priority);
    RunQueueHint = 1;
}

static void UnsetRun(OSThread* thread) {
    OSThreadQueue* queue;

    assert(thread->state == OS_THREAD_STATE_READY);

    assert(OS_PRIORITY_MIN <= thread->priority && thread->priority <= OS_PRIORITY_MAX);
    assert(thread->queue == &RunQueue[thread->priority]);

    queue = thread->queue;

    DEQUEUE(thread, queue, link);

    if (!queue->head) {
        RunQueueBits &= ~(1 << (0x1F - thread->priority));
    }
    thread->queue = nullptr;
}

s32 __OSGetEffectivePriority(OSThread* thread) {
    s32 priority = thread->base;
    struct OSMutex* mutex;

    for (mutex = thread->queueMutex.head; mutex; mutex = mutex->link.next) {
        OSThread* blocked = mutex->queue.head;
        if (blocked && blocked->priority < priority) {
            priority = blocked->priority;
        }
    }
    return priority;
}

static OSThread* SetEffectivePriority(OSThread* thread, s32 priority) {
    assert(!IsSuspended(thread->suspend));

    switch (thread->state) {
        case 1:
            UnsetRun(thread);
            thread->priority = priority;
            SetRun(thread);
            break;
        case 4:
            DEQUEUE(thread, thread->queue, link);
            thread->priority = priority;

            ENQUEUE_PRIO(thread, thread->queue, link);

            if (thread->mutex) {
                assert(thread->mutex->thread);
                return thread->mutex->thread;
            }
            break;
        case 2:
            RunQueueHint = 1;
            thread->priority = priority;
            break;
    }
    return 0;
}

static void UpdatePriority(OSThread* thread) {
    s32 priority;

    while (1) {
        if (thread->suspend > 0) {
            break;
        }
        priority = __OSGetEffectivePriority(thread);
        if (thread->priority == priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
        if (thread == 0) {
            break;
        }
    }
}

void __OSPromoteThread(OSThread* thread, s32 priority) {
    while (1) {
        if (thread->suspend > 0 || thread->priority <= priority) {
            break;
        }
        thread = SetEffectivePriority(thread, priority);
        if (thread == 0) {
            break;
        }
    }
}

static u32 __cntlzw(u32 value) {
    u32 count = 0;
    while (value && (value & 0x80000000) == 0) {
        count++;
        value <<= 1;
    }
    return count;
}

static OSThread* SelectThread(int yield) {
    OSThread* currentThread;
    OSThread* nextThread;
    s32 priority;
    OSThreadQueue* queue;

    if (Reschedule > 0)
        return nullptr;

    currentThread = OSGetCurrentThread();

    if (currentThread) {
        if (currentThread->state == OS_THREAD_STATE_RUNNING) {
            if (yield == 0) {
                priority = __cntlzw(RunQueueBits);
                if (currentThread->priority <= priority)
                    return nullptr;
            }
            currentThread->state = OS_THREAD_STATE_READY;
            SetRun(currentThread);
        }
    }

    // Deliver any alarms that became due before choosing the next runnable thread.
    __OSPollAlarms();

    if (RunQueueBits == 0) {
        while (RunQueueBits == 0) {
            const OSTime nextFire = __OSGetNextAlarmFireTime();
            if (nextFire < 0) {
                __OSDumpDeadlockState(__OSAllThreadsDebug.head, RunQueueBits, RunQueueHint, Reschedule,
                                      __gCurrentThread);
                assert(false && "Nothing to schedule and no alarms pending, deadlock?");
                break;
            }

            const OSTime now = OSGetTime();
            sleep_for_alarm_ticks(nextFire - now);
            __OSPollAlarms();
        }
    }

    __gCurrentThread = nullptr;

    RunQueueHint = 0;
    priority = __cntlzw(RunQueueBits);

    assert(OS_PRIORITY_MIN <= priority && priority <= OS_PRIORITY_MAX);

    queue = &RunQueue[priority];
    nextThread = queue->head;

    DEQUEUE_HEAD(nextThread, queue, link);

    assert(nextThread->priority == priority);

    if (!queue->head)
        RunQueueBits &= ~(1 << (0x1F - priority));

    nextThread->queue = 0;
    nextThread->state = OS_THREAD_STATE_RUNNING;
    __OSSwitchThread(nextThread);
    return nextThread;
}

void __OSReschedule(void) {
    if (RunQueueHint != 0)
        SelectThread(0);
}

void OSYieldThread(void) {
    // BOOL enabled = OSDisableInterrupts();
    SelectThread(1);
    // OSRestoreInterrupts(enabled);
}

static void thread_main(OSThread* thread) {
    try {
        PrivateContext* context = ctx(thread);
        void* result = context->entryFunc(context->entryParam);
        OSExitThread(result);
    } catch (...) {
        thread->error = -1;
        OSExitThread(nullptr);
    }
}

#if defined(_MSC_VER) && defined(_WIN32)
static int trampoline_seh_filter(unsigned int exceptionCode) {
    OSThread* thread = OSGetCurrentThread();
    thread->error = static_cast<s32>(exceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

static void trampoline() {
    OSThread* thread = OSGetCurrentThread();
#ifdef FOREST_ASAN
    __sanitizer_finish_switch_fiber(nullptr, &ctx(thread)->stack, &ctx(thread)->stackSize);
#endif
#if defined(_MSC_VER) && defined(_WIN32)
    __try {
        thread_main(thread);
    } __except (trampoline_seh_filter(_exception_code())) { OSExitThread(nullptr); }
#else
    thread_main(thread);
#endif
#ifdef FOREST_ASAN
    __sanitizer_start_switch_fiber(nullptr, ctx(thread)->stack, ctx(thread)->stackSize);
#endif
}

BOOL OSCreateThread(OSThread* thread, void* (*func)(void*), void* param, void* stack, u32 stackSize, s32 priority,
                    u16 attr) {
    assert((priority >= OS_PRIORITY_MIN && priority <= OS_PRIORITY_MAX) &&
           "OSCreateThread(): priority out of range (0 <= priority <= 31).");

    const size_t pageSize = get_host_page_size();
    assert((stack != nullptr) && "OSCreateThread(): stack must not be null.");
    assert((stackSize % pageSize) == 0 && "OSCreateThread(): stackSize must be page aligned.");
    OSReport("OSCreateThread(): stackSize: %d, pageSize: %d\n", stackSize, pageSize);
    assert(stackSize >= pageSize * 2 && "OSCreateThread(): stackSize must be at least 2 pages.");

    u8* stackTop = static_cast<u8*>(stack);
    u8* guardPage = stackTop - stackSize;
    assert((reinterpret_cast<uintptr_t>(guardPage) % pageSize) == 0 &&
           "OSCreateThread(): stack bottom must be page aligned.");

#if defined(_WIN32)
    {
        DWORD oldProtect = 0;
        BOOL protectOk = VirtualProtect(guardPage, pageSize, PAGE_READONLY, &oldProtect);
        assert(protectOk && "OSCreateThread(): failed to protect guard page.");
    }
#endif

    thread->state = OS_THREAD_STATE_READY;
    thread->attr = attr & 1U;
    thread->base = priority;
    thread->priority = priority;
    thread->suspend = 1;
    thread->val = (void*)-1;
    thread->mutex = 0;
    OSInitThreadQueue(&thread->queueJoin);

    thread->queueMutex.head = thread->queueMutex.tail = nullptr;

    u8* coStackBase = guardPage + pageSize;
    u32 coStackSize = stackSize - static_cast<u32>(pageSize);
    ctx(thread)->coro = co_derive(coStackBase, coStackSize, &trampoline);
    thread->stackBase = stackTop;
    thread->stackEnd = guardPage;

    ctx(thread)->entryFunc = func;
    ctx(thread)->entryParam = param;
    ctx(thread)->debugPrev = nullptr;
    ctx(thread)->debugNext = nullptr;

    // *thread->stackEnd = OS_THREAD_STACK_MAGIC;

    // BOOL enabled = OSDisableInterrupts();

    assert(__OSIsThreadActive(thread) == 0L && "OSCreateThread(): thread is still active.");

    ENQUEUE(thread, &__OSActiveThreadQueue, linkActive);

    link_debug(thread);

    // OSRestoreInterrupts(enabled);
    return TRUE;
}

void OSExitThread(void* val) {
    // BOOL enabled = OSDisableInterrupts();
    OSThread* thread = OSGetCurrentThread();

    assert(thread && "OSExitThread(): current thread does not exist.");
    assert(thread->state == OS_THREAD_STATE_RUNNING && "OSExitThread(): current thread is not running.");
    assert(__OSIsThreadActive(thread) != 0 && "OSExitThread(): current thread is not active.");

    if (thread->attr & OS_THREAD_ATTR_DETACH) {
        DEQUEUE(thread, &__OSActiveThreadQueue, linkActive);
        thread->state = 0;
        unlink_debug(thread);
    } else {
        thread->state = OS_THREAD_STATE_MORIBUND;
        thread->val = val;
        // will be unlinked & deleted in OSJoinThread
    }
    // Well this is extremely dumb lol
    // __OSUnlockAllMutex(currentThread);
    OSWakeupThread(&thread->queueJoin);
    RunQueueHint = 1;

    if (RunQueueHint != 0)
        SelectThread(0);

    // OSRestoreInterrupts(enabled);
}

void OSCancelThread(OSThread* thread) {
    // BOOL enabled = OSDisableInterrupts();

    assert(__OSIsThreadActive(thread) != 0 && "OSExitThread(): thread %p is not active.");

    switch (thread->state) {
        case OS_THREAD_STATE_READY:
            if (thread->suspend <= 0) {
                UnsetRun(thread);
            }
            break;
        case OS_THREAD_STATE_RUNNING:
            RunQueueHint = 1;
            break;
        case OS_THREAD_STATE_WAITING:
            DEQUEUE(thread, thread->queue, link);
            thread->queue = 0;
            if ((thread->suspend <= 0) && (thread->mutex)) {
                assert(thread->mutex->thread);
                UpdatePriority(thread->mutex->thread);
            }
            break;
        default:
            // OSRestoreInterrupts(enabled);
            return;
    }

    if (thread->attr & OS_THREAD_ATTR_DETACH) {
        DEQUEUE(thread, &__OSActiveThreadQueue, linkActive);
        thread->state = 0;
        unlink_debug(thread);
    } else {
        thread->state = OS_THREAD_STATE_MORIBUND;
    }
    // __OSUnlockAllMutex(thread);
    OSWakeupThread(&thread->queueJoin);
    __OSReschedule();
    // OSRestoreInterrupts(enabled);
}

BOOL OSJoinThread(OSThread* thread, void** val) {
    // BOOL enabled = OSDisableInterrupts();

    assert(__OSIsThreadActive(thread) != 0 && "OSJoinThread(): thread %p is not active.");

    if (!(thread->attr & OS_THREAD_ATTR_DETACH) && thread->state != OS_THREAD_STATE_MORIBUND &&
        thread->queueJoin.head == nullptr) {
        OSSleepThread(&thread->queueJoin);
        if (__OSIsThreadActive(thread) == 0) {
            // OSRestoreInterrupts(enabled);
            return FALSE;
        }
    }
    if (thread->state == OS_THREAD_STATE_MORIBUND) {
        if (val)
            *val = thread->val;

        DEQUEUE(thread, &__OSActiveThreadQueue, linkActive);
        thread->state = 0;
        unlink_debug(thread);
        // OSRestoreInterrupts(enabled);
        return TRUE;
    }
    // OSRestoreInterrupts(enabled);
    return FALSE;
}

void OSDetachThread(OSThread* thread) {
    // BOOL enabled = OSDisableInterrupts();

    assert(__OSIsThreadActive(thread) != 0 && "OSDetachThread(): thread %p is not active.");

    thread->attr |= OS_THREAD_ATTR_DETACH;
    if (thread->state == OS_THREAD_STATE_MORIBUND) {
        DEQUEUE(thread, &__OSActiveThreadQueue, linkActive);
        thread->state = 0;
        unlink_debug(thread);
    }
    OSWakeupThread(&thread->queueJoin);
    // OSRestoreInterrupts(enabled);
}

s32 OSResumeThread(OSThread* thread) {
    // BOOL enabled = OSDisableInterrupts();
    s32 suspendCount;

    assert(__OSIsThreadActive(thread) != 0 && "OSResumeThread(): thread %p is not active.");
    assert(thread->state != OS_THREAD_STATE_MORIBUND && "OSResumeThread(): thread %p is terminated.");

    suspendCount = thread->suspend--;
    if (thread->suspend < 0) {
        thread->suspend = 0;
    } else if (thread->suspend == 0) {
        switch (thread->state) {
            case OS_THREAD_STATE_READY:
                thread->priority = __OSGetEffectivePriority(thread);
                SetRun(thread);
                break;
            case OS_THREAD_STATE_WAITING:
                assert(thread->queue);
                DEQUEUE(thread, thread->queue, link);
                thread->priority = __OSGetEffectivePriority(thread);
                ENQUEUE_PRIO(thread, thread->queue, link);
                if (thread->mutex) {
                    UpdatePriority(thread->mutex->thread);
                }
        }
        __OSReschedule();
    }
    // OSRestoreInterrupts(enabled);
    return suspendCount;
}

s32 OSSuspendThread(OSThread* thread) {
    // BOOL enabled = OSDisableInterrupts();
    s32 suspendCount;

    assert(__OSIsThreadActive(thread) != 0 && "OSSuspendThread(): thread %p is not active.");
    assert(thread->state != OS_THREAD_STATE_MORIBUND && "OSSuspendThread(): thread %p is terminated.");

    suspendCount = thread->suspend++;
    if (suspendCount == 0) {
        switch (thread->state) {
            case OS_THREAD_STATE_RUNNING:
                RunQueueHint = 1;
                thread->state = OS_THREAD_STATE_READY;
                break;
            case OS_THREAD_STATE_READY:
                UnsetRun(thread);
                break;
            case OS_THREAD_STATE_WAITING:
                DEQUEUE(thread, thread->queue, link);
                thread->priority = 0x20;
                ENQUEUE(thread, thread->queue, link);
                if (thread->mutex) {
                    assert(thread->mutex->thread);
                    UpdatePriority(thread->mutex->thread);
                }
                break;
        }
        __OSReschedule();
    }
    // OSRestoreInterrupts(enabled);
    return suspendCount;
}

void OSSleepThread(OSThreadQueue* queue) {
    // BOOL enabled = OSDisableInterrupts();
    OSThread* thread = OSGetCurrentThread();

    assert(thread && "OSSleepThread(): current thread does not exist.");
    assert(__OSIsThreadActive(thread) != 0 && "OSSleepThread(): current thread %p is not active.");
    assert(thread->state == OS_THREAD_STATE_RUNNING && "OSSleepThread(): current thread %p is not running.");
    assert(thread->suspend <= 0 && "OSSleepThread(): current thread %p is suspended.");

    thread->state = OS_THREAD_STATE_WAITING;
    thread->queue = queue;
    ENQUEUE_PRIO(thread, queue, link);
    RunQueueHint = 1;
    __OSReschedule();
    // OSRestoreInterrupts(enabled);
}

void OSWakeupThread(OSThreadQueue* queue) {
    // BOOL enabled = OSDisableInterrupts();

    while (queue->head) {
        OSThread* thread = queue->head;

        DEQUEUE_HEAD(thread, queue, link);

        assert(__OSIsThreadActive(thread));
        assert(thread->state != OS_THREAD_STATE_MORIBUND);
        assert(thread->queue == queue);
        thread->state = OS_THREAD_STATE_READY;
        if (thread->suspend <= 0) {
            SetRun(thread);
        }
    }
    __OSReschedule();
    // OSRestoreInterrupts(enabled);
}

BOOL OSSetThreadPriority(struct OSThread* thread, s32 priority) {
    assert(priority >= OS_PRIORITY_MIN && priority <= OS_PRIORITY_MAX &&
           "OSSetThreadPriority(): priority out of range (0 <= priority <= 31).");

    // BOOL enabled = OSDisableInterrupts();

    assert(__OSIsThreadActive(thread) != 0 && "OSSetThreadPriority(): thread %p is not active.");
    assert(thread->state != OS_THREAD_STATE_MORIBUND && "OSSetThreadPriority(): thread %p is terminated.");

    if (thread->base != priority) {
        thread->base = priority;
        UpdatePriority(thread);
        __OSReschedule();
    }
    // OSRestoreInterrupts(enabled);
    return TRUE;
}

s32 OSGetThreadPriority(OSThread* thread) {
    return thread->base;
}

static BOOL CheckThreadQueue(struct OSThreadQueue* queue) {
    struct OSThread* thread;

    if (queue->head != nullptr && queue->head->link.prev != nullptr)
        return FALSE;

    if (queue->tail != nullptr && queue->tail->link.next != nullptr)
        return FALSE;

    thread = queue->head;
    while (thread) {
        if (thread->link.next != nullptr && thread != thread->link.next->link.prev)
            return FALSE;

        if (thread->link.prev != nullptr && thread != thread->link.prev->link.next)
            return FALSE;

        thread = thread->link.next;
    }
    return TRUE;
}

static BOOL IsMember(struct OSThreadQueue* queue, struct OSThread* thread) {
    struct OSThread* member = queue->head;

    while (member) {
        if (thread == member)
            return TRUE;

        member = member->link.next;
    }
    return FALSE;
}

s32 OSCheckActiveThreads() {
    struct OSThread* thread;
    s32 prio;
    s32 cThread;
    int enabled;

    cThread = 0;
    // BOOL enabled = OSDisableInterrupts();

    for (prio = 0; prio <= 0x1F; prio++) {
        if (RunQueueBits & (1 << (0x1F - prio))) {
            assert(RunQueue[prio].head != nullptr && RunQueue[prio].tail != nullptr);
        } else {
            assert(RunQueue[prio].head == nullptr && RunQueue[prio].tail == nullptr);
        }
        assert(CheckThreadQueue(&RunQueue[prio]));
    }

    assert(__OSActiveThreadQueue.head == nullptr || __OSActiveThreadQueue.head->linkActive.prev == nullptr);
    assert(__OSActiveThreadQueue.tail == nullptr || __OSActiveThreadQueue.tail->linkActive.next == nullptr);

    thread = __OSActiveThreadQueue.head;
    while (thread) {
        cThread++;
        assert(thread->linkActive.next == nullptr || thread == thread->linkActive.next->linkActive.prev);
        assert(thread->linkActive.prev == nullptr || thread == thread->linkActive.prev->linkActive.next);
        // assert(*(thread->stackEnd) == OS_THREAD_STACK_MAGIC);
        assert(OS_PRIORITY_MIN <= thread->priority && thread->priority <= OS_PRIORITY_MAX + 1);
        assert(0 <= thread->suspend);
        assert(__OSActiveThreadQueue.tail == nullptr || __OSActiveThreadQueue.tail->linkActive.next == nullptr);

        thread = __OSActiveThreadQueue.head;
        while (thread) {
            cThread++;
            assert(thread->linkActive.next == nullptr || thread == thread->linkActive.next->linkActive.prev);
            assert(thread->linkActive.prev == nullptr || thread == thread->linkActive.prev->linkActive.next);

            assert(OS_PRIORITY_MIN <= thread->priority && thread->priority <= OS_PRIORITY_MAX + 1);
            assert(0 <= thread->suspend);
            assert(CheckThreadQueue(&thread->queueJoin));

            switch (thread->state) {
                case 1:
                    if (thread->suspend <= 0) {
                        assert(thread->queue == &RunQueue[thread->priority]);
                        assert(IsMember(&RunQueue[thread->priority], thread));
                        assert(thread->priority == __OSGetEffectivePriority(thread));
                    }
                    break;
                case 2:
                    assert(!IsSuspended(thread->suspend));
                    assert(thread->queue == nullptr);
                    assert(thread->priority == __OSGetEffectivePriority(thread));
                    break;
                case 4:
                    assert(thread->queue != nullptr);
                    assert(CheckThreadQueue(thread->queue));
                    assert(IsMember(thread->queue, thread));
                    if (thread->suspend <= 0) {
                        assert(thread->priority == __OSGetEffectivePriority(thread));
                    } else {
                        assert(thread->priority == 32);
                    }
                    assert(!__OSCheckDeadLock(thread));
                    break;
                case 8:
                    assert(thread->queueMutex.head == nullptr && thread->queueMutex.tail == nullptr);
                    break;
                default:
                    assert(false && "OSCheckActiveThreads: Failed. unkown thread state (%d) of thread %p\n");
            }
            assert(__OSCheckMutexes(thread));
            thread = thread->linkActive.next;
        }
        //  OSRestoreInterrupts(enabled);
        return cThread;
    }

    return 0;
}

} // extern "C"
