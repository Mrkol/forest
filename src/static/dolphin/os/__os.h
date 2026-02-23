#ifndef _DOLPHIN_OS_INTERNAL_H_
#define _DOLPHIN_OS_INTERNAL_H_

#include <dolphin/os.h>

// OS.c
extern char* __OSExceptionNames[15]; // D ONLY
extern OSTime __OSStartTime;

u32 __OSIsDebuggerPresent(void);
void __OSPSInit(void);

extern void* BOOT_REGION_START AT_ADDRESS(0x812FDFF0);
extern void* BOOT_REGION_END AT_ADDRESS(0x812FDFEC);

// OSAlloc.c
extern volatile int __OSCurrHeap;

// OSAudioSystem.c
void __OSInitAudioSystem(void);
void __OSStopAudioSystem(void);

// OSCache.c
void __OSCacheInit(void);

// OSContext.c
void __OSContextInit(void);

// OSMemory.c
void __OSInitMemoryProtection(void);

// OSError.c
void __OSUnhandledException(u8 exception, struct OSContext* context,
                            u32 dsisr, u32 dar);

// OSInterrupt.c
extern void __RAS_OSDisableInterrupts_begin(void);
extern void __RAS_OSDisableInterrupts_end(void);

extern u64 __OSSpuriousInterrupts; // D ONLY
extern char* __OSInterruptNames[33];              // D ONLY
extern char* __OSPIErrors[8];                     // D ONLY
extern OSErrorHandlerEx __OSErrorTable[16];

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt,
                                             __OSInterruptHandler handler);
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt);
void __OSInterruptInit(void);
OSInterruptMask __OSMaskInterrupts(OSInterruptMask global);
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask global);
void __OSDispatchInterrupt(__OSException exception, OSContext* context);
void __OSModuleInit(void);

// OSMutex.c
void __OSUnlockAllMutex(struct OSThread* thread);
int __OSCheckDeadLock(struct OSThread* thread);
int __OSCheckMutexes(struct OSThread* thread);

// OSResetSW.c
void __OSResetSWInterruptHandler(__OSInterrupt exception,
                                 struct OSContext* context);

// OSReset.c
void __OSDoHotReset(s32);

// OSReboot.c
void __OSReboot(u32, u32);

// OSRtc.c
int __OSGetRTC(u32* rtc);
int __OSSetRTC(u32 rtc);
void __OSInitSram();
struct OSSram* __OSLockSram(void);
struct OSSramEx* __OSLockSramEx(void);
int __OSUnlockSram(int commit);
int __OSUnlockSramEx(int commit);
int __OSSyncSram(void);
int __OSCheckSram(void);
int __OSReadROM(void* buffer, s32 length, s32 offset);
int __OSReadROMAsync(void* buffer, s32 length, s32 offset,
                     void (*callback)());
u8 __OSGetBootMode(void);
void __OSSetBootMode(u8 ntd);

// OSSync.c
extern void __OSSystemCallVectorStart();
extern void __OSSystemCallVectorEnd();

void __OSInitSystemCall(void);

// OSThread.c
void __OSThreadInit(void);
s32 __OSGetEffectivePriority(struct OSThread* thread);
void __OSPromoteThread(struct OSThread* thread, s32 priority);
void __OSReschedule(void);

// OSTime.c
void __OSSetTime(s32 s32 time);
s32 s32 __OSGetSystemTime();
void __OSSetTick(register u32 newTicks);

extern u32 __OSGetDIConfig(void);

// ppc_eabi_init.c
__declspec(section ".init") asm void __init_hardware(void);
__declspec(section ".init") asm void __flush_cache(void* address,
                                                   unsigned int size);
void __init_user(void);
void __init_cpp(void);
void __fini_cpp(void);
void _ExitProcess(void);

// start.c
void __start(void);

__declspec(section ".init") extern void __start(void);

// time.dolphin.c
s32 s32 __get_clock(void);
u32 __get_time(void);
int __to_gm_time(void);

#endif // _DOLPHIN_OS_INTERNAL_H_
