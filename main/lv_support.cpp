#include "includes.h"

#include "lv_support.h"

#if LV_USE_TEXTAREA

void lv_label_get_text_size(lv_point_t* size_res, const lv_obj_t* obj, int32_t letter_space, int32_t line_space,
                            int32_t max_width, lv_text_flag_t flag) {
    const auto text = lv_label_get_text(obj);
    const auto font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);

    lv_txt_get_size(size_res, text, font, letter_space, line_space, max_width, flag);
}

#endif

void lv_obj_set_grid_cell(lv_obj_t* obj, lv_grid_align_t x_align, uint8_t col_pos, lv_grid_align_t y_align,
                          uint8_t row_pos) {
    lv_obj_set_grid_cell(obj, x_align, col_pos, 1, y_align, row_pos, 1);
}

void lv_obj_set_bounds(lv_obj_t* obj, int32_t x, int32_t y, int32_t width, int32_t height, lv_text_align_t align) {
    lv_obj_set_size(obj, width, height);

    switch (align) {
        case LV_TEXT_ALIGN_LEFT:
            lv_obj_set_x(obj, x);
            break;

        case LV_TEXT_ALIGN_CENTER:
            lv_obj_set_x(obj, x - width / 2);
            break;

        case LV_TEXT_ALIGN_RIGHT:
            lv_obj_set_x(obj, x - width);
            break;
    }

    lv_obj_set_y(obj, y - height / 2);
}

void lv_obj_set_visibility(lv_obj_t* obj, bool visible) {
    const auto is_visible = !lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN);
    if (visible != is_visible) {
        if (visible) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}
