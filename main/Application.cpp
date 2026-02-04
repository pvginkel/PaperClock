#include "includes.h"

#include "Application.h"

#include <chrono>

#include "Messages.h"
#include "driver/i2c.h"

LOG_TAG(Application);

Application::Application(Device* device) : _device(device) {}

void Application::do_begin() {
    ESP_LOGI(TAG, "Starting UI worker task");

    ESP_ERROR_ASSERT(xTaskCreatePinnedToCore([](void* param) { ((Application*)param)->run(); }, "Application::run_task",
                                             8192, this, 1, nullptr, 1));

    get_mqtt_connection().on_connected_changed([this](auto state) {
        if (state.connected) {
            state_changed();

            register_mqtt_callbacks();
        }
    });
}

void Application::run() {
    ESP_LOGI(TAG, "Setting up loading UI");

    _loading_ui = new LoadingUI(is_silent_startup());

    _loading_ui->begin();
    _loading_ui->set_title(MSG_STARTING);
    _loading_ui->set_state(LoadingUIState::Loading);
    _loading_ui->render();

    auto last_tick_call = chrono::high_resolution_clock::now();

    while (true) {
        auto start = chrono::high_resolution_clock::now();

        process();

        lv_timer_handler();

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        auto sleep = 10 - duration;
        if (sleep > 0) {
            vTaskDelay(pdMS_TO_TICKS(sleep));
        }

        auto after_sleep = chrono::high_resolution_clock::now();
        auto last_tick_duration = chrono::duration_cast<chrono::milliseconds>(after_sleep - last_tick_call).count();

        lv_tick_inc(last_tick_duration);

        last_tick_call = after_sleep;
    }
}

void Application::do_network_connection_failed() {
    if (_loading_ui) {
        _loading_ui->set_error(MSG_FAILED_TO_CONNECT);
        _loading_ui->set_state(LoadingUIState::Error);
        _loading_ui->render();
    }
}

void Application::do_ready() {
    delete _loading_ui;
    _loading_ui = nullptr;

    _clock_ui = new ClockUI(_device, this);
    _clock_ui->begin();

    _shutdown_ui = new ShutdownUI(_device);
    _shutdown_ui->begin();

    _current_ui = _clock_ui;
    _current_ui->render();
}

void Application::do_process() {
    _device->process();

    if (_current_ui) {
        _current_ui->update();
    }
}

void Application::state_changed() {
    if (!get_mqtt_connection().is_connected()) {
        return;
    }

    get_mqtt_connection().send_state();
}

void Application::parse_hour_forecast(const char* json, ForecastHour& forecast) {
    auto root = cJSON_Parse(json);
    if (root == nullptr) {
        return;
    }

    forecast.hour = 0;
    forecast.image = "";
    forecast.temperature = 0;
    forecast.wind_speed = 0;

    auto state = cJSON_GetObjectItemCaseSensitive(root, "state");
    if (cJSON_IsString(state) && (state->valuestring != nullptr)) {
        forecast.hour = atoi(state->valuestring);
    }

    auto attributes = cJSON_GetObjectItemCaseSensitive(root, "attributes");
    if (attributes != nullptr) {
        auto image = cJSON_GetObjectItemCaseSensitive(attributes, "image");
        if (cJSON_IsString(image) && image->valuestring != nullptr) {
            forecast.image = image->valuestring;
        }

        auto temp = cJSON_GetObjectItemCaseSensitive(attributes, "temp");
        if (cJSON_IsNumber(temp)) {
            forecast.temperature = temp->valuedouble;
        }

        // Get the wind speed
        cJSON* windbft = cJSON_GetObjectItemCaseSensitive(attributes, "windbft");
        if (cJSON_IsNumber(windbft)) {
            forecast.wind_speed = windbft->valueint;
        }
    }

    cJSON_Delete(root);
}

void Application::register_mqtt_callbacks() {
    get_mqtt_connection().register_callback("screen_on", [this](auto data) {
        bool is_on = data == "true";

        get_queue().enqueue([this, is_on]() { screen_on_changed(is_on); });
    });

    get_mqtt_connection().register_callback("forecast_hour_1", [this](auto data) {
        parse_hour_forecast(data.c_str(), _forecast_hours[0]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_hour_2", [this](auto data) {
        parse_hour_forecast(data.c_str(), _forecast_hours[1]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_hour_3", [this](auto data) {
        parse_hour_forecast(data.c_str(), _forecast_hours[2]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_hour_4", [this](auto data) {
        parse_hour_forecast(data.c_str(), _forecast_hours[3]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_day_1", [this](auto data) {
        parse_day_forecast(data.c_str(), _forecast_days[0]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_day_2", [this](auto data) {
        parse_day_forecast(data.c_str(), _forecast_days[1]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_day_3", [this](auto data) {
        parse_day_forecast(data.c_str(), _forecast_days[2]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_day_4", [this](auto data) {
        parse_day_forecast(data.c_str(), _forecast_days[3]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("forecast_day_5", [this](auto data) {
        parse_day_forecast(data.c_str(), _forecast_days[4]);
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("outside_temperature", [this](auto data) {
        _outside_temperature = atof(data.c_str());
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("woonkamer_humidity", [this](auto data) {
        _woonkamer_humidity = atof(data.c_str());
        _update_cookie++;
    });

    get_mqtt_connection().register_callback("printer_voortgang", [this](auto data) {
        _printer_voortgang = atof(data.c_str());
        _update_cookie++;
    });
}

void Application::screen_on_changed(bool is_on) {
    _current_ui = is_on ? (LvglUI*)_clock_ui : _shutdown_ui;
    _current_ui->render();
}

void Application::parse_day_forecast(const char* json, ForecastDay& forecast) {
    forecast.weekday = 0;
    forecast.weekday_code = "";
    forecast.min_temperature = 0;
    forecast.max_temperature = 0;
    forecast.percent_rain = 0;
    forecast.percent_sun = 0;
    forecast.image = "";

    auto root = cJSON_Parse(json);
    if (root == nullptr) {
        return;
    }

    auto state = cJSON_GetObjectItemCaseSensitive(root, "state");
    if (cJSON_IsString(state) && state->valuestring != nullptr) {
        forecast.weekday = atoi(state->valuestring);
        forecast.weekday_code = get_weekday_code(forecast.weekday);
    }

    auto attributes = cJSON_GetObjectItemCaseSensitive(root, "attributes");
    if (attributes != nullptr) {
        auto image = cJSON_GetObjectItemCaseSensitive(attributes, "image");
        if (cJSON_IsString(image) && image->valuestring != nullptr) {
            forecast.image = image->valuestring;
        }

        auto min_temp = cJSON_GetObjectItemCaseSensitive(attributes, "min_temp");
        if (cJSON_IsNumber(min_temp)) {
            forecast.min_temperature = min_temp->valuedouble;
        }

        auto max_temp = cJSON_GetObjectItemCaseSensitive(attributes, "max_temp");
        if (cJSON_IsNumber(max_temp)) {
            forecast.max_temperature = max_temp->valuedouble;
        }

        auto neersl_perc_dag = cJSON_GetObjectItemCaseSensitive(attributes, "neersl_perc_dag");
        if (cJSON_IsNumber(neersl_perc_dag)) {
            forecast.percent_rain = neersl_perc_dag->valuedouble;
        }

        auto zond_perc_dag = cJSON_GetObjectItemCaseSensitive(attributes, "zond_perc_dag");
        if (cJSON_IsNumber(zond_perc_dag)) {
            forecast.percent_sun = zond_perc_dag->valuedouble;
        }
    }

    cJSON_Delete(root);
}

string Application::get_weekday_code(int weekday) {
    switch (weekday) {
        case 0:
            return "zo";
        case 1:
            return "ma";
        case 2:
            return "di";
        case 3:
            return "wo";
        case 4:
            return "do";
        case 5:
            return "vr";
        case 6:
        default:
            return "za";
    }
}