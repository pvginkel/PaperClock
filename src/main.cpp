#include "includes.h"

#ifndef LV_SIMULATOR

#include "Application.h"
#include "Device.h"
#include "Queue.h"
#include "backtrace.h"

LOG_TAG(main);

struct bt_ctx {
    struct backtrace_state *state;
    int error;
};

static void error_callback(void *data, const char *msg, int errnum) {
    struct bt_ctx *ctx = (bt_ctx *)data;
    fprintf(stderr, "ERROR: %s (%d)", msg, errnum);
    ctx->error = 1;
}

static void syminfo_callback(void *data, uintptr_t pc, const char *symname, uintptr_t symval, uintptr_t symsize) {
    // struct bt_ctx *ctx = data;
    if (symname) {
        printf("%lx %s ??:0\n", (unsigned long)pc, symname);
    } else {
        printf("%lx ?? ??:0\n", (unsigned long)pc);
    }
}

static int full_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function) {
    struct bt_ctx *ctx = (bt_ctx *)data;
    if (function) {
        printf("%lx %s %s:%d\n", (unsigned long)pc, function, filename ? filename : "??", lineno);
    } else {
        backtrace_syminfo(ctx->state, pc, syminfo_callback, error_callback, data);
    }
    return 0;
}

static int simple_callback(void *data, uintptr_t pc) {
    struct bt_ctx *ctx = (bt_ctx *)data;
    backtrace_pcinfo(ctx->state, pc, full_callback, error_callback, data);
    return 0;
}

static inline void bt(struct backtrace_state *state) {
    struct bt_ctx ctx = {state, 0};
    // backtrace_print(state, 0, stdout);
    backtrace_simple(state, 0, simple_callback, error_callback, &ctx);
}

static void handle_abort() {
    struct backtrace_state *state = backtrace_create_state(nullptr, 1, error_callback, NULL);
    bt(state);
}

int main() {
    set_terminate(handle_abort);

    Device device;

    if (!device.begin()) {
        LOGE(TAG, "Device begin failed");
        return 1;
    }

    device.clear_screen();

    Application application(&device);

    application.begin();

    auto last_tick_call = chrono::high_resolution_clock::now();

    while (true) {
        auto start = chrono::high_resolution_clock::now();

        application.process();

        lv_timer_handler();

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

        auto sleep = 5000 - duration;
        if (sleep > 0) {
            usleep(sleep);
        }

        auto after_sleep = chrono::high_resolution_clock::now();
        auto last_tick_duration = chrono::duration_cast<chrono::milliseconds>(after_sleep - last_tick_call).count();

        lv_tick_inc(last_tick_duration);

        last_tick_call = after_sleep;
    }

    return 0;
}

#endif
