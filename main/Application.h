#pragma once

#include "ApplicationBase.h"
#include "ClockUI.h"
#include "LoadingUI.h"
#include "ShutdownUI.h"

struct ForecastHour {
    int hour;
    string image;
    double temperature;
    int wind_speed;
};

struct ForecastDay {
    int weekday;
    string weekday_code;
    string image;
    double min_temperature;
    double max_temperature;
    double percent_rain;
    double percent_sun;
};

class Application : public ApplicationBase {
    Device* _device;
    LoadingUI* _loading_ui{};
    ClockUI* _clock_ui{};
    ShutdownUI* _shutdown_ui{};
    LvglUI* _current_ui{};
    int _update_cookie;
    ForecastHour _forecast_hours[4]{};
    ForecastDay _forecast_days[5]{};
    double _outside_temperature{};
    double _woonkamer_humidity{};
    double _printer_voortgang{};

public:
    Application(Device* device);

    int get_update_cookie() { return _update_cookie; }
    const ForecastHour& get_forecast_hour(int index) const { return _forecast_hours[index]; }
    const ForecastDay& get_forecast_day(int index) const { return _forecast_days[index]; }
    double get_outside_temperature() { return _outside_temperature; }
    double get_woonkamer_humidity() { return _woonkamer_humidity; }
    double get_printer_voortgang() { return _printer_voortgang; }

protected:
    void do_begin() override;
    void do_ready() override;
    void do_network_connection_failed() override;
    void do_process() override;

private:
    void run();
    void state_changed();
    void register_mqtt_callbacks();
    void screen_on_changed(bool is_on);
    void parse_hour_forecast(const char* json, ForecastHour& forecast);
    void parse_day_forecast(const char* json, ForecastDay& forecast);
    string get_weekday_code(int weekday);
};
