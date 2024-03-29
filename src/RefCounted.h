#pragma once

class RefCounted {
    int _count;

public:
    RefCounted() : _count(1) {}
    RefCounted(const RefCounted& other) = delete;
    RefCounted(RefCounted&& other) noexcept = delete;
    RefCounted& operator=(const RefCounted& other) = delete;
    RefCounted& operator=(RefCounted&& other) noexcept = delete;
    virtual ~RefCounted() {}

    void add_ref() { _count++; }

    void remove_ref() {
        if (--_count == 0) {
            delete this;
        }
    }
};
