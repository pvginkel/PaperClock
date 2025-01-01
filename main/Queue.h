#pragma once

#ifdef LV_SIMULATOR
#include <deque>
#endif

class Queue {
#ifndef LV_SIMULATOR
    QueueHandle_t _queue;
#else
    deque<function<void()>> _queue;
#endif

public:
    Queue();

    void enqueue(const function<void()> &task);
    void process();
};