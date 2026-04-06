#include <dolphin/os/OSAlarm.h>

#include <cassert>

#include "__os.h"

struct OSAlarmQueue {
    OSAlarm* head;
    OSAlarm* tail;
};

static OSAlarmQueue sAlarmQueue = { nullptr, nullptr };

static void remove_alarm(OSAlarm* alarm) {
    OSAlarm* next = alarm->next;

    if (next == nullptr)
        sAlarmQueue.tail = alarm->prev;
    else
        next->prev = alarm->prev;

    if (alarm->prev != nullptr)
        alarm->prev->next = next;
    else
        sAlarmQueue.head = next;

    alarm->prev = nullptr;
    alarm->next = nullptr;
}

static void insert_alarm(OSAlarm* alarm, OSTime fire, OSAlarmHandler handler) {
    OSAlarm* next;

    if (alarm->period > 0) {
        const OSTime now = OSGetTime();
        fire = alarm->start;
        if (fire < now) {
            const OSTime elapsed = now - fire;
            fire += alarm->period * (elapsed / alarm->period + 1);
        }
    }

    alarm->handler = handler;
    alarm->fire = fire;

    for (next = sAlarmQueue.head; next != nullptr; next = next->next) {
        if (next->fire <= fire)
            continue;

        alarm->prev = next->prev;
        alarm->next = next;
        next->prev = alarm;

        if (alarm->prev != nullptr)
            alarm->prev->next = alarm;
        else
            sAlarmQueue.head = alarm;

        return;
    }

    alarm->next = nullptr;
    alarm->prev = sAlarmQueue.tail;

    if (sAlarmQueue.tail != nullptr) {
        sAlarmQueue.tail->next = alarm;
    } else {
        sAlarmQueue.head = alarm;
    }

    sAlarmQueue.tail = alarm;
}

static void fire_due_alarms(void) {
    while (sAlarmQueue.head != nullptr) {
        OSAlarm* alarm = sAlarmQueue.head;
        const OSTime now = OSGetTime();
        if (now < alarm->fire)
            return;

        remove_alarm(alarm);

        OSAlarmHandler handler = alarm->handler;
        alarm->handler = nullptr;

        if (alarm->period > 0 && handler != nullptr) {
            insert_alarm(alarm, 0, handler);
        }

        if (handler != nullptr) {
            OSDisableScheduler();
            OSContext* context = nullptr;
            OSThread* currentThread = OSGetCurrentThread();
            if (currentThread != nullptr)
                context = &currentThread->context;
            handler(alarm, context);
            OSEnableScheduler();
        }
    }
}

extern "C" {

BOOL OSCheckAlarmQueue(void) {
    assert((sAlarmQueue.head == nullptr && sAlarmQueue.tail == nullptr) ||
           (sAlarmQueue.head != nullptr && sAlarmQueue.tail != nullptr));
    assert(sAlarmQueue.head == nullptr || sAlarmQueue.head->prev == nullptr);
    assert(sAlarmQueue.tail == nullptr || sAlarmQueue.tail->next == nullptr);

    for (OSAlarm* alarm = sAlarmQueue.head; alarm != nullptr; alarm = alarm->next) {
        assert(alarm->next == nullptr || alarm->next->prev == alarm);
        assert(alarm->next != nullptr || sAlarmQueue.tail == alarm);
    }

    return TRUE;
}

void OSInitAlarm(void) {
    sAlarmQueue.head = nullptr;
    sAlarmQueue.tail = nullptr;
}

void OSCreateAlarm(OSAlarm* alarm) {
    alarm->handler = nullptr;
    alarm->tag = 0;
    alarm->fire = 0;
    alarm->prev = nullptr;
    alarm->next = nullptr;
    alarm->period = 0;
    alarm->start = 0;
}

void OSSetAlarm(OSAlarm* alarm, OSTime tick, OSAlarmHandler handler) {
    if (alarm->handler != nullptr) {
        OSCancelAlarm(alarm);
    }
    alarm->period = 0;
    alarm->start = 0;
    insert_alarm(alarm, OSGetTime() + tick, handler);
}

void OSSetAbsAlarm(OSAlarm* alarm, OSTime time, OSAlarmHandler handler) {
    if (alarm->handler != nullptr) {
        OSCancelAlarm(alarm);
    }
    alarm->period = 0;
    alarm->start = 0;
    insert_alarm(alarm, time, handler);
}

void OSSetPeriodicAlarm(OSAlarm* alarm, OSTime start, OSTime period, OSAlarmHandler handler) {
    if (alarm->handler != nullptr) {
        OSCancelAlarm(alarm);
    }
    alarm->period = period;
    alarm->start = start;
    insert_alarm(alarm, start, handler);
}

void OSCancelAlarm(OSAlarm* alarm) {
    if (alarm->handler == nullptr) {
        return;
    }

    remove_alarm(alarm);
    alarm->handler = nullptr;
}

void OSSetAlarmTag(OSAlarm* alarm, u32 tag) {
    alarm->tag = tag;
}

void OSCancelAlarms(u32 tag) {
    OSAlarm* alarm = sAlarmQueue.head;

    while (alarm != nullptr) {
        OSAlarm* next = alarm->next;
        if (alarm->tag == tag)
            OSCancelAlarm(alarm);

        alarm = next;
    }
}

OSTime __OSGetNextAlarmFireTime(void) {
    if (sAlarmQueue.head == nullptr) {
        return -1;
    }
    return sAlarmQueue.head->fire;
}

void __OSPollAlarms(void) {
    fire_due_alarms();
}

} // extern "C"
