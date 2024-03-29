#include "includes.h"

#include "Tasks.h"

void Tasks::run(function<void()> func) {
    // TODO: This implementation can be improved by reusing threads we create.

    thread thread(func);

    thread.detach();
}
