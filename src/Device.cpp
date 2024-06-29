#include "includes.h"

#ifndef LV_SIMULATOR

#include "Device.h"
#include "IT8951-ePaper/DEV_Config.h"
#include "IT8951-ePaper/EPD_IT8951.h"
#include "lvgl.h"

static auto constexpr VCOM = -1.58;
static auto constexpr MODE = 0;

LOG_TAG(Device);

void Device::flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    // The color format is RGB565. We howevery only support 4 bit grayscale.
    // We can just take the four most significant bytes of (4 of the 5 bits of red).
    // This assumes that we're drawing black & white anyway.

    auto buf = (uint16_t*)px_map;
    auto offset = 0;
    uint8_t byte = 0;

    for (auto y = area->y1; y <= area->y2; y++) {
        for (auto x = area->x1; x <= area->x2; x++) {
            auto pixel = buf[y * _panel_width + x];

            if (offset % 2 == 0) {
                byte = pixel >> 12;
            } else {
                byte |= (pixel >> 12) << 4;

                _device_buffer[offset / 2] = byte;
            }

            offset++;
        }
    }

    if (_four_byte_align) {
        LOGE(TAG, "Alignment is not supported");
        exit(2);
        return;
    }

    LOGI(TAG, "Drawing %d pixels area %d %d %d %d", offset, area->x1, area->y1, area->x2 + 1, area->y2 + 1);

    EPD_IT8951_4bp_Refresh(_device_buffer, area->x1, area->y1, area->x2 + 1 - area->x1, area->y2 + 1 - area->y1, true,
                           _init_target_memory_addr, true);

    lv_display_flush_ready(disp);
}

bool Device::begin() {
    // Init the BCM2835 Device
    if (DEV_Module_Init() != 0) {
        LOGE(TAG, "DEV_Module_Init failed");
        return false;
    }

    auto vcom = (UWORD)(fabs(VCOM) * 1000);
    auto epd_mode = MODE;

    auto dev_info = EPD_IT8951_Init(vcom);

    if (!dev_info.LUT_Version || !*dev_info.LUT_Version) {
        LOGE(TAG, "Cannot connect to device!\r\n");
        return false;
    }

#if (Enhance)
    Debug("Attention! Enhanced driving ability, only used when the screen is blurred\r\n");
    Enhance_Driving_Capability();
#endif

    // get some important info from Dev_Info structure
    _panel_width = dev_info.Panel_W;
    _panel_height = dev_info.Panel_H;
    _init_target_memory_addr = dev_info.Memory_Addr_L | (dev_info.Memory_Addr_H << 16);
    _four_byte_align = false;
    _lut_version = (char*)dev_info.LUT_Version;

    if (strcmp(_lut_version, "M641") == 0) {
        // 6inch e-Paper HAT(800,600), 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        A2_Mode = 4;
        _four_byte_align = true;
    } else if (strcmp(_lut_version, "M841_TFAB512") == 0) {
        // Another firmware version for 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        A2_Mode = 6;
        _four_byte_align = true;
    } else if (strcmp(_lut_version, "M841") == 0) {
        // 9.7inch e-Paper HAT(1200,825)
        A2_Mode = 6;
    } else if (strcmp(_lut_version, "M841_TFA2812") == 0) {
        // 7.8inch e-Paper HAT(1872,1404)
        A2_Mode = 6;
    } else if (strcmp(_lut_version, "M841_TFA5210") == 0) {
        // 10.3inch e-Paper HAT(1872,1404)
        A2_Mode = 6;
    } else {
        // default set to 6 as A2 Mode
        A2_Mode = 6;
    }

    EPD_IT8951_Clear_Refresh(dev_info, _init_target_memory_addr, INIT_Mode, true);

    lv_init();

    auto display = lv_display_create(_panel_width, _panel_height);

    lv_display_set_resolution(display, _panel_width, _panel_height);
    lv_display_set_dpi(lv_display_get_default(), LV_DPI_DEF);
    lv_display_set_user_data(display, this);

    lv_display_set_flush_cb(display, [](lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
        auto self = (Device*)lv_display_get_user_data(disp);
        self->flush_cb(disp, area, px_map);
    });

    auto draw_buffer_size = _panel_width * _panel_height;
    auto draw_buffer = malloc(draw_buffer_size * sizeof(uint16_t));

    _device_buffer = (uint8_t*)malloc(draw_buffer_size / 2);  // 4 bits grayscale, so two pixes per byte.

    lv_display_set_buffers(display, draw_buffer, nullptr, draw_buffer_size * sizeof(uint16_t),
                           LV_DISPLAY_RENDER_MODE_DIRECT);

    return true;
}

#endif
