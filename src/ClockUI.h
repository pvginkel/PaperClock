#pragma once
#pragma once

#include "LvglUI.h"
#include "Queue.h"
#include "Tasks.h"
#include "HomeAssistantApi.h"

class ClockUI : public LvglUI {
    Tasks* _tasks;
    Queue* _queue;
    HomeAssistantApi* _api;

public:
    ClockUI(Tasks* tasks, Queue* queue, HomeAssistantApi* api)
        : _tasks(tasks), _queue(queue), _api(api) {}

protected:
    void do_render(lv_obj_t* parent) override;

private:
    void quit();
};
