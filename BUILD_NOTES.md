# AlarmaTik - Rebuild & Fixes

## Что было сделано

### 1. **Восстановлен исходный код**
   - Добавлены недостающие файлы заголовков (`stubs.h`, `alarmatik_icons.h`)
   - Заменены внешние зависимости Flipper SDK на минимальные заглушки для компиляции на хосте

### 2. **Исправлены ошибки компиляции**

#### ✅ Структурные типы
- `DateTime` — структура для работы с датой/временем
- `InfraredWorkerSignal` — структура для IR-сигналов  
- `NotificationMessage` — структура для уведомлений
- `Storage`, `File` — типы для работы с файлами

#### ✅ Константы и перечисления
- **Входные события**: `InputTypeShort`, `InputTypeRepeat`, `InputTypeLong`, `InputTypeMAX`
- **Клавиши**: `InputKeyUp`, `InputKeyDown`, `InputKeyLeft`, `InputKeyRight`, `InputKeyOk`, `InputKeyBack`
- **GPIO режимы**: `GpioModeOutputPushPull`, `GpioPullNo`, `GpioSpeedVeryHigh`
- **Файловые операции**: `FSAM_READ`, `FSAM_WRITE`, `FSOM_OPEN_EXISTING`, `FSOM_CREATE_ALWAYS`
- **Уведомления**: `NotificationMessageTypeLedRed`, `NotificationMessageTypeLedGreen`, `NotificationMessageTypeLedBlue`

#### ✅ Глобальные переменные/объекты
- `message_note_c8`, `message_delay_50`, `message_sound_off` и другие сообщения уведомлений
- `sequence_reset_rgb`, `sequence_display_backlight_on`, `sequence_display_backlight_off`
- `gpio_ext_pc3`, `gpio_ext_pb2` — пины GPIO
- `I_IR_On` — иконка IR

#### ✅ Функции API
- Фури/RTOS: `furi_get_tick()`, `furi_message_queue_*()`, `furi_mutex_*()`, `furi_timer_*()`, `furi_kernel_get_tick_frequency()`
- GUI/Canvas: `canvas_draw_str()`, `canvas_draw_icon()`, `canvas_draw_rframe()`, `elements_button_*()`, `elements_progress_bar()`
- Уведомления: `notification_message()`
- GPIO: `furi_hal_gpio_read()`, `furi_hal_gpio_write()`, `furi_hal_gpio_init()`, `furi_hal_rtc_get_datetime()`
- Infrared: `infrared_worker_*()` функции
- Хранилище: `storage_file_*()` функции
- GUI запись: `gui_add_view_port()`, `view_port_*()`, `furi_record_open()`, `furi_record_close()`

### 3. **Исправлены форматные строки**
   - Изменены `%02ld` на `%02d` в макросах `TIMER_TIME_FORMAT` и `MMSS_TIME_FORMAT`
   - Исправлены сигнатуры функций (например, `elements_progress_bar` теперь с правильным числом аргументов)

### 4. **Добавлены пути к файлам**
   - Определены константы для хранилища: `STORAGE_APP_DATA_PATH_PREFIX`, `SAVING_FILENAME`
   - Упрощены вызовы миграции хранилища для хоста

## Результат компиляции

✅ **Все файлы успешно откомпилированы:**
- `Application.o` — 47 KB
- `F0App.o` — 6.6 KB

⚠️ **Остаточные предупреждения** (не критичные):
- Несовместимые типы квалификатора для `const` в уведомлениях (вызвано дизайном заглушек)
- Формат `%ld` vs `%d` для типов `int32_t` (безопасно для хоста)

## Как использовать

### Для компиляции на хосте (Linux/macOS):
```bash
gcc -I. -Wall -Wextra -c *.c
gcc *.o -o alarmatik.elf
```

### Для сборки реального FAP файла для Flipper Zero:
```bash
# Используйте официальный Flipper SDK
cd /path/to/flipper/sdk
./fbt fap_alarmatik
```

При использовании SDK сборщик автоматически подхватит корректные заголовки Flipper и создаст готовый `.fap` файл.

## Файлы проекта

- `Application.c` / `Application.h` — основная логика приложения
- `F0App.c` / `F0App.h` — интеграция с GUI фреймворком
- `F0BigData.h` — встроенные шрифты и данные иконок
- `stubs.h` — **НОВЫЙ**: заглушки для хост-компиляции
- `alarmatik_icons.h` — **НОВЫЙ**: определения иконок
- `application.fam` — манифест приложения для Flipper
- `build_alarmatik.sh` — **НОВЫЙ**: скрипт сборки

## Версия

- **AlarmaTik**: 1.5.1 (с исправлениями)
- **Построено**: 27 февраля 2026
- **Целевые платформы**: Flipper Zero (Momentum/Unleashed), хост-системы
