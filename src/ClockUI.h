#pragma once
#pragma once

#include "HomeAssistantApi.h"
#include "LvglUI.h"
#include "Queue.h"
#include "Tasks.h"

class ClockUI : public LvglUI {
    struct ForecastIcon {
        lv_obj_t* icon_label;
        lv_obj_t* hour_label;
        lv_obj_t* wind_speed_label;
    };

    struct Stat {
        lv_obj_t* label;
        lv_obj_t* sub_label;
    };

    Tasks* _tasks;
    Queue* _queue;
    lv_font_t* _font_xl;
    lv_font_t* _font_l;
    lv_font_t* _font_l_mdi;
    lv_font_t* _font_m;
    lv_font_t* _font_m_mdi;
    lv_font_t* _font_s;
    lv_font_t* _font_xs;
    lv_font_t* _font_xs_mdi;
    HomeAssistantApi* _api;
    time_t _last_update_time;
    int _api_cookie;
    lv_obj_t* _clock_label;
    ForecastIcon _forecast1;
    ForecastIcon _forecast2;
    ForecastIcon _forecast3;
    Stat _outside_temp;
    Stat _min_temp;
    Stat _max_temp;
    Stat _humidity;
    Stat _printer;

public:
    ClockUI(Tasks* tasks, Queue* queue, HomeAssistantApi* api)
        : _tasks(tasks), _queue(queue), _api(api), _api_cookie(0) {}

    void update();

protected:
    void do_begin() override;
    void do_render(lv_obj_t* parent) override;

private:
    ForecastIcon create_forecast_icon(lv_obj_t* cont, int row);
    Stat create_stat(lv_obj_t* cont, int col, int row, int row_span, bool large, bool sub_label, const char* unit,
                     const char* icon);
};
