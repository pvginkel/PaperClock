#pragma once

class HomeAssistantApi {
    string _address;
    string _client_id;
    int _update_cookie;

public:
    HomeAssistantApi(string address, string client_id)
        : _address(std::move(address)), _client_id(std::move(client_id)) {}

    int get_update_cookie() { return _update_cookie; }
    const char* get_forecast_hour_1_image() { return "zonnig"; }
    int get_forecast_hour_1_wind_speed() { return 2; }
    int get_forecast_hour_1_hour() { return 20; }
    const char* get_forecast_hour_2_image() { return "lichtbewolkt"; }
    int get_forecast_hour_2_wind_speed() { return 3; }
    int get_forecast_hour_2_hour() { return 21; }
    const char* get_forecast_hour_3_image() { return "helderenacht"; }
    int get_forecast_hour_3_hour() { return 22; }
    int get_forecast_hour_3_wind_speed() { return 2; }
    double get_outside_temperature() { return 18.7; }
    double get_forecast_temperature_low() { return 9.2; }
    double get_forecast_temperature_high() { return 19.3; }
    double get_woonkamer_humidity() { return 59; }
    double get_printer_voortgang() { return 0; }
};
