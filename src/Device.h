#pragma once

#ifndef LV_SIMULATOR

#include "IT8951-ePaper/EPD_IT8951.h"

class Device {
    uint16_t _panel_width;
    uint16_t _panel_height;
    uint32_t _init_target_memory_addr;
    bool _four_byte_align;
    char* _lut_version;
    uint8_t* _device_buffer;
    uint32_t _draw_count;
    IT8951_Dev_Info _device_info;
    bool _on;

public:
    bool begin();
    void set_on(bool on);

private:
    void flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
};

#endif
