#pragma once

string strformat(const char* fmt, ...);

template <typename Result>
static Result with_mutex(mutex& mutex, function<Result()> func) {
    auto guard = lock_guard<std::mutex>(mutex);

    return func();
}

static void with_mutex(mutex& mutex, function<void()> func) {
    auto guard = lock_guard<std::mutex>(mutex);

    func();
}

#define LOG_TAG(name) static const char* TAG = #name
#define LOGE(tag, format, ...) printf("\033[31mERROR [%s] " format "\033[0m\n", tag, ##__VA_ARGS__)
#define LOGW(tag, format, ...) printf("\033[33mWARN [%s] " format "\033[0m\n", tag, ##__VA_ARGS__)
#define LOGI(tag, format, ...) printf("\033[97mINFO [%s] " format "\033[0m\n", tag, ##__VA_ARGS__)
#define LOGD(tag, format, ...) printf("DEBUG [%s] " format "\n", tag, ##__VA_ARGS__)

class cJSON_Data {
    cJSON* _data;

public:
    cJSON_Data(cJSON* data) : _data(data) {}
    cJSON_Data(const cJSON_Data& other) = delete;
    cJSON_Data(cJSON_Data&& other) noexcept = delete;
    cJSON_Data& operator=(const cJSON_Data& other) = delete;
    cJSON_Data& operator=(cJSON_Data&& other) noexcept = delete;

    ~cJSON_Data() {
        if (_data) {
            cJSON_Delete(_data);
        }
    }

    cJSON* operator*() const { return _data; }
};
