#include "includes.h"

#include "HomeAssistantApi.h"

#define TOPIC_PREFIX "pieter/Paper Clock/"

static constexpr auto QOS_NONE = 0;
static constexpr auto QOS_ONE = 1;
static constexpr auto QOS_EXACTLY_ONE = 2;

LOG_TAG(HomeAssistantApi);

bool HomeAssistantApi::begin() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    conn_opts.username = _user_name.c_str();
    conn_opts.password = _password.c_str();

    if ((rc = MQTTClient_create(&_client, _address.c_str(), _client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE,
                                nullptr)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_create failed with %d", rc);
        return false;
    }

    if ((rc = MQTTClient_setCallbacks(
             _client, this, [](void *context, char *cause) { ((HomeAssistantApi *)context)->connection_lost(cause); },
             [](void *context, char *topicName, int topicLen, MQTTClient_message *message) {
                 return ((HomeAssistantApi *)context)->message_arrived(topicName, topicLen, message);
             },
             [](void *context, MQTTClient_deliveryToken dt) { ((HomeAssistantApi *)context)->delivered(dt); })) !=
        MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_setCallbacks failed with %d", rc);
        return false;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;

    will_opts.message = "offline";
    will_opts.topicName = TOPIC_PREFIX "state";
    will_opts.qos = QOS_ONE;
    will_opts.retained = true;

    conn_opts.will = &will_opts;

    if ((rc = MQTTClient_connect(_client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_connect failed with %d", rc);
        return false;
    }

    if ((rc = MQTTClient_subscribe(_client, TOPIC_PREFIX "#", QOS_ONE)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_connect failed with %d", rc);
        return false;
    }

    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = (void *)"online";
    pubmsg.payloadlen = (int)strlen((char *)pubmsg.payload);
    pubmsg.qos = QOS_ONE;
    pubmsg.retained = true;

    if ((rc = MQTTClient_publishMessage(_client, TOPIC_PREFIX "state", &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_publishMessage failed with %d", rc);
        return false;
    }

    return true;
}

void HomeAssistantApi::connection_lost(char *cause) { LOGE(TAG, "Connected lost: %s", cause); }

int HomeAssistantApi::message_arrived(char *topic_name, int topic_len, MQTTClient_message *message) {
    // LOGI(TAG, "Received on topic %s payload %s", topic_name, (char *)message->payload);

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

    return 1;
}

void HomeAssistantApi::delivered(MQTTClient_deliveryToken dt) {}

void HomeAssistantApi::end() {
    int rc;

    if ((rc = MQTTClient_disconnect(_client, 10'000)) != MQTTCLIENT_SUCCESS) {
        LOGE(TAG, "MQTTClient_disconnect failed with %d", rc);
    }

    MQTTClient_destroy(&_client);
}
