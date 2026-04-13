#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstdio>

#include <dolphin/os/OSThread.h>

#include "__os.h"

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#endif

extern "C" {

static const char* state_name(u16 state) {
    switch (state) {
        case OS_THREAD_STATE_READY:
            return "READY";
        case OS_THREAD_STATE_RUNNING:
            return "RUNNING";
        case OS_THREAD_STATE_WAITING:
            return "WAITING";
        case OS_THREAD_STATE_MORIBUND:
            return "MORIBUND";
        case 0:
            return "DEAD";
        default:
            return "UNKNOWN";
    }
}

static PrivateContext* ctx(OSThread* thread) {
    return reinterpret_cast<PrivateContext*>(thread->context.storage);
}

static BOOL is_stack_addr(const OSThread* thread, uintptr_t addr, size_t bytes) {
    if (thread == nullptr || thread->stackBase == nullptr || thread->stackEnd == nullptr)
        return FALSE;

    uintptr_t low = reinterpret_cast<uintptr_t>(thread->stackEnd);
    uintptr_t high = reinterpret_cast<uintptr_t>(thread->stackBase);
    if (low >= high)
        return FALSE;

    if (addr < low || addr > high)
        return FALSE;

    if (bytes > static_cast<size_t>(high - addr))
        return FALSE;

    return TRUE;
}

static int walk_saved_stack_addresses_cpu(const OSThread* thread, uintptr_t* outAddresses, int maxFrames,
                                          uintptr_t* outEsp, uintptr_t* outEbp) {
    if (outEsp != nullptr)
        *outEsp = 0;

    if (outEbp != nullptr)
        *outEbp = 0;

    assert(outAddresses != nullptr);
    assert(maxFrames > 0);

#if defined(_M_IX86) || defined(__i386__)
    const PrivateContext* context = ctx(const_cast<OSThread*>(thread));

    uintptr_t savedEsp;
    uintptr_t savedEbp;

    if (OSGetCurrentThread() != thread) {
        const auto* regs = reinterpret_cast<const uintptr_t*>(context->coro);
        savedEsp = regs[0];
        savedEbp = regs[1];
    } else {
#if defined(_WIN32)
        CONTEXT captureContext = {};
        RtlCaptureContext(&captureContext);
        savedEsp = captureContext.Esp;
        savedEbp = captureContext.Ebp;
#else
        // TODO: ucontext_t impl
        return 0;
#endif
    }

    if (outEsp != nullptr)
        *outEsp = savedEsp;

    if (outEbp != nullptr)
        *outEbp = savedEbp;

    if (!is_stack_addr(thread, savedEbp, sizeof(uintptr_t) * 2))
        return 0;

    uintptr_t frame = savedEbp;
    int count = 0;
    for (int i = 0; i < maxFrames; ++i) {
        if (!is_stack_addr(thread, frame, sizeof(uintptr_t) * 2))
            break;

        const auto* fp = reinterpret_cast<const uintptr_t*>(frame);
        uintptr_t next = fp[0];
        uintptr_t ret = fp[1];
        outAddresses[count++] = ret;

        if (next == 0 || next <= frame)
            break;

        frame = next;
    }
    return count;
#else
    (void)thread;
    (void)outAddresses;
    (void)maxFrames;
    return 0;
#endif
}

static void print_symbolized_stack_os(const uintptr_t* addresses, int count) {
    if (addresses == nullptr || count <= 0) {
        std::fprintf(stderr, "    no stack frames collected\n");
        return;
    }

#if defined(_WIN32)
    HANDLE process = GetCurrentProcess();
    static BOOL symbolsInitialized = FALSE;
    if (!symbolsInitialized) {
        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
        symbolsInitialized = SymInitialize(process, nullptr, TRUE) ? TRUE : FALSE;
    }

    for (int i = 0; i < count; ++i) {
        DWORD64 displacement = 0;
        DWORD64 address = static_cast<DWORD64>(addresses[i]);

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        if (symbolsInitialized && SymFromAddr(process, address, &displacement, symbol)) {
            IMAGEHLP_LINE64 lineInfo = {};
            lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(process, address, &lineDisplacement, &lineInfo)) {
                std::fprintf(stderr, "    #%02d %p %s+0x%llx (%s:%lu)\n", i, reinterpret_cast<void*>(addresses[i]),
                             symbol->Name, static_cast<unsigned long long>(displacement), lineInfo.FileName,
                             static_cast<unsigned long>(lineInfo.LineNumber));
            } else {
                std::fprintf(stderr, "    #%02d %p %s+0x%llx\n", i, reinterpret_cast<void*>(addresses[i]), symbol->Name,
                             static_cast<unsigned long long>(displacement));
            }
        } else {
            std::fprintf(stderr, "    #%02d %p\n", i, reinterpret_cast<void*>(addresses[i]));
        }
    }

#else
    for (int i = 0; i < count; ++i) {
        std::fprintf(stderr, "    #%02d %p\n", i, reinterpret_cast<void*>(addresses[i]));
    }
#endif
}

static void dump_saved_stack_trace(OSThread* thread, int maxFrames) {
    if (thread == nullptr)
        return;

    uintptr_t savedEsp = 0;
    uintptr_t savedEbp = 0;
    uintptr_t addresses[64] = {};
    if (maxFrames > 64)
        maxFrames = 64;

    int count = walk_saved_stack_addresses_cpu(thread, addresses, maxFrames, &savedEsp, &savedEbp);

#if defined(_M_IX86) || defined(__i386__)
    std::fprintf(stderr, "    saved esp=%p ebp=%p\n", reinterpret_cast<void*>(savedEsp),
                 reinterpret_cast<void*>(savedEbp));
#else
    std::fprintf(stderr, "    stack walk not implemented for this CPU\n");
#endif

    print_symbolized_stack_os(addresses, count);
}

void __OSDumpDeadlockState(OSThread* allThreadsHead, u32 runQueueBits, int runQueueHint, s32 reschedule,
                           OSThread* currentThread) {

    std::fprintf(stderr, "\n[dthread] scheduler deadlock: RunQueueBits=0x%08X hint=%d resched=%d current=%p\n",
                 static_cast<unsigned int>(runQueueBits), runQueueHint, static_cast<int>(reschedule),
                 static_cast<void*>(currentThread));

    int index = 0;
    for (OSThread* thread = allThreadsHead; thread != nullptr;) {
        assert(thread->state <= OS_THREAD_STATE_MORIBUND);

        PrivateContext* context = ctx(thread);
        std::fprintf(stderr,
                     "  [thread %d] self=%p state=%s(%u) prio=%d base=%d suspend=%d attr=0x%04X queue=%p stack=[%p,%p) "
                     "coro=%p\n",
                     index++, static_cast<void*>(thread), state_name(thread->state),
                     static_cast<unsigned int>(thread->state), static_cast<int>(thread->priority),
                     static_cast<int>(thread->base), static_cast<int>(thread->suspend),
                     static_cast<unsigned int>(thread->attr), static_cast<void*>(thread->queue),
                     static_cast<void*>(thread->stackEnd), static_cast<void*>(thread->stackBase), context->coro);

        dump_saved_stack_trace(thread, 32);

        thread = context->debugNext;
    }

    std::fflush(stderr);
}

} // extern "C"
