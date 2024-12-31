#include "includes.h"

#include "ShutdownUI.h"

#include "mdi-icons.h"

LOG_TAG(ShutdownUI);

static constexpr auto FONT_SIZE_L = 800;

void ShutdownUI::do_begin() {
    LvglUI::do_begin();

    _font_l_fa = lv_freetype_font_create(FONTS_PREFIX "fa-light-300.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                         FONT_SIZE_L, LV_FREETYPE_FONT_STYLE_NORMAL);
}

void ShutdownUI::do_render(lv_obj_t* parent) {
    _device->clear_screen();
    _device->standby_after_next_paint();

    auto outer_cont = lv_obj_create(parent);
    reset_outer_container_styles(outer_cont);
    static int32_t outer_cont_col_desc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t outer_cont_row_desc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(outer_cont, outer_cont_col_desc, outer_cont_row_desc);

    auto label = lv_label_create(outer_cont);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, _font_l_fa, LV_PART_MAIN);
    lv_label_set_text(label, FA_FACE_SLEEPING);
}
