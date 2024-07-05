#include "includes.h"

#include "TestClockUI.h"

#include <chrono>

LOG_TAG(TestClockUI);

void TestClockUI::do_begin() {
    LvglUI::do_begin();

    _font_xl = lv_freetype_font_create(FONTS_PREFIX "Roboto-Medium.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 440,
                                       LV_FREETYPE_FONT_STYLE_NORMAL);
}

void TestClockUI::do_render(lv_obj_t* parent) {
    // Outer container contains the clock, forecast to the right and stats to the bottom.

    auto outer_cont = lv_obj_create(parent);
    reset_outer_container_styles(outer_cont);
    static int32_t outer_cont_col_desc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t outer_cont_row_desc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(outer_cont, outer_cont_col_desc, outer_cont_row_desc);

    // Clock.

    _clock_label = lv_label_create(outer_cont);
    lv_obj_set_grid_cell(_clock_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_align(_clock_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(_clock_label, _font_xl, LV_PART_MAIN);

    update();
}

void TestClockUI::do_update() {
    const auto now = chrono::system_clock::now();
    auto now_time_t = chrono::system_clock::to_time_t(now);

    if (_last_time == now_time_t) {
        return;
    }

    _last_time = now_time_t;

    auto now_tm = localtime(&now_time_t);
    lv_label_set_text(_clock_label,
                      strformat("%02d:%02d:%02d", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec).c_str());
}
