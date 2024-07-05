#pragma once

#include "Device.h"
#ifndef LV_SIMULATOR
#include "MQTTClient.h"
#endif

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

class HomeAssistantApi {
    string _address;
    string _client_id;
    string _user_name;
    string _password;
    int _update_cookie;
    Queue* _queue;
#ifndef LV_SIMULATOR
    MQTTClient _client;
#endif
    ForecastHour _forecast_hours[4];
    ForecastDay _forecast_days[5];
    double _outside_temperature;
    double _woonkamer_humidity;
    double _printer_vooruitgang;
    Callback<bool> _screen_on_changed;

public:
    HomeAssistantApi(Queue* queue, string address, string user_name, string password)
        : _address(std::move(address)),
          _client_id(strformat("session%d", rand())),
          _user_name(std::move(user_name)),
          _password(std::move(password)),
          _queue(queue)

    {}

    void begin();
    void end();
    void on_screen_on_changed(function<void(bool)> func) { _screen_on_changed.add(func); }

    int get_update_cookie() { return _update_cookie; }
    const ForecastHour& get_forecast_hour(int index) const { return _forecast_hours[index]; }
    const ForecastDay& get_forecast_day(int index) const { return _forecast_days[index]; }
    double get_outside_temperature() { return _outside_temperature; }
    double get_woonkamer_humidity() { return _woonkamer_humidity; }
    double get_printer_voortgang() { return _printer_vooruitgang; }

private:
#ifndef LV_SIMULATOR
    void connect();
    void disconnect();
    void connection_lost(char* cause);
    void message_arrived(char* topic_name, int topic_len, MQTTClient_message* message);
    void delivered(MQTTClient_deliveryToken dt);
#endif
    void parse_hour_forecast(const char* json, ForecastHour& forecast);
    void parse_day_forecast(const char* json, ForecastDay& forecast);
    string get_weekday_code(int weekday);
};

/*

*/
