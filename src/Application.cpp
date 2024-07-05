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

    _api = new HomeAssistantApi(&_queue, address, user_name, password);
    _api->begin();

    _api->on_screen_on_changed([this](bool on) {
        if (on) {
            _current_screen =
#ifdef TEST_CLOCK
                _test_clock;
#else
                _clock;
#endif
            _current_screen->render();

            _device->set_on(true);
        } else {
            _device->clear_screen();

            _current_screen = _shutdown;
            _current_screen->render();

            lv_timer_create(
                [](lv_timer_t* timer) {
                    ((Application*)timer->user_data)->_device->set_on(false);
                    lv_timer_delete(timer);
                },
                1000, this);
        }
    });

#ifdef TEST_CLOCK
    _test_clock = new TestClockUI();
    _test_clock->begin();

    _current_screen = _test_clock;
#else
    _clock = new ClockUI(_api);
    _clock->begin();

    _current_screen = _clock;
#endif

    _shutdown = new ShutdownUI();
    _shutdown->begin();

    _current_screen->render();
}

void Application::process() {
    _current_screen->update();

    _queue.process();
}
