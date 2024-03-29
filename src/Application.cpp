#include "includes.h"

#include "Application.h"

LOG_TAG(Application);

void Application::begin() {
    LOGI(TAG, "Starting...");

    auto address = getenv("MQTT_ADDRESS");
    auto client_id = getenv("MQTT_PASSWORD");

    if (!address) {
        LOGE(TAG, "MQTT_ADDRESS environment variable not set");
        exit(1);
    }
    if (!client_id) {
        LOGE(TAG, "MQTT_PASSWORD environment variable not set");
        exit(1);
    }

    _api = new HomeAssistantApi(address, client_id);

    _clock = new ClockUI(&_tasks, &_queue, _api);

    _clock->begin();
    _clock->render();
}

void Application::process() { _queue.process(); }
