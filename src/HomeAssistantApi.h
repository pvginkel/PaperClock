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
    string _forecast_hour_1_image;
    int _forecast_hour_1_wind_speed;
    int _forecast_hour_1_hour;
    string _forecast_hour_2_image;
    int _forecast_hour_2_wind_speed;
    int _forecast_hour_2_hour;
    string _forecast_hour_3_image;
    int _forecast_hour_3_wind_speed;
    int _forecast_hour_3_hour;
    double _outside_temperature;
    double _forecast_temperature_low;
    double _forecast_temperature_high;
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
    const string& get_forecast_hour_1_image() { return _forecast_hour_1_image; }
    int get_forecast_hour_1_wind_speed() { return _forecast_hour_1_wind_speed; }
    int get_forecast_hour_1_hour() { return _forecast_hour_1_hour; }
    const string& get_forecast_hour_2_image() { return _forecast_hour_2_image; }
    int get_forecast_hour_2_wind_speed() { return _forecast_hour_2_wind_speed; }
    int get_forecast_hour_2_hour() { return _forecast_hour_2_hour; }
    const string& get_forecast_hour_3_image() { return _forecast_hour_3_image; }
    int get_forecast_hour_3_wind_speed() { return _forecast_hour_3_wind_speed; }
    int get_forecast_hour_3_hour() { return _forecast_hour_3_hour; }
    double get_outside_temperature() { return _outside_temperature; }
    double get_forecast_temperature_low() { return _forecast_temperature_low; }
    double get_forecast_temperature_high() { return _forecast_temperature_high; }
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
};

/*
string forecast_hour_1 = R"({'entity_id': 'sensor.weerlive_forecast_hour_1', 'state': '17', 'attributes': {'image':
'regen', 'temp': 16, 'windbft': 3, 'friendly_name': 'WeerLive Forecast Hour 1'}, 'last_changed':
'2024-07-05T15:34:53.398432+00:00', 'last_reported': '2024-07-05T15:34:53.398432+00:00', 'last_updated':
'2024-07-05T15:34:53.398432+00:00', 'context': {'id': '01J21RHDPPEEYXWAJY1KT4670R', 'parent_id': None, 'user_id':
None}})";

string forecast_day_1 = R"({'entity_id': 'sensor.weerlive_forecast_day_1', 'state': '5', 'attributes': {'image':
'halfbewolkt', 'min_temp': 16, 'max_temp': 17, 'neersl_perc_dag': 20, 'zond_perc_dag': 22, 'friendly_name': 'WeerLive
Forecast Day 1'}, 'last_changed': '2024-07-05T15:34:53.400550+00:00', 'last_reported':
'2024-07-05T15:34:53.400550+00:00', 'last_updated': '2024-07-05T15:34:53.400550+00:00', 'context': {'id':
'01J21RHDPRZGB0E6Y8V9HVK1DD', 'parent_id': None, 'user_id': None}})";
*/
