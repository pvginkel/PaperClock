#include "includes.h"

#ifndef LV_SIMULATOR

#include "Device.h"
#include "IT8951-ePaper/DEV_Config.h"
#include "lvgl.h"

static auto constexpr VCOM = -1.58;
static auto constexpr MODE = 0;
static auto constexpr INIT_DRAW_INTERVAL = 30;

LOG_TAG(Device);

void Device::flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
    if (!_on) {
        lv_display_flush_ready(disp);
        return;
    }

    // The color format is RGB565. We howevery only support 4 bit grayscale.
    // We can just take the four most significant bytes of (4 of the 5 bits of red).
    // This assumes that we're drawing black & white anyway.

    auto buf = (uint16_t*)px_map;
    auto target = _device_buffer;

    auto left = area->x1;
    auto top = area->y1;
    auto width = area->x2 - area->x1 + 1;
    auto height = area->y2 - area->y1 + 1;

    // Ensure width is a multiple of 8.
    width = (width + 7) & ~7;

    // Every INIT_DRAW_INTERVAL draws we clear the screen and update the whole screen.

    if (_draw_count++ % INIT_DRAW_INTERVAL == 0) {
        LOGI(TAG, "Performing full refresh");

        left = 0;
        top = 0;
        width = _panel_width;
        height = _panel_height;

        // Tried to include a draw screen in the refresh. An image is sent with
        // the command, so it looks like it's possible, but it doesn't work.

        EPD_IT8951_Clear_Refresh(_device_info, _init_target_memory_addr, INIT_Mode, true);
    }

    // There's still a bug somewhere that requires updates to be done in whole
    // lines.

    left = 0;
    width = _panel_width;

    uint8_t byte = 0;

    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            auto pixel = buf[(top + y) * _panel_width + (x + left)];

            byte |= (pixel >> 15) << (x % 8);

            if (x % 8 == 7) {
                *(target++) = byte;
                byte = 0;
            }
        }
    }

    EPD_IT8951_1bp_Refresh(_device_buffer, left, top, width, height, A2_Mode, _init_target_memory_addr, true);

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

    _device_info = EPD_IT8951_Init(vcom);

    if (!_device_info.LUT_Version || !*_device_info.LUT_Version) {
        LOGE(TAG, "Cannot connect to device!\r\n");
        return false;
    }

#if (Enhance)
    Debug("Attention! Enhanced driving ability, only used when the screen is blurred\r\n");
    Enhance_Driving_Capability();
#endif

    // get some important info from Dev_Info structure
    _panel_width = _device_info.Panel_W;
    _panel_height = _device_info.Panel_H;
    _init_target_memory_addr = _device_info.Memory_Addr_L | (_device_info.Memory_Addr_H << 16);
    _four_byte_align = false;
    _lut_version = (char*)_device_info.LUT_Version;

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

    if (_four_byte_align) {
        LOGE(TAG, "Alignment is not supported");
        return false;
    }

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

void Device::clear_screen() { EPD_IT8951_Clear_Refresh(_device_info, _init_target_memory_addr, INIT_Mode, true); }

void Device::set_on(bool on) {
    if (on == _on) {
        return;
    }

    _on = on;

    if (on) {
        LOGI(TAG, "Turning screen on");

        // Force a full refresh.
        _draw_count = 0;

        EPD_IT8951_SystemRun();

        // Force a full redraw of the screen.

        lv_obj_invalidate(lv_screen_active());
    } else {
        LOGI(TAG, "Turning screen off");

        EPD_IT8951_Sleep();
    }
}

#endif
