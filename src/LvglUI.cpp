#include "includes.h"

#include "LvglUI.h"

LOG_TAG(LvglUI);

constexpr auto CIRCLES = 11;
constexpr auto CIRCLES_RADIUS = 10;
constexpr auto CIRCLE_RADIUS = 4;

atomic<uint32_t> LvglUI::_current_cookie(0);

void lv_obj_set_bounds(lv_obj_t* obj, int x, int y, int width, int height, lv_text_align_t align) {
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

bool LvglUICookie::is_valid() const { return LvglUI::_current_cookie.load() == _cookie; }

LvglUI::~LvglUI() { remove_loading_ui(); }

void LvglUI::begin() { do_begin(); }

void LvglUI::render() {
    _current_cookie++;

    auto parent = lv_screen_active();

    lv_obj_clean(parent);

    lv_theme_default_init(nullptr, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN),
                          LV_THEME_DEFAULT_DARK, NORMAL_FONT);

    lv_obj_set_style_bg_color(parent, lv_color_black(), LV_PART_MAIN);

    remove_loading_ui();

    do_render(parent);
}

void LvglUI::render_loading_ui(lv_obj_t* parent) {
    auto centerX = pw(50);
    auto centerY = ph(50);
    auto radius = pw(CIRCLES_RADIUS);

    for (auto i = 0; i < CIRCLES; i++) {
        auto obj = lv_obj_create(parent);
        _loading_circles.push_back(obj);

        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(obj, lv_theme_get_color_primary(parent), 0);
        lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(obj, 0, 0);
        lv_obj_set_size(obj, pw(CIRCLE_RADIUS), pw(CIRCLE_RADIUS));

        auto angleRadians = (360.0 / CIRCLES * i) * (M_PI / 180);

        auto x = centerX + radius * cos(angleRadians);
        auto y = centerY + radius * sin(angleRadians);

        lv_obj_set_x(obj, int32_t(x));
        lv_obj_set_y(obj, int32_t(y));
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_values(&a, 0, CIRCLES);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_user_data(&a, this);
    lv_anim_set_exec_cb(&a, loading_animation_callback);

    lv_anim_start(&a);
}

void LvglUI::loading_animation_callback(void* var, int32_t v) {
    const auto self = (LvglUI*)var;

    for (const auto obj : self->_loading_circles) {
        lv_obj_set_style_bg_opa(obj, (v / double(CIRCLES)) * 255, 0);
        v = (v - 1) % CIRCLES;
    }
}

void LvglUI::remove_loading_ui() {
    lv_anim_del(this, loading_animation_callback);

    _loading_circles.clear();
}

void LvglUI::reset_outer_container_styles(lv_obj_t* cont) {
    auto row_pad = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    auto column_pad = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_row(cont, lv_dpx(PADDING), LV_PART_MAIN);
    lv_obj_set_style_pad_column(cont, lv_dpx(PADDING), LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont, lv_dpx(PADDING), LV_PART_MAIN);
}

void LvglUI::style_icon_button(lv_obj_t* button) {
    lv_obj_set_style_text_font(button, &lv_font_symbols, LV_PART_MAIN);
    auto pad = lv_obj_get_style_pad_top(button, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(button, pad * 1.3, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(button, pad * 1.2, LV_PART_MAIN);
}
