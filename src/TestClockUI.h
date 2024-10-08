#pragma once

#include "LvglUI.h"

class TestClockUI : public LvglUI {
    lv_font_t* _font_xl;
    lv_obj_t* _clock_label;
    time_t _last_time;

public:
    TestClockUI() : _font_xl(nullptr), _clock_label(nullptr), _last_time(0) {}

protected:
    void do_begin() override;
    void do_render(lv_obj_t* parent) override;
    void do_update() override;
};
