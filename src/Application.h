#pragma once

#include "ClockUI.h"
#include "Queue.h"
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

    Queue _queue;
    Tasks _tasks;
    ClockUI* _clock;
#ifdef TEST_CLOCK
    TestClockUI* _test_clock = nullptr;
#endif
    HomeAssistantApi* _api;

public:
    Application() : _clock(nullptr), _api(nullptr) {}

    void begin();
    void process();
};
