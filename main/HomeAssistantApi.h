#pragma once

#include "Device.h"

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

struct HomeAssistantConnectionState {
    bool connected;
};

class HomeAssistantApi {
    string _address;
    string _client_id;
    int _update_cookie;
    Queue* _queue;
#ifndef LV_SIMULATOR
    esp_mqtt_client_handle_t _client;
#endif
    ForecastHour _forecast_hours[4]{};
    ForecastDay _forecast_days[5]{};
    double _outside_temperature{};
    double _woonkamer_humidity{};
    double _printer_vooruitgang{};
    Callback<bool> _screen_on_changed;
    Callback<HomeAssistantConnectionState> _state_changed;

public:
    HomeAssistantApi(Queue* queue) : _queue(queue) {}

    void begin();
    void end();
    void on_screen_on_changed(function<void(bool)> func) { _screen_on_changed.add(func); }
    void on_state_changed(function<void(HomeAssistantConnectionState)> func) { _state_changed.add(func); }

    int get_update_cookie() { return _update_cookie; }
    const ForecastHour& get_forecast_hour(int index) const { return _forecast_hours[index]; }
    const ForecastDay& get_forecast_day(int index) const { return _forecast_days[index]; }
    double get_outside_temperature() { return _outside_temperature; }
    double get_woonkamer_humidity() { return _woonkamer_humidity; }
    double get_printer_voortgang() { return _printer_vooruitgang; }

private:
#ifndef LV_SIMULATOR
    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void* eventData);
    void handleConnected();
    void handleData(esp_mqtt_event_handle_t event);
    void subscribe(const char* topic);
    void setOnline();
#endif
    void parse_hour_forecast(const char* json, ForecastHour& forecast);
    void parse_day_forecast(const char* json, ForecastDay& forecast);
    string get_weekday_code(int weekday);
};
