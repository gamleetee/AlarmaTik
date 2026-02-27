#pragma once

/*
 * The original project depended on the Flipper Furi SDK.  The headers are
 * not available in this workspace, so we substitute minimal stubs that allow
 * the sources to compile on a host machine.  See `stubs.h` for the actual
 * definitions and prototypes.
 */

#include "stubs.h"

#include "Application.h"

struct F0App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriMutex* mutex;
    FuriTimer* timer;
    NotificationApp* Notificator;
    bool TimerEventFlag;
    float SystemScreenBrightness;
    float SystemLedBrightness;
};
typedef struct F0App F0App;

void UpdateView();
void SetLED(int r, int g, int b, float br);
void SetRLED(float br);
void ResetLED();
void SetScreenBacklightBrightness(int brightness);
bool PinRead(GpioPin pin);
