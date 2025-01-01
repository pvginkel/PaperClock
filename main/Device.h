#pragma once

#ifndef LV_SIMULATOR
#include "it8951.h"
#endif

class Device {
public:
#ifndef LV_SIMULATOR
    bool begin();
    void process();
    bool set_on(bool on);
    void clear_screen() { _display.clear_screen(); }
#endif

    void standby_after_next_paint() { _standby_after_next_paint = true; }

private:
#ifndef LV_SIMULATOR
    void flush_cb(lv_display_t* display, const lv_area_t* area, uint8_t* px_map);

    bool _on{true};
    bool _flushing{false};
    uint32_t _flush_start{0};
    IT8951 _display{};
    lv_area_t _screen_buffer_dirty{};
#endif

    bool _standby_after_next_paint{false};
};
