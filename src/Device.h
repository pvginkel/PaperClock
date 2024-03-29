#pragma once

#ifndef LV_SIMULATOR

class Device {
public:
    void begin();

private:
    void set_fbcon_cursor(bool blank);
};

#endif
