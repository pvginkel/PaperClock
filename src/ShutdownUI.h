#pragma once

#include "HomeAssistantApi.h"
#include "LvglUI.h"
#include "Queue.h"
#include "Tasks.h"

class ShutdownUI : public LvglUI {
    lv_font_t* _font_l_fa;

protected:
    void do_begin() override;
    void do_render(lv_obj_t* parent) override;
};
