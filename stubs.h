#pragma once

/* Basic C types */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* date/time struct used by the application */
typedef struct {
    int hour, minute, second;
    int weekday;
    int day, month, year;
} DateTime;

/* some utility macros used by the original code */
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/* forward declarations for various Furi/GUI types */
typedef struct FuriMessageQueue FuriMessageQueue;
typedef struct FuriMutex FuriMutex;
typedef struct FuriTimer FuriTimer;
typedef struct Gui Gui;
typedef struct ViewPort ViewPort;
typedef struct Canvas Canvas;
typedef struct GpioPin { int dummy; } GpioPin;
typedef struct InfraredWorker InfraredWorker;

typedef struct NotificationApp {
    struct {
        float display_brightness;
        float led_brightness;
    } settings;
} NotificationApp;

typedef struct NotificationMessage NotificationMessage;

/* basic contents needed by the code - only the fields that are accessed */
struct NotificationMessage {
    int type;
    union {
        struct { int value; } led;
        /* other fields are ignored */
    } data;
};

/* sequence type is an array of pointers to messages */
typedef NotificationMessage* NotificationSequence[];

/* simple enum placeholders */
enum { InputTypeMAX, InputTypeLong, InputTypeRepeat, InputTypeShort };
typedef struct {
    int type;
    int key;
} InputEvent;

/* input key codes used by the application */
enum { InputKeyUp, InputKeyDown, InputKeyLeft, InputKeyRight, InputKeyBack, InputKeyOk };

/* status/result types */
typedef int FuriStatus;
#define FuriStatusOk 0
#define FuriWaitForever (-1)
#define FuriFlagWaitAny 0

/* timer/mutex types */
enum { FuriTimerTypePeriodic };
enum { FuriMutexTypeNormal };

/* record identifiers, layers, etc */
enum { RECORD_GUI, RECORD_NOTIFICATION, RECORD_STORAGE };
enum { GuiLayerFullscreen };

/* alignment constants */
enum { AlignRight, AlignTop, AlignLeft, AlignBottom, AlignCenter };

/* many external messages used in the code; declare as extern to satisfy references */
extern NotificationMessage message_note_c8;
extern NotificationMessage message_delay_50;
extern NotificationMessage message_sound_off;
extern NotificationMessage message_force_display_brightness_setting_1f;
extern NotificationMessage message_display_backlight_on;
extern NotificationMessage message_display_backlight_off;
extern NotificationMessage message_vibro_on;
extern NotificationMessage message_vibro_off;
extern NotificationMessage message_delay_100;
extern NotificationMessage message_delay_500;
extern NotificationMessage message_note_c5;
extern NotificationMessage message_note_c6;
extern NotificationMessage message_delay_1;
extern NotificationMessage message_do_not_reset;
extern NotificationMessage sequence_reset_rgb[];
extern NotificationMessage sequence_display_backlight_off[];
extern NotificationMessage sequence_display_backlight_on[];

/* notification message type constants used by the application */
#define NotificationMessageTypeLedRed   1
#define NotificationMessageTypeLedGreen 2
#define NotificationMessageTypeLedBlue  3

/* GUI / canvas helpers */
extern void canvas_set_custom_u8g2_font(Canvas* c, const void* font);
extern void canvas_set_font(Canvas* c, int font);
extern void canvas_draw_str(Canvas* c, int x, int y, const char* s);
extern void canvas_draw_str_aligned(Canvas* c, int x, int y, int a1, int a2, const char* s);
extern void canvas_draw_icon(Canvas* c, int x, int y, const void* icon);

/* notification API */
extern void notification_message(NotificationApp* app, NotificationSequence* seq);

extern void elements_button_left(Canvas* c, const char* s);
extern void elements_button_center(Canvas* c, const char* s);
extern void elements_button_right(Canvas* c, const char* s);
extern void elements_progress_bar(Canvas* c, int x, int y, int w, int h, float progress, int percent);
extern void canvas_draw_rframe(Canvas* c, int x, int y, int w, int h, int r);

/* HAL / Furi API stubs */
extern uint32_t furi_get_tick(void);
extern FuriMessageQueue* furi_message_queue_alloc(int count, size_t size);
extern FuriStatus furi_message_queue_get(FuriMessageQueue* q, void* msg, int timeout);
extern FuriStatus furi_message_queue_put(FuriMessageQueue* q, const void* msg, int timeout);
extern void furi_message_queue_free(FuriMessageQueue* q);

extern FuriMutex* furi_mutex_alloc(int type);
extern void furi_mutex_free(FuriMutex* m);
extern FuriStatus furi_mutex_acquire(FuriMutex* m, int timeout);
extern FuriStatus furi_mutex_release(FuriMutex* m);

extern FuriTimer* furi_timer_alloc(void (*cb)(void*), int type, void* ctx);
extern void furi_timer_start(FuriTimer* t, uint32_t freq);
extern void furi_timer_stop(FuriTimer* t);

extern void* view_port_alloc(void);
extern void view_port_draw_callback_set(void* vp, void (*cb)(Canvas*, void*), void* ctx);
extern void view_port_input_callback_set(void* vp, void (*cb)(InputEvent*, void*), void* ctx);
extern void view_port_update(ViewPort* vp);

extern Gui* furi_record_open(int rec);
extern void furi_record_close(int rec);

extern bool furi_hal_gpio_read(const GpioPin* pin);
extern void furi_hal_gpio_init(void* pin, int mode, int pull, int speed);
extern void furi_hal_gpio_write(void* pin, int value);
extern void furi_hal_rtc_get_datetime(void* dt);

extern void furi_thread_flags_wait(int flags, int mode, int timeout);

/* additional helpers/prototypes that were referenced implicitly */
extern void furi_assert(void* ptr);
extern uint32_t furi_kernel_get_tick_frequency(void);
extern void gui_add_view_port(Gui* gui, ViewPort* vp, int layer);
extern void view_port_free(ViewPort* vp);
extern void furi_timer_free(FuriTimer* t);

/* GPIO constants and pins used in application */
extern GpioPin gpio_ext_pc3;
extern GpioPin gpio_ext_pb2;
enum { GpioModeOutputPushPull };
enum { GpioPullNo };
enum { GpioSpeedVeryHigh };

/* font constant */
extern int FontSecondary;

/* infrared signal structure */
typedef struct { int dummy; } InfraredWorkerSignal;

/* file storage types */
typedef struct Storage { int dummy; } Storage;
typedef struct File { int dummy; } File;

/* file access modes */
#define FSAM_READ  0
#define FSAM_WRITE 1

/* file open modes */
#define FSOM_OPEN_EXISTING 0
#define FSOM_CREATE_ALWAYS 1

/* storage constants and API */
extern File* storage_file_alloc(Storage* s);
extern int storage_file_open(File* f, const char* path, int flags, int mode);
extern int storage_file_read(File* f, void* buffer, size_t count);
extern int storage_file_write(File* f, const void* buffer, size_t count);
extern int storage_file_close(File* f);
extern void storage_file_free(File* f);

#define FSE_OK 0

/* infrared worker stub */
extern InfraredWorker* infrared_worker_alloc(void);
extern void infrared_worker_rx_enable_signal_decoding(InfraredWorker* w, bool en);
extern void infrared_worker_rx_enable_blink_on_receiving(InfraredWorker* w, bool en);
/* callback passes InfraredWorkerSignal pointer */
extern void infrared_worker_rx_set_received_signal_callback(InfraredWorker* w, void (*cb)(void*, InfraredWorkerSignal*), void* ctx);
extern void infrared_worker_rx_start(InfraredWorker* w);
extern void infrared_worker_rx_stop(InfraredWorker* w);
extern void infrared_worker_free(InfraredWorker* w);

/* extra helpers */
extern void view_port_enabled_set(ViewPort* vp, bool enable);
extern void gui_remove_view_port(Gui* gui, ViewPort* vp);

/* dummy allocation functions to satisfy malloc/free macros if needed */
#include <stdlib.h>

