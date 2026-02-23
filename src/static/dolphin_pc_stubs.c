/**
 * PC stub implementations for Dolphin SDK symbols used by jaudio_NES and others.
 * The real Dolphin SDK is GameCube/PowerPC-specific (inline asm, hardware regs).
 * These stubs allow the PC build to link; audio streaming may be no-op until
 * proper PC implementations exist.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <dolphin/types.h>
#include <dolphin/os/OSAlloc.h>
#include <dolphin/os/OSInterrupt.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSArena.h>
#include <dolphin/os/OSThread.h>
#include <dolphin/os/OSMutex.h>
#include <dolphin/os/OSMessage.h>
#include <dolphin/os/OSTime.h>
#include <dolphin/os/OSReset.h>
#include <dolphin/os/OSAlarm.h>
#include <dolphin/os/OSContext.h>
#include <dolphin/os/OSMemory.h>
#include <dolphin/os/OSError.h>
#include <dolphin/os/OSLink.h>
#include <dolphin/os/OSRtc.h>
#include <dolphin/os/OSFont.h>
#include <dolphin/ai.h>
#include <dolphin/dvd.h>
#include <dolphin/vi.h>
#include <dolphin/ar.h>
#include <dolphin/pad.h>
#include <dolphin/base/PPCArch.h>
#include <jaudio_NES/dummyprobe.h>

#ifdef TARGET_PC

/* Dummy for boot.c search_partial_address / LoadLink (REL module); NULL = no modules */
OSModuleHeader* BaseModule = NULL;

/* Arena for OSGetArenaHi/Lo (dummy range) */
static char arena_lo[1];
static char arena_hi[1];

/* -------------------------------------------------------------------------- */
/* libforest ReconfigBATs (PowerPC BAT setup; no-op on PC)                     */
/* -------------------------------------------------------------------------- */
void ReconfigBATs(void)
{
}

/* -------------------------------------------------------------------------- */
/* OS arena                                                                   */
/* -------------------------------------------------------------------------- */
void* OSGetArenaHi(void) { return (void*)arena_hi; }
void* OSGetArenaLo(void) { return (void*)arena_lo; }
void OSSetArenaHi(void* p) { (void)p; }
void OSSetArenaLo(void* p) { (void)p; }

/* -------------------------------------------------------------------------- */
/* OS heap (used by OSAlloc macro -> OSAllocFromHeap(__OSCurrHeap, size))     */
/* -------------------------------------------------------------------------- */
volatile int __OSCurrHeap = 0;

void* OSAllocFromHeap(int heap, unsigned long size)
{
    (void)heap;
    return malloc((size_t)size);
}

/* -------------------------------------------------------------------------- */
/* OS interrupts                                                              */
/* -------------------------------------------------------------------------- */
BOOL OSDisableInterrupts(void) { return FALSE; }
BOOL OSEnableInterrupts(void) { return FALSE; }
BOOL OSRestoreInterrupts(BOOL level) { (void)level; return FALSE; }

/* -------------------------------------------------------------------------- */
/* OS time                                                                    */
/* -------------------------------------------------------------------------- */
OSTime OSGetTime(void) { return 0; }

/* -------------------------------------------------------------------------- */
/* OS init / alarm                                                            */
/* -------------------------------------------------------------------------- */
void OSInit(void) {}
void OSInitAlarm(void) {}

void OSCreateAlarm(OSAlarm* a) { (void)a; }
void OSCancelAlarm(OSAlarm* a) { (void)a; }
void OSSetAlarm(OSAlarm* a, OSTime tick, OSAlarmHandler h) { (void)a;(void)tick;(void)h; }

void OSClearContext(OSContext* c) { (void)c; }
void OSSetCurrentContext(OSContext* c) { (void)c; }

BOOL OSJamMessage(OSMessageQueue* q, OSMessage msg, int flags) { (void)q;(void)msg;(void)flags; return TRUE; }

void OSSleepThread(OSThreadQueue* q) { (void)q; }
s32 OSSuspendThread(OSThread* t) { (void)t; return 0; }
OSPriority OSGetThreadPriority(OSThread* t) { (void)t; return 0; }
void OSInitThreadQueue(OSThreadQueue* q) { (void)q; }

OSTick OSGetTick(void) { return 0; }

void* OSInitAlloc(void* start, void* end, int maxHeaps) { (void)start;(void)end;(void)maxHeaps; return NULL; }

void* OSPhysicalToCached(u32 paddr) { (void)paddr; return NULL; }

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt intr, __OSInterruptHandler h) { (void)intr;(void)h; return NULL; }
u32 __OSUnmaskInterrupts(u32 mask) { (void)mask; return 0; }

void DCFlushRange(void* addr, u32 size) { (void)addr;(void)size; }
void DCFlushRangeNoSync(void* addr, u32 size) { (void)addr;(void)size; }
void DCTouchRange(void* addr, u32 len) { (void)addr;(void)len; }
void DCZeroRange(void* addr, u32 size) { (void)addr;(void)size; }

void DVDInit(void) {}
BOOL DVDCheckDisk(void) { return FALSE; }
BOOL DVDCloseDir(DVDDir* dir) { (void)dir; return TRUE; }
s32 DVDConvertPathToEntrynum(char* path) { (void)path; return -1; }
BOOL DVDOpenDir(char* dirName, DVDDir* dir) { (void)dirName;(void)dir; return FALSE; }
BOOL DVDReadDir(DVDDir* dir, DVDDirEntry* entry) { (void)dir;(void)entry; return FALSE; }

void VIInit(void) {}
typedef void (*VIRetraceCallback)(u32);
VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback cb) { (void)cb; return NULL; }
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback cb) { (void)cb; return NULL; }

void PPCSync(void) {}
void LCDisable(void) {}

u32 ARGetBaseAddress(void) { return 0; }

AIDCallback AIRegisterDMACallback(AIDCallback cb) { (void)cb; return NULL; }
void AISetDSPSampleRate(u32 rate) { (void)rate; }
void AIStartDMA(void) {}

/* DSP (avoid pulling in dsp.h which uses vu32) */
u32 DSPCheckMailFromDSP(void) { return 0; }
u32 DSPCheckMailToDSP(void) { return 0; }
u32 DSPReadMailFromDSP(void) { return 0; }
void DSPSendMailToDSP(u32 mail) { (void)mail; }

/* -------------------------------------------------------------------------- */
/* OS thread                                                                  */
/* -------------------------------------------------------------------------- */
u32 OSGetStackPointer(void) { return 0; }
OSThread* OSGetCurrentThread(void) { return NULL; }
void OSExitThread(void* val) { (void)val; for(;;); }
BOOL OSSetThreadPriority(OSThread* t, OSPriority p) { (void)t; (void)p; return TRUE; }
s32 OSEnableScheduler(void) { return 0; }
void OSYieldThread(void) {}

BOOL OSCreateThread(OSThread* thread, void* (*func)(void*), void* param,
    void* stack, u32 stackSize, OSPriority priority, u16 attr)
{
    (void)thread; (void)func; (void)param; (void)stack; (void)stackSize;
    (void)priority; (void)attr;
    return FALSE;
}
s32 OSResumeThread(OSThread* thread) { (void)thread; return 0; }
BOOL OSIsThreadTerminated(OSThread* thread) { (void)thread; return TRUE; }
void OSDetachThread(OSThread* thread) { (void)thread; }
void OSCancelThread(OSThread* thread) { (void)thread; }

/* -------------------------------------------------------------------------- */
/* OS mutex                                                                    */
/* -------------------------------------------------------------------------- */
void OSInitMutex(OSMutex* mutex) { (void)mutex; memset(mutex, 0, sizeof(OSMutex)); }
void OSLockMutex(OSMutex* mutex) { (void)mutex; }
void OSUnlockMutex(OSMutex* mutex) { (void)mutex; }

/* -------------------------------------------------------------------------- */
/* OS message queue                                                           */
/* -------------------------------------------------------------------------- */
void OSInitMessageQueue(OSMessageQueue* queue, OSMessage* msgArray, int msgCount)
{
    (void)queue; (void)msgArray; (void)msgCount;
    if (queue) memset(queue, 0, sizeof(OSMessageQueue));
}
BOOL OSSendMessage(OSMessageQueue* queue, OSMessage msg, int flags)
{
    (void)queue; (void)msg; (void)flags;
    return TRUE;
}
BOOL OSReceiveMessage(OSMessageQueue* queue, OSMessage* msgPtr, int flags)
{
    (void)queue; (void)msgPtr; (void)flags;
    return FALSE;
}

/* -------------------------------------------------------------------------- */
/* OS reset / console                                                         */
/* -------------------------------------------------------------------------- */
u32 OSGetResetCode(void) { return 0; }
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu)
{
    (void)reset; (void)resetCode; (void)forceMenu;
}
BOOL OSGetResetSwitchState(void) { return FALSE; }
void OSGetSaveRegion(void** start, void** end) { if (start) *start = NULL; if (end) *end = NULL; }
u32 OSGetConsoleType(void) { return 0; }

/* -------------------------------------------------------------------------- */
/* OS RTC / font / link                                                        */
/* -------------------------------------------------------------------------- */
u32 OSGetProgressiveMode(void) { return 0; }
void OSSetProgressiveMode(u32 on) { (void)on; }
void __OSSetBootMode(u8 mode) { (void)mode; }
BOOL __OSSyncSram(void) { return TRUE; }
u16 OSGetFontEncode(void) { return 0; }
void OSSetStringTable(const void* string_table) { (void)string_table; }
BOOL OSLink(OSModuleInfo* newModule, void* bss) { (void)newModule; (void)bss; return TRUE; }
BOOL OSUnlink(OSModuleInfo* module) { (void)module; return TRUE; }

/* -------------------------------------------------------------------------- */
/* OS error / context / memory                                                 */
/* -------------------------------------------------------------------------- */
OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler)
{
    (void)error; (void)handler;
    return NULL;
}
void OSFillFPUContext(OSContext* context) { (void)context; }
void OSProtectRange(u32 chan, void* addr, u32 nBytes, u32 control)
{
    (void)chan; (void)addr; (void)nBytes; (void)control;
}
u32 OSGetConsoleSimulatedMemSize(void) { return 0x2000000; /* 32MB */ }

/* -------------------------------------------------------------------------- */
/* Cache                                                                      */
/* -------------------------------------------------------------------------- */
void DCStoreRange(void* addr, u32 size) { (void)addr; (void)size; }
void DCInvalidateRange(void* addr, u32 size) { (void)addr; (void)size; }

/* -------------------------------------------------------------------------- */
/* PowerPC (no-op on PC)                                                      */
/* -------------------------------------------------------------------------- */
u32 PPCMfmsr(void) { return 0; }
void PPCMtmsr(u32 value) { (void)value; }

/* -------------------------------------------------------------------------- */
/* Debug (called from OSPanic path)                                            */
/* -------------------------------------------------------------------------- */
void __debugbreak(void) { __builtin_trap(); }

/* -------------------------------------------------------------------------- */
/* VI                                                                         */
/* -------------------------------------------------------------------------- */
void VIWaitForRetrace(void) {}
void VIConfigurePan(u16 x, u16 y, u16 w, u16 h) { (void)x;(void)y;(void)w;(void)h; }
void VIConfigure(const struct _GXRenderModeObj* rm) { (void)rm; }
void VISetBlack(BOOL black) { (void)black; }
void VISetNextFrameBuffer(void* fb) { (void)fb; }
void VIFlush(void) {}
u32 VIGetRetraceCount(void) { return 0; }
u32 VIGetDTVStatus(void) { return 0; }

/* -------------------------------------------------------------------------- */
/* DVD (extra)                                                                */
/* -------------------------------------------------------------------------- */
DVDDiskID* DVDGetCurrentDiskID(void) { return NULL; }
s32 DVDReadPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio)
{
    (void)fileInfo; (void)addr; (void)length; (void)offset; (void)prio;
    return -1;
}
BOOL DVDFastOpen(s32 entryNum, DVDFileInfo* fileInfo)
{
    (void)entryNum; (void)fileInfo;
    return FALSE;
}
s32 DVDGetCommandBlockStatus(const DVDCommandBlock* block) { (void)block; return 0; }
BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset,
    DVDCallback callback, s32 prio)
{
    (void)fileInfo; (void)addr; (void)length; (void)offset; (void)callback; (void)prio;
    return FALSE;
}

/* -------------------------------------------------------------------------- */
/* AR (ARAM)                                                                  */
/* -------------------------------------------------------------------------- */
u32 ARInit(u32* stack_index_addr, u32 num_entries)
{
    (void)stack_index_addr; (void)num_entries;
    return 0;
}
void ARQInit(void) {}
u32 ARGetSize(void) { return 0; }
u32 ARAlloc(u32 length) { (void)length; return 0; }
void ARQPostRequest(ARQRequest* task, u32 owner, u32 type, u32 priority,
    u32 source, u32 dest, u32 length, ARQCallback callback)
{
    (void)task; (void)owner; (void)type; (void)priority;
    (void)source; (void)dest; (void)length; (void)callback;
}

/* -------------------------------------------------------------------------- */
/* PAD                                                                        */
/* -------------------------------------------------------------------------- */
BOOL PADInit(void) { return TRUE; }
u32 PADRead(PADStatus* status) { if (status) memset(status, 0, sizeof(PADStatus)); return 0; }
void PADClamp(PADStatus* status) { (void)status; }
void PADSetSpec(u32 spec) { (void)spec; }
void PADSetAnalogMode(u32 mode) { (void)mode; }
BOOL PADReset(u32 mask) { (void)mask; return TRUE; }
void PADControlMotor(s32 chan, u32 command) { (void)chan; (void)command; }
BOOL PADRecalibrate(u32 mask) { (void)mask; return TRUE; }

/* -------------------------------------------------------------------------- */
/* AI (audio interface)                                                        */
/* -------------------------------------------------------------------------- */
void AIInit(u8* stack)
{
    (void)stack;
}

void AIInitDMA(u32 start_addr, u32 length)
{
    (void)start_addr;
    (void)length;
}

u32 AIGetStreamSampleCount(void)
{
    return 0;
}

void AIResetStreamSampleCount(void) {}

void AISetStreamTrigger(u32 trigger)
{
    (void)trigger;
}

u32 AIGetStreamTrigger(void)
{
    return 0;
}

void AISetStreamPlayState(u32 state)
{
    (void)state;
}

u32 AIGetStreamPlayState(void)
{
    return 0;
}

void AISetStreamSampleRate(u32 rate)
{
    (void)rate;
}

void AISetStreamVolLeft(u8 vol)
{
    (void)vol;
}

void AISetStreamVolRight(u8 vol)
{
    (void)vol;
}

/* -------------------------------------------------------------------------- */
/* DVD (streaming / file) — stubs return failure so stream path gives up      */
/* -------------------------------------------------------------------------- */
BOOL DVDOpen(char* filename, DVDFileInfo* fileInfo)
{
    (void)filename;
    (void)fileInfo;
    return FALSE;
}

BOOL DVDClose(DVDFileInfo* fileInfo)
{
    (void)fileInfo;
    return TRUE;
}

BOOL DVDPrepareStreamAsync(DVDFileInfo* fileInfo, u32 length, u32 offset,
                           DVDCallback callback)
{
    (void)fileInfo;
    (void)length;
    (void)offset;
    (void)callback;
    return FALSE;
}

s32 DVDGetDriveStatus(void)
{
    return 0; /* e.g. DVD_STATE_END */
}

s32 DVDCancelStream(DVDCommandBlock* block)
{
    (void)block;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Probe (profiling) — no-op                                                  */
/* -------------------------------------------------------------------------- */
void Probe_Start(s32 id, const char* label)
{
    (void)id;
    (void)label;
}

void Probe_Finish(s32 id)
{
    (void)id;
}

#endif /* TARGET_PC */
