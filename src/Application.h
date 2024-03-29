#pragma once

#include "ClockUI.h"
#include "Queue.h"

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
    HomeAssistantApi* _api;

public:
    Application() : _clock(nullptr), _api(nullptr) {}

    void begin();
    void process();
};
