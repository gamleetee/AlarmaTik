#include "F0App.h"

F0App* FApp = 0;

static void F0App_timer_callback(void* ctx) {
    InputEvent event = {.type = InputTypeMAX, .key = 255};
    furi_message_queue_put((FuriMessageQueue*)ctx, &event, 0);
}

static void F0App_draw_callback(Canvas* canvas, void* ctx) {
    if(furi_mutex_acquire(FApp->mutex, 200) != FuriStatusOk) return;
    Draw(canvas, ctx);
    furi_mutex_release(FApp->mutex);
}

static void F0App_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    furi_message_queue_put((FuriMessageQueue*)ctx, input_event, FuriWaitForever);
}

int32_t F0App_entry(void* p) {
    UNUSED(p);
    F0App* app = malloc(sizeof(F0App));
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    if(app->event_queue == NULL) return 255;

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(app->mutex == NULL) {
        furi_message_queue_free(app->event_queue);
        return 255;
    }
    app->timer = furi_timer_alloc(F0App_timer_callback, FuriTimerTypePeriodic, app->event_queue);
    if(app->timer == NULL) {
        furi_mutex_free(app->mutex);
        furi_message_queue_free(app->event_queue);
        return 255;
    }
    furi_timer_start(app->timer, furi_kernel_get_tick_frequency());

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, F0App_draw_callback, NULL);
    view_port_input_callback_set(app->view_port, F0App_input_callback, app->event_queue);
    app->gui = furi_record_open(RECORD_GUI);
    app->Notificator = furi_record_open(RECORD_NOTIFICATION);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    FApp = app;
// SDK no longer exposes notification settings structure, so we
    // cannot read the current values directly.  Store sane defaults instead.
    app->SystemScreenBrightness = 1.0f; // assume full backlight
    app->SystemLedBrightness = 1.0f;    // assume LEDs at full brightness
    AppInit();
    InputEvent event;
    while(1) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(furi_mutex_acquire(app->mutex, FuriWaitForever) != FuriStatusOk) continue;
            if(KeyProc(event.type, event.key) == 255)
                break;
            else
                UpdateView();
            furi_mutex_release(app->mutex);
        }
    }
    furi_timer_stop(app->timer);
    AppDeinit();
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_message_queue_free(app->event_queue);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_timer_free(app->timer);
    furi_mutex_free(app->mutex);
    free(app);
    return 0;
}

void SetLED(int r, int g, int b, float br) {
    NotificationMessage notification_led_message_1;
    notification_led_message_1.type = NotificationMessageTypeLedRed;
    NotificationMessage notification_led_message_2;
    notification_led_message_2.type = NotificationMessageTypeLedGreen;
    NotificationMessage notification_led_message_3;
    notification_led_message_3.type = NotificationMessageTypeLedBlue;

    notification_led_message_1.data.led.value = r;
    notification_led_message_2.data.led.value = g;
    notification_led_message_3.data.led.value = b;
    const NotificationSequence notification_sequence = {
        &notification_led_message_1,
        &notification_led_message_2,
        &notification_led_message_3,
        &message_do_not_reset,
        NULL,
    };
    /* LED brightness is now controlled via a notification message rather
       than by poking a settings struct.  Map 0.0‑1.0 to 0‑255 and send a
       special message before the color sequence so future color commands use
       the requested brightness. */
    NotificationMessage brightness_msg;
    brightness_msg.type = NotificationMessageTypeLedBrightnessSettingApply;
    /* clamp and convert */
    if(br < 0.f) br = 0.f;
    if(br > 1.f) br = 1.f;
    brightness_msg.data.led.value = (uint8_t)(br * 255.f);
    /* wrap in a one-element sequence */
     /* wrap the single message in a NotificationSequence using the typedef
         so that the pointer-to-array matches what notification_message expects */
     const NotificationSequence seq1 = {&brightness_msg, NULL};
     notification_message(FApp->Notificator, &seq1);

    notification_message(FApp->Notificator, &notification_sequence);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 10);
}

void ResetLED() {
    notification_message(FApp->Notificator, &sequence_reset_rgb);
}

/*brightness in % (0-100)*/
void SetScreenBacklightBrightness(int brightness) {
    /* Forge a message to force the display brightness setting.  The
       notification service will remember this value internally. */
    NotificationMessage msg;
    msg.type = NotificationMessageTypeForceDisplayBrightnessSetting;
    if(brightness < 0) brightness = 0;
    if(brightness > 100) brightness = 100;
    msg.data.forced_settings.display_brightness = ((float)brightness) / 100.f;
    const NotificationSequence seq_msg = {&msg, NULL};
    notification_message(FApp->Notificator, &seq_msg);

    if(brightness)
        notification_message(FApp->Notificator, &sequence_display_backlight_on);
    else
        notification_message(FApp->Notificator, &sequence_display_backlight_off);
}

void UpdateView() {
    view_port_update(FApp->view_port);
}

bool PinRead(GpioPin pin) {
    return furi_hal_gpio_read(&pin);
}
