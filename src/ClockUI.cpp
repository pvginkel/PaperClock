#include "includes.h"

#include "ClockUI.h"

#include <chrono>

#include "mdi-icons.h"

LOG_TAG(ClockUI);

static constexpr auto FONT_SIZE_XL = 440;
static constexpr auto FONT_SIZE_L = 220;
static constexpr auto FONT_SIZE_M = 140;
static constexpr auto FONT_SIZE_S = 110;
static constexpr auto FONT_SIZE_XS = 90;

void ClockUI::do_begin() {
    LvglUI::do_begin();

    _font_xl = lv_freetype_font_create(FONTS_PREFIX "Roboto-Medium.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                       FONT_SIZE_XL, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_l = lv_freetype_font_create(FONTS_PREFIX "Roboto-Medium.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      FONT_SIZE_L, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_m = lv_freetype_font_create(FONTS_PREFIX "Roboto-Medium.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      FONT_SIZE_M, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_s = lv_freetype_font_create(FONTS_PREFIX "Roboto-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      FONT_SIZE_S, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_xs = lv_freetype_font_create(FONTS_PREFIX "Roboto-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                       FONT_SIZE_XS, LV_FREETYPE_FONT_STYLE_NORMAL);

    _font_l_mdi =
        lv_freetype_font_create(FONTS_PREFIX "materialdesignicons-webfont.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                FONT_SIZE_L, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_m_mdi =
        lv_freetype_font_create(FONTS_PREFIX "materialdesignicons-webfont.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                FONT_SIZE_M, LV_FREETYPE_FONT_STYLE_NORMAL);
    _font_xs_mdi =
        lv_freetype_font_create(FONTS_PREFIX "materialdesignicons-webfont.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                FONT_SIZE_XS, LV_FREETYPE_FONT_STYLE_NORMAL);
}

void ClockUI::do_render(lv_obj_t* parent) {
    // Outer container contains the clock, forecast to the right and stats to the bottom.

    auto outer_cont = lv_obj_create(parent);
    reset_outer_container_styles(outer_cont);
    static int32_t outer_cont_col_desc[] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t outer_cont_row_desc[] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(outer_cont, outer_cont_col_desc, outer_cont_row_desc);

    // Right container contains the forecast.

    auto right_cont = lv_obj_create(outer_cont);
    reset_layout_container_styles(right_cont);
    lv_obj_set_style_pad_right(right_cont, lv_dpx(30), LV_PART_MAIN);
    lv_obj_set_style_pad_top(right_cont, lv_dpx(30), LV_PART_MAIN);
    static int32_t right_cont_col_desc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t right_cont_row_desc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(right_cont, right_cont_col_desc, right_cont_row_desc);
    lv_obj_set_grid_cell(right_cont, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // Bottom container contains the stats.

    auto bottom_cont = lv_obj_create(outer_cont);
    reset_layout_container_styles(bottom_cont);
    lv_obj_set_style_pad_right(bottom_cont, lv_dpx(30), LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(bottom_cont, lv_dpx(30), LV_PART_MAIN);
    static int32_t bottom_cont_col_desc[] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t bottom_cont_row_desc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(bottom_cont, bottom_cont_col_desc, bottom_cont_row_desc);
    lv_obj_set_grid_cell(bottom_cont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 1, 1);

    // Clock.

    _clock_label = lv_label_create(outer_cont);
    lv_obj_set_grid_cell(_clock_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_align(_clock_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(_clock_label, _font_xl, LV_PART_MAIN);

    // Add forecast icons.

    _forecast1 = create_forecast_icon(right_cont, 0);
    _forecast2 = create_forecast_icon(right_cont, 1);
    _forecast3 = create_forecast_icon(right_cont, 2);

    // Add stats.

    _outside_temp = create_stat(bottom_cont, 0, 0, 2, true, true, "\U000000B0C", MDI_THERMOMETER);
    _min_temp = create_stat(bottom_cont, 1, 0, 1, false, false, "\U000000B0C", MDI_THERMOMETER_LOW);
    _max_temp = create_stat(bottom_cont, 2, 0, 1, false, false, "\U000000B0C", MDI_THERMOMETER_HIGH);
    _humidity = create_stat(bottom_cont, 1, 1, 1, false, false, "%", MDI_WATER_PERCENT);
    _printer = create_stat(bottom_cont, 2, 1, 1, false, false, "%", MDI_PRINTER_3D);

    _last_update_time = 0;
    _api_cookie = 0;

    update();
}

ClockUI::ForecastIcon ClockUI::create_forecast_icon(lv_obj_t* cont, int row) {
    auto icon_cont = lv_obj_create(cont);
    reset_layout_container_styles(icon_cont);
    lv_obj_set_style_pad_all(icon_cont, lv_dpx(30), LV_PART_MAIN);
    static int32_t icon_cont_col_desc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t icon_cont_row_desc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(icon_cont, icon_cont_col_desc, icon_cont_row_desc);
    lv_obj_set_grid_cell(icon_cont, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, row, 1);

    ForecastIcon result;

    result.icon_label = lv_label_create(icon_cont);
    lv_obj_set_grid_cell(result.icon_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 2);
    lv_obj_set_style_text_align(result.icon_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(result.icon_label, _font_m_mdi, LV_PART_MAIN);

    result.hour_label = lv_label_create(icon_cont);
    lv_obj_set_grid_cell(result.hour_label, LV_GRID_ALIGN_START, 1, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(result.hour_label, _font_xs, LV_PART_MAIN);

    const auto wind_speed_icon = lv_label_create(icon_cont);
    lv_obj_set_grid_cell(wind_speed_icon, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_label_set_text(wind_speed_icon, MDI_WINDSOCK);
    lv_obj_set_style_text_font(wind_speed_icon, _font_xs_mdi, LV_PART_MAIN);
    lv_obj_set_style_pad_right(wind_speed_icon, lv_dpx(6), LV_PART_MAIN);

    result.wind_speed_label = lv_label_create(icon_cont);
    lv_obj_set_grid_cell(result.wind_speed_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_style_text_font(result.wind_speed_label, _font_xs, LV_PART_MAIN);

    return result;
}

ClockUI::Stat ClockUI::create_stat(lv_obj_t* cont, int col, int row, int row_span, bool large, bool sub_label,
                                   const char* unit, const char* icon) {
    const auto font = large ? _font_l : _font_m;
    const auto font_mdi = large ? _font_l_mdi : _font_m_mdi;

    const auto stat_cont = lv_obj_create(cont);
    reset_layout_container_styles(stat_cont);
    lv_obj_set_style_pad_hor(stat_cont, lv_dpx(20), LV_PART_MAIN);
    lv_obj_set_style_pad_ver(stat_cont, lv_dpx(6), LV_PART_MAIN);
    static int32_t stat_cont_col_desc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT,
                                           LV_GRID_TEMPLATE_LAST};
    static int32_t stat_cont_row_desc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(stat_cont, stat_cont_col_desc, stat_cont_row_desc);
    lv_obj_set_grid_cell(stat_cont, LV_GRID_ALIGN_START, col, 1, LV_GRID_ALIGN_CENTER, row, row_span);

    Stat result;

    const auto icon_label = lv_label_create(stat_cont);
    lv_obj_set_grid_cell(icon_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_text(icon_label, icon);
    lv_obj_set_style_text_font(icon_label, font_mdi, LV_PART_MAIN);

    result.label = lv_label_create(stat_cont);
    lv_obj_set_grid_cell(result.label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(result.label, font, LV_PART_MAIN);

    if (sub_label) {
        result.sub_label = lv_label_create(stat_cont);
        lv_obj_set_grid_cell(result.sub_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);
        lv_obj_set_style_text_font(result.sub_label, _font_s, LV_PART_MAIN);
        lv_obj_set_style_margin_bottom(result.sub_label, lv_dpx(23), LV_PART_MAIN);
    } else {
        result.sub_label = nullptr;
    }

    const auto unit_label = lv_label_create(stat_cont);
    lv_obj_set_grid_cell(unit_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_text(unit_label, unit);
    lv_obj_set_style_text_font(unit_label, font, LV_PART_MAIN);

    return result;
}

void ClockUI::update() {
    const auto now = chrono::system_clock::now();
    auto now_time = chrono::system_clock::to_time_t(now);

    auto update = _last_update_time == 0 || (now_time % 60 == 0 && now_time != _last_update_time) ||
                  (_api_cookie == 0 && _api->get_update_cookie() != 0);

#if false

    if (_api->get_update_cookie() != _api_cookie) {
        update = true;
    }

#endif

    if (!update) {
        return;
    }

    _last_update_time = now_time;

    auto now_tm = localtime(&now_time);
    lv_label_set_text(_clock_label, strformat("%02d:%02d", now_tm->tm_hour, now_tm->tm_min).c_str());

#ifndef LV_SIMULATOR

    const auto update_cookie = _api->get_update_cookie();
    if (update_cookie == _api_cookie) {
        return;
    }

    _api_cookie = update_cookie;

#endif

    lv_label_set_text(_forecast1.icon_label, classify_weather_image(_api->get_forecast_hour_1_image()));
    lv_label_set_text(_forecast1.hour_label, strformat("%du", _api->get_forecast_hour_1_hour()).c_str());
    lv_label_set_text(_forecast1.wind_speed_label, strformat("%d", _api->get_forecast_hour_1_wind_speed()).c_str());
    lv_label_set_text(_forecast2.icon_label, classify_weather_image(_api->get_forecast_hour_2_image()));
    lv_label_set_text(_forecast2.hour_label, strformat("%du", _api->get_forecast_hour_2_hour()).c_str());
    lv_label_set_text(_forecast2.wind_speed_label, strformat("%d", _api->get_forecast_hour_2_wind_speed()).c_str());
    lv_label_set_text(_forecast3.icon_label, classify_weather_image(_api->get_forecast_hour_3_image()));
    lv_label_set_text(_forecast3.hour_label, strformat("%du", _api->get_forecast_hour_3_hour()).c_str());
    lv_label_set_text(_forecast3.wind_speed_label, strformat("%d", _api->get_forecast_hour_3_wind_speed()).c_str());

    auto outside_temperature_half_rounded = round(_api->get_outside_temperature() * 2) / 2;

    auto outside_temperature_rounded = (int)outside_temperature_half_rounded;
    auto outside_temperature_fraction = (int)(outside_temperature_half_rounded * 10) % 10;

    lv_label_set_text(_outside_temp.label, strformat("%d", outside_temperature_rounded).c_str());
    lv_label_set_text(_outside_temp.sub_label, strformat(",%d", outside_temperature_fraction).c_str());

    lv_label_set_text(_min_temp.label, strformat("%.0f", _api->get_forecast_temperature_low()).c_str());
    lv_label_set_text(_max_temp.label, strformat("%.0f", _api->get_forecast_temperature_high()).c_str());

    lv_label_set_text(_humidity.label, strformat("%.0f", _api->get_woonkamer_humidity()).c_str());
    lv_label_set_text(_printer.label, strformat("%.0f", _api->get_printer_voortgang()).c_str());
}
