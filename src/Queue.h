#pragma once

class Queue {
    mutex _mutex;
    queue<function<void()>> _queue;

public:
    Queue();

    void enqueue(const function<void()>& task);
    void process();
};