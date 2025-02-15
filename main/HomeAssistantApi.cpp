#include "includes.h"

#include "HomeAssistantApi.h"

#define TOPIC_PREFIX "pieter/Paper Clock/"

constexpr auto QOS_MAX_ONE = 0;      // Send at most one.
constexpr auto QOS_MIN_ONE = 1;      // Send at least one.
constexpr auto QOS_EXACTLY_ONE = 2;  // Send exactly one.

LOG_TAG(HomeAssistantApi);

#ifndef LV_SIMULATOR

void HomeAssistantApi::begin() {
    esp_mqtt5_connection_property_config_t connect_property = {
        .session_expiry_interval = 10,
        .maximum_packet_size = 1024,
        .receive_maximum = 65535,
        .topic_alias_maximum = 2,
        .request_resp_info = true,
        .request_problem_info = true,
        .will_delay_interval = 10,
        .message_expiry_interval = 10,
        .payload_format_indicator = true,
    };

    const auto lastWillMessage = "offline";

    esp_mqtt_client_config_t configuration = {
        .broker =
            {
                .address =
                    {
                        .uri = CONFIG_MQTT_BROKER_URL,
                    },
            },
        .credentials =
            {
                .username = CONFIG_MQTT_USER_ID,
                .authentication =
                    {
                        .password = CONFIG_MQTT_PASSWORD,
                    },
            },
        .session =
            {
                .last_will =
                    {
                        .topic = TOPIC_PREFIX "state",
                        .msg = lastWillMessage,
                        .msg_len = strlen(lastWillMessage),
                        .qos = QOS_MIN_ONE,
                        .retain = true,
                    },
                .protocol_ver = MQTT_PROTOCOL_V_5,
            },
        .network =
            {
                .disable_auto_reconnect = false,
            },
    };

    _client = esp_mqtt_client_init(&configuration);

    esp_mqtt5_client_set_connect_property(_client, &connect_property);

    esp_mqtt_client_register_event(
        _client, MQTT_EVENT_ANY,
        [](auto eventHandlerArg, auto eventBase, auto eventId, auto eventData) {
            ((HomeAssistantApi *)eventHandlerArg)->eventHandler(eventBase, eventId, eventData);
        },
        this);

    esp_mqtt_client_start(_client);
}

void HomeAssistantApi::eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, eventBase, eventId);
    auto event = (esp_mqtt_event_handle_t)eventData;

    ESP_LOGD(TAG, "Free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(),
             esp_get_minimum_free_heap_size());

    switch ((esp_mqtt_event_id_t)eventId) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            handleConnected();
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");

            _state_changed.queue(_queue, {.connected = false});
            break;

        case MQTT_EVENT_SUBSCRIBED:
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            break;

        case MQTT_EVENT_PUBLISHED:
            break;

        case MQTT_EVENT_DATA:
            handleData(event);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT return code is %d", event->error_handle->connect_return_code);
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                if (event->error_handle->esp_tls_last_esp_err) {
                    ESP_LOGI(TAG, "reported from esp-tls");
                }
                if (event->error_handle->esp_tls_stack_err) {
                    ESP_LOGI(TAG, "reported from tls stack");
                }
                if (event->error_handle->esp_transport_sock_errno) {
                    ESP_LOGI(TAG, "captured as transport's socket errno");
                }
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;

        default:
            ESP_LOGD(TAG, "Other event id: %d", event->event_id);
            break;
    }
}

void HomeAssistantApi::handleConnected() {
    subscribe(TOPIC_PREFIX "#");

    setOnline();

    _state_changed.queue(_queue, {.connected = true});
}

void HomeAssistantApi::setOnline() {
    ESP_ERROR_ASSERT(esp_mqtt_client_publish(_client, TOPIC_PREFIX "state", "online", 0, QOS_MIN_ONE, true) >= 0);
}

void HomeAssistantApi::handleData(esp_mqtt_event_handle_t event) {
    auto topic = string(event->topic, event->topic_len);
    auto data = string(event->data, event->data_len);

    ESP_LOGI(TAG, "Received data topic %s data '%s'", topic.c_str(), data.c_str());

    if (strncmp(topic.c_str(), TOPIC_PREFIX, strlen(TOPIC_PREFIX)) == 0) {
        auto sub_topic = topic.c_str() + strlen(TOPIC_PREFIX);

        if (strcmp(sub_topic, "set/screen_on") == 0) {
            _screen_on_changed.queue(_queue, strcmp(data.c_str(), "true") == 0);
        } else if (strcmp(sub_topic, "set/forecast_hour_1") == 0) {
            parse_hour_forecast(data.c_str(), _forecast_hours[0]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_2") == 0) {
            parse_hour_forecast(data.c_str(), _forecast_hours[1]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_3") == 0) {
            parse_hour_forecast(data.c_str(), _forecast_hours[2]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_4") == 0) {
            parse_hour_forecast(data.c_str(), _forecast_hours[3]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_day_1") == 0) {
            parse_day_forecast(data.c_str(), _forecast_days[0]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_day_2") == 0) {
            parse_day_forecast(data.c_str(), _forecast_days[1]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_day_3") == 0) {
            parse_day_forecast(data.c_str(), _forecast_days[2]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_day_4") == 0) {
            parse_day_forecast(data.c_str(), _forecast_days[3]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_day_5") == 0) {
            parse_day_forecast(data.c_str(), _forecast_days[4]);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/outside_temperature") == 0) {
            _outside_temperature = atof(data.c_str());
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/woonkamer_humidity") == 0) {
            _woonkamer_humidity = atof(data.c_str());
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/printer_voortgang") == 0) {
            _printer_voortgang = atof(data.c_str());
            _update_cookie++;
        }
    }
}

void HomeAssistantApi::subscribe(const char *topic) {
    ESP_LOGI(TAG, "Subscribing to topic %s", topic);

    ESP_ERROR_ASSERT(esp_mqtt_client_subscribe(_client, topic, 0) >= 0);
}

#else

void HomeAssistantApi::begin() {
    parse_hour_forecast(
        R"({"entity_id": "sensor.weerlive_forecast_hour_1", "state": "17", "attributes": {"image": "regen", "temp": 16, "windbft": 3, "friendly_name": "WeerLive Forecast Hour 1"}, "last_changed": "2024-07-05T15:34:53.398432+00:00", "last_reported": "2024-07-05T15:34:53.398432+00:00", "last_updated": "2024-07-05T15:34:53.398432+00:00", "context": {"id": "01J21RHDPPEEYXWAJY1KT4670R", "parent_id": null, "user_id": null}})",
        _forecast_hours[0]);
    parse_day_forecast(
        R"({"entity_id": "sensor.weerlive_forecast_day_1", "state": "5", "attributes": {"image": "halfbewolkt", "min_temp": 6, "max_temp": 7, "neersl_perc_dag": 20, "zond_perc_dag": 22, "friendly_name": "WeerLive Forecast Day 1"}, "last_changed": "2024-07-05T15:34:53.400550+00:00", "last_reported": "2024-07-05T15:34:53.400550+00:00", "last_updated": "2024-07-05T15:34:53.400550+00:00", "context": {"id": "01J21RHDPRZGB0E6Y8V9HVK1DD", "parent_id": null, "user_id": null}})",
        _forecast_days[0]);

    for (auto i = 1; i < 4; i++) {
        _forecast_hours[i] = _forecast_hours[0];
        _forecast_hours[i].hour += i;
    }

    for (auto i = 1; i < 5; i++) {
        _forecast_days[i] = _forecast_days[0];
        _forecast_days[i].weekday = (_forecast_days[i].weekday + i) % 7;
        _forecast_days[i].weekday_code = get_weekday_code(_forecast_days[i].weekday);
    }
}

void HomeAssistantApi::end() {}

#endif

void HomeAssistantApi::parse_hour_forecast(const char *json, ForecastHour &forecast) {
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
        cJSON *windbft = cJSON_GetObjectItemCaseSensitive(attributes, "windbft");
        if (cJSON_IsNumber(windbft)) {
            forecast.wind_speed = windbft->valueint;
        }
    }

    cJSON_Delete(root);
}

void HomeAssistantApi::parse_day_forecast(const char *json, ForecastDay &forecast) {
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

string HomeAssistantApi::get_weekday_code(int weekday) {
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
