#include "includes.h"

#include "HomeAssistantApi.h"

#define TOPIC_PREFIX "pieter/Paper Clock/"

static constexpr auto QOS_NONE = 0;
static constexpr auto QOS_ONE = 1;
static constexpr auto QOS_EXACTLY_ONE = 2;

LOG_TAG(HomeAssistantApi);

#ifndef LV_SIMULATOR

void HomeAssistantApi::begin() {
#if NDEBUG
    MQTTClient_setTraceLevel(MQTTCLIENT_TRACE_ERROR);
#endif

    MQTTClient_setTraceCallback([](enum MQTTCLIENT_TRACE_LEVELS level, char *message) {
        if (level >= MQTTCLIENT_TRACE_ERROR) {
            LOGE(TAG, "paho: %s", message);
        } else if (level != MQTTCLIENT_TRACE_MINIMUM) {
            LOGD(TAG, "paho %d: %s", (int)level, message);
        }
    });

    connect();
}

void HomeAssistantApi::connect() {
    _client = nullptr;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    int rc;

    conn_opts.username = _user_name.c_str();
    conn_opts.password = _password.c_str();

    if ((rc = MQTTClient_create(&_client, _address.c_str(), _client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE,
                                nullptr)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_create failed with %d", rc);
        goto failure;
    }

    if ((rc = MQTTClient_setCallbacks(
             _client, this,
             [](void *context, char *cause) {
                 ((HomeAssistantApi *)context)->_queue->enqueue([context, cause]() {
                     ((HomeAssistantApi *)context)->connection_lost(cause);
                 });
             },
             [](void *context, char *topicName, int topicLen, MQTTClient_message *message) {
                 ((HomeAssistantApi *)context)->_queue->enqueue([context, topicName, topicLen, message]() {
                     ((HomeAssistantApi *)context)->message_arrived(topicName, topicLen, message);
                 });
                 return 1;
             },
             [](void *context, MQTTClient_deliveryToken dt) {
                 ((HomeAssistantApi *)context)->_queue->enqueue([context, dt]() {
                     ((HomeAssistantApi *)context)->delivered(dt);
                 });
             })) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_setCallbacks failed with %d", rc);
        goto failure;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    will_opts.message = "offline";
    will_opts.topicName = TOPIC_PREFIX "state";
    will_opts.qos = QOS_ONE;
    will_opts.retained = true;

    conn_opts.will = &will_opts;

    if ((rc = MQTTClient_connect(_client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_connect failed with %d", rc);
        goto failure;
    }

    if ((rc = MQTTClient_subscribe(_client, TOPIC_PREFIX "#", QOS_ONE)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_connect failed with %d", rc);
        goto failure;
    }

    pubmsg.payload = (void *)"online";
    pubmsg.payloadlen = (int)strlen((char *)pubmsg.payload);
    pubmsg.qos = QOS_ONE;
    pubmsg.retained = true;

    if ((rc = MQTTClient_publishMessage(_client, TOPIC_PREFIX "state", &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_publishMessage failed with %d", rc);
        goto failure;
    }
    return;

failure:
    disconnect();

    const auto RECONNECT_SECONDS = 5;

    LOGI(TAG, "Reconnecting in %d seconds...", RECONNECT_SECONDS);

    lv_timer_create(
        [](lv_timer_t *timer) {
            ((HomeAssistantApi *)timer->user_data)->connect();
            lv_timer_delete(timer);
        },
        RECONNECT_SECONDS * 1000, this);
}

void HomeAssistantApi::disconnect() {
    if (!_client) {
        return;
    }

    int rc;

    if ((rc = MQTTClient_disconnect(_client, 10'000)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_disconnect failed with %d", rc);
    }

    MQTTClient_destroy(&_client);

    _client = nullptr;
}

void HomeAssistantApi::connection_lost(char *cause) {
    LOGE(TAG, "Connected lost: %s; reconnecting...", cause);

    connect();
}

void HomeAssistantApi::message_arrived(char *topic_name, int topic_len, MQTTClient_message *message) {
    LOGD(TAG, "Received on topic %s payload %s", topic_name, (char *)message->payload);

    if (strncmp(topic_name, TOPIC_PREFIX, strlen(TOPIC_PREFIX)) == 0) {
        auto sub_topic = topic_name + strlen(TOPIC_PREFIX);

        if (strcmp(sub_topic, "set/screen_on") == 0) {
            _screen_on_changed.call(strcmp((char *)message->payload, "true") == 0);
        } else if (strcmp(sub_topic, "set/forecast_hour_1_image") == 0) {
            _forecast_hour_1_image = (char *)message->payload;
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_1_wind_speed") == 0) {
            _forecast_hour_1_wind_speed = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_1_hour") == 0) {
            _forecast_hour_1_hour = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_2_image") == 0) {
            _forecast_hour_2_image = (char *)message->payload;
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_2_wind_speed") == 0) {
            _forecast_hour_2_wind_speed = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_2_hour") == 0) {
            _forecast_hour_2_hour = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_3_image") == 0) {
            _forecast_hour_3_image = (char *)message->payload;
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_3_wind_speed") == 0) {
            _forecast_hour_3_wind_speed = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_hour_3_hour") == 0) {
            _forecast_hour_3_hour = atoi((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/outside_temperature") == 0) {
            _outside_temperature = atof((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_temperature_low") == 0) {
            _forecast_temperature_low = atof((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/forecast_temperature_high") == 0) {
            _forecast_temperature_high = atof((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/woonkamer_humidity") == 0) {
            _woonkamer_humidity = atof((char *)message->payload);
            _update_cookie++;
        } else if (strcmp(sub_topic, "set/printer_vooruitgang") == 0) {
            _printer_vooruitgang = atof((char *)message->payload);
            _update_cookie++;
        }
    }

    MQTTClient_free(topic_name);
    MQTTClient_freeMessage(&message);
}

void HomeAssistantApi::delivered(MQTTClient_deliveryToken dt) {}

void HomeAssistantApi::end() { disconnect(); }

#else

void HomeAssistantApi::begin() {
    parse_hour_forecast(
        R"({"entity_id": "sensor.weerlive_forecast_hour_1", "state": "17", "attributes": {"image": "regen", "temp": 16, "windbft": 3, "friendly_name": "WeerLive Forecast Hour 1"}, "last_changed": "2024-07-05T15:34:53.398432+00:00", "last_reported": "2024-07-05T15:34:53.398432+00:00", "last_updated": "2024-07-05T15:34:53.398432+00:00", "context": {"id": "01J21RHDPPEEYXWAJY1KT4670R", "parent_id": null, "user_id": null}})",
        _forecast_hours[0]);
    parse_day_forecast(
        R"({"entity_id": "sensor.weerlive_forecast_day_1", "state": "5", "attributes": {"image": "halfbewolkt", "min_temp": 16, "max_temp": 17, "neersl_perc_dag": 20, "zond_perc_dag": 22, "friendly_name": "WeerLive Forecast Day 1"}, "last_changed": "2024-07-05T15:34:53.400550+00:00", "last_reported": "2024-07-05T15:34:53.400550+00:00", "last_updated": "2024-07-05T15:34:53.400550+00:00", "context": {"id": "01J21RHDPRZGB0E6Y8V9HVK1DD", "parent_id": null, "user_id": null}})",
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
