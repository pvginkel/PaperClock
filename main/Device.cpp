#include "includes.h"

#include "Device.h"

#include "lvgl.h"

#define LVGL_TICK_PERIOD_MS 2
#define LV_DRAW_BUFFER_PREFIX (LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_NATIVE) * 4)

LOG_TAG(Device);

void Device::process() {
    // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
    lv_timer_handler();
}

bool Device::set_on(bool on) {
    if (on == _on) {
        return false;
    }

    _on = on;

    if (on) {
        ESP_LOGI(TAG, "Turning screen on");

        _display.set_system_run();

        _display.clear_screen();
    } else {
        ESP_LOGI(TAG, "Turning screen off");

        _display.set_sleep();
    }

    return true;
}

void Device::flush_cb(lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
    ESP_ERROR_ASSERT(LV_COLOR_DEPTH == 1);

    ESP_LOGD(TAG, "Updating display %" PRIi32 "x%" PRIi32 " %" PRIi32 "x%" PRIi32, area->x1, area->y1, area->x2 + 1,
             area->y2 + 1);

    const uint32_t display_width = lv_display_get_horizontal_resolution(display);

    // The screen buffer dirty field tracks the outer bounds of the screen buffer
    // that got written to in this flush_cb run.

    if (!_flushing) {
        _flushing = true;

        lv_area_copy(&_screen_buffer_dirty, area);
    } else {
        _screen_buffer_dirty.x1 = min(_screen_buffer_dirty.x1, area->x1);
        _screen_buffer_dirty.y1 = min(_screen_buffer_dirty.y1, area->y1);
        _screen_buffer_dirty.x2 = max(_screen_buffer_dirty.x2, area->x2);
        _screen_buffer_dirty.y2 = max(_screen_buffer_dirty.y2, area->y2);
    }

    auto is_last = lv_display_flush_is_last(display);

    if (is_last) {
        auto turned_on = set_on(true);

        // There's a bug somewhere that requires us to send whole scan lines.

        _screen_buffer_dirty.x1 = 0;
        _screen_buffer_dirty.x2 = display_width - 1;

        if (turned_on) {
            _screen_buffer_dirty.y1 = 0;
            _screen_buffer_dirty.y2 = lv_display_get_vertical_resolution(display) - 1;
        }

        const auto dirty_width = lv_area_get_width(&_screen_buffer_dirty);
        const auto dirty_height = lv_area_get_height(&_screen_buffer_dirty);

        // Copy the dirty area of the screen buffer to the controller.

        IT8951Area area = {
            .x = (uint16_t)_screen_buffer_dirty.x1,
            .y = (uint16_t)_screen_buffer_dirty.y1,
            .w = (uint16_t)dirty_width,
            .h = (uint16_t)dirty_height,
        };

        _display.load_image_start(area, _display.get_memory_address(), IT8951_ROTATE_0, IT8951_PIXEL_FORMAT_1BPP);

        const auto buffer_len = _display.get_buffer_len();

        auto buffer = _display.get_buffer();
        size_t buffer_offset = 0;

        for (uint16_t y = 0; y < dirty_height; y++) {
            auto line = &px_map[LV_DRAW_BUFFER_PREFIX +
                                ((_screen_buffer_dirty.y1 + y) * display_width + _screen_buffer_dirty.x1) / 8];

            for (uint16_t x = 0; x < dirty_width / 8; x++) {
                buffer[buffer_offset++] = reverse_bits(*(line++));

                if (buffer_offset >= buffer_len) {
                    _display.load_image_flush_buffer(buffer_offset);
                    buffer = _display.get_buffer();
                    buffer_offset = 0;
                }
            }
        }

        if (buffer_offset > 0) {
            _display.load_image_flush_buffer(buffer_offset);
        }

        _display.load_image_end();

        _display.display_area(area, _display.get_memory_address(), IT8951_PIXEL_FORMAT_1BPP, IT8951_DISPLAY_MODE_A2);

        _flushing = false;

        auto flush_end = esp_get_millis();

        ESP_LOGI(TAG, "Updated %" PRIi32 "x%" PRIi32 " %" PRIi32 "x%" PRIi32 " in %" PRIu32 " ms",
                 _screen_buffer_dirty.x1, _screen_buffer_dirty.y1, _screen_buffer_dirty.x2 + 1,
                 _screen_buffer_dirty.y2 + 1, flush_end - _flush_start);
    }

    lv_display_flush_ready(display);

    if (is_last && _standby_after_next_paint) {
        _standby_after_next_paint = false;

        set_on(false);
    }
}

bool Device::begin() {
    _display.setup(-1.15f);

    _display.clear_screen();

    lv_init();

    ESP_LOGI(TAG, "Install LVGL tick timer");

    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = [](void* arg) { lv_tick_inc(LVGL_TICK_PERIOD_MS); },
        .name = "lvgl_tick",
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, ESP_TIMER_MS(LVGL_TICK_PERIOD_MS)));

    const size_t draw_buffer_size = LV_DRAW_BUFFER_PREFIX + _display.get_width() / 8 * _display.get_height();

    ESP_LOGI(TAG, "Allocating %" PRIu32 " bytes for draw buffer", (uint32_t)draw_buffer_size);

    auto draw_buffer = malloc(draw_buffer_size);
    if (!draw_buffer) {
        ESP_LOGE(TAG, "Failed to allocate draw buffer");
        esp_restart();
    }

    lv_display_t* display = lv_display_create(_display.get_width(), _display.get_height());
    lv_display_set_user_data(display, this);
    lv_display_set_buffers(display, draw_buffer, nullptr, draw_buffer_size, LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_display_set_flush_cb(display, [](lv_display_t* display, const lv_area_t* area, uint8_t* px_map) {
        ((Device*)lv_display_get_user_data(display))->flush_cb(display, area, px_map);
    });

    lv_display_set_dpi(display, LV_DPI_DEF);

    lv_display_add_event_cb(
        display,
        [](lv_event_t* e) {
            const auto self = (Device*)lv_event_get_user_data(e);

            self->_flush_start = esp_get_millis();
        },
        LV_EVENT_REFR_START, this);

    ESP_LOGI(TAG, "Device initialization complete");

    return true;
}
