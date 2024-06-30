#pragma once

#include "Device.h"
#include "MQTTClient.h"

class HomeAssistantApi {
    Device* _device;
    string _address;
    string _client_id;
    string _user_name;
    string _password;
    int _update_cookie;
    MQTTClient _client;
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

public:
    HomeAssistantApi(Device* device, string address, string user_name, string password)
        : _device(device),
          _address(std::move(address)),
          _client_id(strformat("session%d", rand())),
          _user_name(std::move(user_name)),
          _password(std::move(password))

    {}

    bool begin();
    void end();

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
    bool subscribe(const string& topic, int qos);
    void connection_lost(char* cause);
    int message_arrived(char* topic_name, int topic_len, MQTTClient_message* message);
    void delivered(MQTTClient_deliveryToken dt);
};
