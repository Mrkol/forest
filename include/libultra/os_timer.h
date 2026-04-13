#ifndef LIBULTRA_OS_TIMER_H
#define LIBULTRA_OS_TIMER_H

#include "types.h"
#include <dolphin/os/OSAlarm.h>
#include <dolphin/os/OSTime.h>
#include <dolphin/os/OSTimer.h>
#include <dolphin/os/OSMessage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSTimer_s {
    OSAlarm alarm;
    struct OSTimer_s* next;
    struct OSTimer_s* prev;
    OSTime interval;
    OSTime value;
    OSMessageQueue* mq;
    OSMessage msg;
} OSTimer;

extern int osSetTimer(OSTimer* t, OSTime countdown, OSTime interval, OSMessageQueue* mq, OSMessage msg);
extern void osStopTimerAll(void);

extern OSTimer* __osTimerList;

#ifdef __cplusplus
}
#endif

#endif
