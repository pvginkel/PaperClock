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
#endif

    _queue.process();
}
