#pragma once

#ifndef LV_SIMULATOR

class Device {
    uint16_t _panel_width;
    uint16_t _panel_height;
    uint32_t _init_target_memory_addr;
    bool _four_byte_align;
    char* _lut_version;
    uint8_t* _device_buffer;

public:
    bool begin();

private:
    void flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
};

#endif
