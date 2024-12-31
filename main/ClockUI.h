#pragma once

#include "HomeAssistantApi.h"
#include "LvglUI.h"
#include "Queue.h"
#include "Tasks.h"

class ClockUI : public LvglUI {
    struct ForecastDayIcon {
        lv_obj_t* icon_label;
        lv_obj_t* weekday_label;
        lv_obj_t* temperature_label;
    };

    struct ForecastHourIcon {
        lv_obj_t* icon_label;
        lv_obj_t* hour_label;
        lv_obj_t* wind_speed_label;
    };

    struct Stat {
        lv_obj_t* label;
        lv_obj_t* sub_label;
    };

    lv_font_t* _font_xl;
    lv_font_t* _font_l;
    lv_font_t* _font_l_mdi;
    lv_font_t* _font_m;
    lv_font_t* _font_m_mdi;
    lv_font_t* _font_s;
    lv_font_t* _font_xs;
    lv_font_t* _font_xs_mdi;
    Device* _device;
    HomeAssistantApi* _api;
    time_t _last_update_time;
    int _api_cookie;
    lv_obj_t* _clock_label;
    ForecastDayIcon _forecast_days[5];
    ForecastHourIcon _forecast_hours[3];
    Stat _outside_temp;
    Stat _humidity;
    Stat _printer;

public:
    ClockUI(Device* device, HomeAssistantApi* api) : _device(device), _api(api), _api_cookie(0) {}

protected:
    void do_begin() override;
    void do_render(lv_obj_t* parent) override;
    void do_update() override;

private:
    ForecastDayIcon create_forecast_day_icon(lv_obj_t* cont, int index);
    ForecastHourIcon create_forecast_hour_icon(lv_obj_t* cont, int index);
    Stat create_stat(lv_obj_t* cont, int col, int row, int row_span, bool large, bool sub_label, const char* unit,
                     const char* icon);
};
