#include "includes.h"

#include "HomeAssistantApi.h"

#define TOPIC_PREFIX "pieter/Paper Clock/"

static constexpr auto QOS_NONE = 0;
static constexpr auto QOS_ONE = 1;
static constexpr auto QOS_EXACTLY_ONE = 2;

LOG_TAG(HomeAssistantApi);

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
