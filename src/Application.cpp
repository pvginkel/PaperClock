#include "includes.h"

#include "Application.h"

LOG_TAG(Application);

void Application::begin() {
    LOGI(TAG, "Starting...");

    auto address = getenv("MQTT_ADDRESS");
    auto user_name = getenv("MQTT_USER_NAME");
    auto password = getenv("MQTT_PASSWORD");

    if (!address) {
        LOGE(TAG, "MQTT_ADDRESS environment variable not set");
        exit(1);
    }
    if (!user_name) {
        LOGE(TAG, "MQTT_USER_NAME environment variable not set");
        exit(1);
    }
    if (!password) {
        LOGE(TAG, "MQTT_PASSWORD environment variable not set");
        exit(1);
    }

    _api = new HomeAssistantApi(_device, address, user_name, password);
    if (!_api->begin()) {
        LOGE(TAG, "Failed to begin API");
        exit(2);
    }

#ifdef TEST_CLOCK
    _test_clock = new TestClockUI();

    _test_clock->begin();
    _test_clock->render();
#else
    _clock = new ClockUI(&_tasks, &_queue, _api);

    _clock->begin();
    _clock->render();
#endif
}

void Application::process() {
#ifdef TEST_CLOCK
    _test_clock->update();
#else
    _clock->update();
#endif

    _queue.process();
}
