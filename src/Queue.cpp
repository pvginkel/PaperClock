#include "includes.h"

#include "Queue.h"

Queue::Queue() {}

void Queue::enqueue(const function<void()>& task) {
    with_mutex(_mutex, [this, task]() { _queue.push(task); });
}

void Queue::process() {
    while (true) {
        auto task = with_mutex<optional<function<void()>>>(_mutex, [this]() -> optional<function<void()>> {
            if (_queue.empty()) {
                return nullopt;
            }

            auto result = _queue.front();
            _queue.pop();
            return result;
        });

        if (!task.has_value()) {
            return;
        }

        task.value()();
    }
}
