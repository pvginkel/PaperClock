#include "includes.h"

#ifndef LV_SIMULATOR

#include "Device.h"

LOG_TAG(Device);

void Device::begin() {
    auto fb_dev = (const char*)getenv("FB_DEV");
    auto keyboard_dev = getenv("KEYBOARD_DEV");
    auto mouse_dev = getenv("MOUSE_DEV");

    if (!fb_dev) {
        fb_dev = "/dev/fb0";
        LOGW(TAG, "No frame buffer device specified, defaulting to %s", fb_dev);
    }
    if (!keyboard_dev) {
        LOGW(TAG, "No keyboard device specified; keyboard is disabled");
    }
    if (!mouse_dev) {
        LOGW(TAG, "No mouse device specified; mouse is disabled");
    }

    set_fbcon_cursor(false);

    lv_init();

    /*Linux frame buffer device init*/
    auto disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, fb_dev);

    if (keyboard_dev) {
        lv_evdev_create(LV_INDEV_TYPE_KEYPAD, keyboard_dev);
    }
    if (mouse_dev) {
        auto mouse_indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, mouse_dev);

#if !NDEBUG
        LV_IMG_DECLARE(mouse_cursor_icon);
        auto cursor_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(mouse_indev, cursor_obj);
#endif
    }

    lv_display_set_dpi(lv_display_get_default(), LV_DPI_DEF);
}

void Device::set_fbcon_cursor(bool blank) {
    auto fd = open("/dev/tty1", O_RDWR);
    if (0 < fd) {
        write(fd, "\033[?25", 5);
        write(fd, blank ? "h" : "l", 1);
    }
    close(fd);
}

#endif
