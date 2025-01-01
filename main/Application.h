#pragma once

#include "ClockUI.h"
#include "LoadingUI.h"
#include "LogManager.h"
#include "NetworkConnection.h"
#include "OTAManager.h"
#include "Queue.h"
#include "ShutdownUI.h"

class Application {
    Device* _device;
    NetworkConnection _network_connection;
    OTAManager _ota_manager;
    LoadingUI* _loading_ui{};
    ClockUI* _clock_ui{};
    ShutdownUI* _shutdown_ui{};
    LvglUI* _current_ui{};
    Queue _queue;
    DeviceConfiguration _configuration;
    LogManager _log_manager;
    HomeAssistantApi* _api;
    bool _silent_startup;

public:
    Application(Device* device);

    void begin(bool silent);

private:
    void setup_flash();
    void do_begin(bool silent);
    void begin_network();
    void begin_network_available();
    void begin_mqtt();
    void begin_after_initialization();
    void begin_ui();
    void run();
    void process();
};
