/**
 * PC stub implementations for Dolphin SDK symbols used by jaudio_NES and others.
 * The real Dolphin SDK is GameCube/PowerPC-specific (inline asm, hardware regs).
 * These stubs allow the PC build to link; audio streaming may be no-op until
 * proper PC implementations exist.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <dolphin/types.h>
#include <dolphin/os/OSAlloc.h>
#include <dolphin/os/OSInterrupt.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSThread.h>
#include <dolphin/os/OSMutex.h>
#include <dolphin/os/OSMessage.h>
#include <dolphin/os/OSTime.h>
#include <dolphin/os/OSReset.h>
#include <dolphin/os/OSAlarm.h>
#include <dolphin/os/OSContext.h>
#include <dolphin/os/OSMemory.h>
#include <dolphin/os/OSError.h>
#include <dolphin/os/OSRtc.h>
#include <dolphin/os/OSFont.h>
#include <dolphin/ai.h>
#include <dolphin/dsp.h>
#include <dolphin/dvd.h>
#include <dolphin/vi.h>
#include <dolphin/ar.h>
#include <dolphin/pad.h>
#include <dolphin/exi.h>
#include <dolphin/card.h>
#include <dolphin/base/PPCArch.h>
#include <GBA/gba.h>
#include <GBA/GBAPriv.h>
#include <jaudio_NES/dummyprobe.h>

#ifdef TARGET_PC

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/* Dummy for boot.c search_partial_address / LoadLink (REL module); NULL = no modules */
// OSModuleHeader* BaseModule = NULL;
GBAControl __GBA[4];
BOOL __GBAReset = FALSE;
CARDControl __CARDBlock[2];

/* Arena for OSGetArenaHi/Lo (dummy range) */
static char arena_lo[1];
static char arena_hi[1];

/* -------------------------------------------------------------------------- */
/* VI retrace emulation                                                       */
/* -------------------------------------------------------------------------- */
static volatile u32 s_viRetraceCount;
static VIRetraceCallback s_viPreRetraceCb;
static VIRetraceCallback s_viPostRetraceCb;
static OSMessageQueue* s_viEventQueue;
static OSMessage s_viEventMessage;
static u32 s_viEventPeriod = 1;
static u32 s_viEventCountdown = 1;
static BOOL s_viAlarmInitialized = FALSE;
static OSAlarm s_viRetraceAlarm;

static void pc_vi_retrace_alarm(OSAlarm* alarm, OSContext* context)
{
    VIRetraceCallback preCb;
    VIRetraceCallback postCb;
    OSMessageQueue* msgQueue;
    OSMessage msg;
    u32 retraceCount;

    (void)alarm;
    (void)context;

    retraceCount = ++s_viRetraceCount;
    preCb = s_viPreRetraceCb;
    postCb = s_viPostRetraceCb;
    msgQueue = s_viEventQueue;
    msg = s_viEventMessage;

    if (preCb != NULL) {
        preCb(retraceCount);
    }

    if (msgQueue != NULL) {
        if (s_viEventCountdown > 1) {
            s_viEventCountdown--;
        } else {
            OSSendMessage(msgQueue, msg, OS_MESSAGE_NOBLOCK);
            s_viEventCountdown = s_viEventPeriod;
        }
    }

    if (postCb != NULL) {
        postCb(retraceCount);
    }
}

static void pc_vi_ensure_started(void)
{
    if (s_viAlarmInitialized == FALSE) {
        OSCreateAlarm(&s_viRetraceAlarm);
        OSSetPeriodicAlarm(&s_viRetraceAlarm, OSGetTime() + OSMicrosecondsToTicks(16667ull),
                           OSMicrosecondsToTicks(16667ull), pc_vi_retrace_alarm);
        s_viAlarmInitialized = TRUE;
    }
}

void __OSPCSetViEvent(OSMessageQueue* msgq, OSMessage msg, u32 retraceCount)
{
    s_viEventQueue = msgq;
    s_viEventMessage = msg;
    s_viEventPeriod = retraceCount == 0 ? 1 : retraceCount;
    s_viEventCountdown = s_viEventPeriod;
    pc_vi_ensure_started();
}

/* -------------------------------------------------------------------------- */
/* libforest ReconfigBATs (PowerPC BAT setup; no-op on PC)                     */
/* -------------------------------------------------------------------------- */
void ReconfigBATs(void)
{
}

/* -------------------------------------------------------------------------- */
/* OS                                                                         */
/* -------------------------------------------------------------------------- */

void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu) { }
BOOL OSEnableInterrupts(void) { return FALSE; }
BOOL OSDisableInterrupts(void) { return FALSE; }
BOOL OSRestoreInterrupts(BOOL level) { return FALSE; }
OSInterruptMask __OSMaskInterrupts(OSInterruptMask mask) { return mask; }
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask) { return mask; }
BOOL OSGetResetSwitchState(void) { return FALSE; }
u32 OSGetResetCode(void) { return 0; }
u32 OSGetConsoleType(void) { return 0; }
void OSGetSaveRegion(void** start, void** end) { if (start) *start = NULL; if (end) *end = NULL; }
u32 OSGetProgressiveMode(void) { return 0; }
void OSSetProgressiveMode(u32 on) { (void)on; }
VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback cb)
{
    VIRetraceCallback old = s_viPreRetraceCb;
    s_viPreRetraceCb = cb;
    pc_vi_ensure_started();
    return old;
}
void LCDisable(void) { }
void OSFillFPUContext(OSContext* context) { (void)context; }
void OSProtectRange(u32 chan, void* addr, u32 nBytes, u32 control)
{
    (void)chan;
    (void)addr;
    (void)nBytes;
    (void)control;
}
__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler)
{
    (void)interrupt;
    (void)handler;
    return NULL;
}
void OSClearContext(OSContext* context) { (void)context; }
void OSSetCurrentContext(OSContext* context) { (void)context; }

u16 OSGetFontEncode(void) { return 0; }
void OSSetStringTable(void* string_table) { (void)string_table; }
BOOL OSLink(OSModuleInfo* newModule, void* bss) { (void)newModule; (void)bss; return TRUE; }
BOOL OSUnlink(OSModuleInfo* module) { (void)module; return TRUE; }
void OSSetSoundMode(u32 mode) { (void)mode; }

/* -------------------------------------------------------------------------- */
/* OS error / context / memory                                                 */
/* -------------------------------------------------------------------------- */
OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler)
{
    (void)error; (void)handler;
    return NULL;
}


/* -------------------------------------------------------------------------- */
/* Cache                                                                      */
/* -------------------------------------------------------------------------- */
void DCStoreRange(void* addr, u32 size) { (void)addr; (void)size; }
void DCStoreRangeNoSync(void* addr, u32 size) { (void)addr; (void)size; }
void DCInvalidateRange(void* addr, u32 size) { (void)addr; (void)size; }
void DCFlushRange(void* addr, u32 size) { (void)addr; (void)size; }
void DCTouchRange(void* addr, u32 size) { (void)addr; (void)size; }
void DCFlushRangeNoSync(void* addr, u32 size) { (void)addr; (void)size; }
void DCZeroRange(void* addr, u32 size) { (void)addr; (void)size; }

/* -------------------------------------------------------------------------- */
/* DSP                                                                        */
/* -------------------------------------------------------------------------- */
u32 DSPCheckMailToDSP(void) { return 0; }
u32 DSPCheckMailFromDSP(void) { return 0; }
u32 DSPReadMailFromDSP(void) { return 0; }
void DSPSendMailToDSP(u32 mail) { (void)mail; }

/* -------------------------------------------------------------------------- */
/* PowerPC (no-op on PC)                                                      */
/* -------------------------------------------------------------------------- */
u32 PPCMfmsr(void) { return 0; }
void PPCMtmsr(u32 value) { (void)value; }

/* -------------------------------------------------------------------------- */
/* VI (PC: retrace count and callbacks driven by background thread)           */
/* -------------------------------------------------------------------------- */
void VIWaitForRetrace(void)
{
    u32 retraceCount;

    pc_vi_ensure_started();
    retraceCount = s_viRetraceCount;
    while (s_viRetraceCount == retraceCount) {
        OSYieldThread();
    }
}
void VIConfigurePan(u16 x, u16 y, u16 w, u16 h) { (void)x;(void)y;(void)w;(void)h; }
void VIConfigure(const GXRenderModeObj* rm) { (void)rm; }
void VISetBlack(BOOL black) { (void)black; }
void VISetNextFrameBuffer(void* fb) { (void)fb; }
u32 VIGetRetraceCount(void)
{
    pc_vi_ensure_started();
    return s_viRetraceCount;
}
/* Return non-zero so initial_menu can progress from logo (step 0) to progressive select (step 1) on B press. */
u32 VIGetDTVStatus(void) { return 1; }

/* -------------------------------------------------------------------------- */
/* AR (ARAM)                                                                  */
/* -------------------------------------------------------------------------- */
/* Real AR/ARQ implementation is provided by aurora::os (AR.cpp) on PC. */
#if 0
u32 ARInit(u32* stack_index_addr, u32 num_entries)
{
    (void)stack_index_addr; (void)num_entries;
    return 0;
}
void ARQInit(void) {}
u32 ARGetSize(void) { return 0; }
u32 ARAlloc(u32 length) { (void)length; return 0; }
void ARQPostRequest(ARQRequest* task, u32 owner, u32 type, u32 priority,
    uintptr_t source, uintptr_t dest, u32 length, ARQCallback callback)
{
    (void)task; (void)owner; (void)type; (void)priority;
    (void)source; (void)dest; (void)length; (void)callback;
}
#endif

void AISetDSPSampleRate(u32 rate) { (void)rate; }
void (*AIRegisterDMACallback(void (*callback)(void)))(void) { (void)callback; return NULL; }
void AIStartDMA(void) { }
u32 ARGetBaseAddress(void) { return 0x4000; }
void PPCSync(void) { }
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback cb)
{
    VIRetraceCallback old = s_viPostRetraceCb;
    s_viPostRetraceCb = cb;
    pc_vi_ensure_started();
    return old;
}

/* -------------------------------------------------------------------------- */
/* GBA                                                                        */
/* -------------------------------------------------------------------------- */
void __GBAX01(s32 chan, s32 ret)
{
    (void)chan;
    (void)ret;
}
s32 GBAGetStatus(s32 chan, u8* status)
{
    (void)chan;
    if (status) {
        *status = 0;
    }
    return GBA_NOT_READY;
}
s32 GBAGetProcessStatus(s32 chan, u8* percentp)
{
    (void)chan;
    if (percentp) {
        *percentp = 0;
    }
    return GBA_NOT_READY;
}
s32 GBAJoyBootAsync(s32 chan, s32 palette_color, s32 palette_speed, u8* programp, s32 length, u8* status, GBACallback callback)
{
    (void)chan;
    (void)palette_color;
    (void)palette_speed;
    (void)programp;
    (void)length;
    if (status) {
        *status = 0;
    }
    if (callback) {
        callback(chan, GBA_NOT_READY);
    }
    return GBA_NOT_READY;
}
s32 GBAWrite(s32 chan, u8* src, u8* status)
{
    (void)chan;
    (void)src;
    if (status) {
        *status = 0;
    }
    return GBA_NOT_READY;
}
s32 GBAReset(s32 chan, u8* status)
{
    (void)chan;
    if (status) {
        *status = 0;
    }
    return GBA_NOT_READY;
}
s32 GBARead(s32 chan, u8* dst, u8* status)
{
    (void)chan;
    if (dst) {
        memset(dst, 0, 4);
    }
    if (status) {
        *status = 0;
    }
    return GBA_NOT_READY;
}

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
/* Misc game/system hooks                                                      */
/* -------------------------------------------------------------------------- */
u32 OSGetSoundMode(void) { return 0; }
void GBAInit(void) {}
int _strip(void) { return 0; }

/* -------------------------------------------------------------------------- */
/* Runtime / compiler stubs (game code references when foresta is linked in)   */
/* -------------------------------------------------------------------------- */
int __abs(int x) { return x < 0 ? -x : x; }
void* __alloca(size_t size) { return malloc(size); }

/* -------------------------------------------------------------------------- */
/* Probe (profiling) — no-op                                                  */
/* -------------------------------------------------------------------------- */
// void Probe_Start(s32 id, const char* label)
// {
//     (void)id;
//     (void)label;
// }

// void Probe_Finish(s32 id)
// {
//     (void)id;
// }

#endif /* TARGET_PC */
