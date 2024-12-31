#pragma once

#include "ClockUI.h"
#include "Device.h"
#include "Queue.h"
#include "ShutdownUI.h"
#include "TestClockUI.h"

#if false
#define TEST_CLOCK
#endif

class Application {
public:
#if NDEBUG
    static constexpr auto PRODUCTION = true;
#else
    static constexpr auto PRODUCTION = false;
#endif

    Device* _device;
    Queue _queue;
    Tasks _tasks;
    LvglUI* _current_screen;
    ClockUI* _clock;
    ShutdownUI* _shutdown;
#ifdef TEST_CLOCK
    TestClockUI* _test_clock;
#endif
    HomeAssistantApi* _api;

public:
    Application(Device* device) : _device(device) {}

    void begin();
    void process();
};
