#include "includes.h"

class LvglEventManager : public RefCounted {
    map<lv_event_code_t, Callback<void>> _event_map;

public:
    void add_event(lv_event_code_t event, function<void()> func) { _event_map[event].add(func); }

    static void cb(lv_event_t* event) {
        auto self = (LvglEventManager*)lv_event_get_user_data(event);

        self->add_ref();

        auto event_code = lv_event_get_code(event);
        auto it = self->_event_map.find(event_code);
        if (it != self->_event_map.end()) {
            it->second.call();
        }

        if (event_code == LV_EVENT_DELETE) {
            self->remove_ref();
        }

        self->remove_ref();
    }
};

void lv_obj_add_event(lv_obj_t* obj, lv_event_code_t event, function<void()> func) {
    LvglEventManager* event_manager = nullptr;

    for (auto i = (uint32_t)0, event_cnt = lv_obj_get_event_count(obj); i < event_cnt; i++) {
        lv_event_dsc_t* event_dsc = lv_obj_get_event_dsc(obj, i);
        if (lv_event_dsc_get_cb(event_dsc) == LvglEventManager::cb) {
            event_manager = (LvglEventManager*)lv_event_dsc_get_user_data(event_dsc);
            break;
        }
    }

    if (!event_manager) {
        event_manager = new LvglEventManager();
        lv_obj_add_event_cb(obj, LvglEventManager::cb, LV_EVENT_ALL, event_manager);
    }

    event_manager->add_event(event, func);
}

#define lvgl_define_event(uname, lname)                                    \
    void lv_obj_on_##lname(lv_obj_t* obj, function<void()> func) { \
        lv_obj_add_event(obj, LV_EVENT_##uname, func);                     \
    }

lvgl_define_event(PRESSED, pressed);
lvgl_define_event(PRESSING, pressing);
lvgl_define_event(PRESS_LOST, press_lost);
lvgl_define_event(SHORT_CLICKED, short_clicked);
lvgl_define_event(LONG_PRESSED, long_pressed);
lvgl_define_event(LONG_PRESSED_REPEAT, long_pressed_repeat);
lvgl_define_event(CLICKED, clicked);
lvgl_define_event(RELEASED, released);
lvgl_define_event(SCROLL_BEGIN, scroll_begin);
lvgl_define_event(SCROLL_THROW_BEGIN, scroll_throw_begin);
lvgl_define_event(SCROLL_END, scroll_end);
lvgl_define_event(SCROLL, scroll);
lvgl_define_event(GESTURE, gesture);
lvgl_define_event(KEY, key);
lvgl_define_event(FOCUSED, focused);
lvgl_define_event(DEFOCUSED, defocused);
lvgl_define_event(LEAVE, leave);
lvgl_define_event(HIT_TEST, hit_test);
lvgl_define_event(INDEV_RESET, indev_reset);
lvgl_define_event(COVER_CHECK, cover_check);
lvgl_define_event(REFR_EXT_DRAW_SIZE, refr_ext_draw_size);
lvgl_define_event(DRAW_MAIN_BEGIN, draw_main_begin);
lvgl_define_event(DRAW_MAIN, draw_main);
lvgl_define_event(DRAW_MAIN_END, draw_main_end);
lvgl_define_event(DRAW_POST_BEGIN, draw_post_begin);
lvgl_define_event(DRAW_POST, draw_post);
lvgl_define_event(DRAW_POST_END, draw_post_end);
lvgl_define_event(DRAW_TASK_ADDED, draw_task_added);
lvgl_define_event(VALUE_CHANGED, value_changed);
lvgl_define_event(INSERT, insert);
lvgl_define_event(REFRESH, refresh);
lvgl_define_event(READY, ready);
lvgl_define_event(CANCEL, cancel);
lvgl_define_event(CREATE, create);
lvgl_define_event(DELETE, delete);
lvgl_define_event(CHILD_CHANGED, child_changed);
lvgl_define_event(CHILD_CREATED, child_created);
lvgl_define_event(CHILD_DELETED, child_deleted);
lvgl_define_event(SCREEN_UNLOAD_START, screen_unload_start);
lvgl_define_event(SCREEN_LOAD_START, screen_load_start);
lvgl_define_event(SCREEN_LOADED, screen_loaded);
lvgl_define_event(SCREEN_UNLOADED, screen_unloaded);
lvgl_define_event(SIZE_CHANGED, size_changed);
lvgl_define_event(STYLE_CHANGED, style_changed);
lvgl_define_event(LAYOUT_CHANGED, layout_changed);
lvgl_define_event(GET_SELF_SIZE, get_self_size);
lvgl_define_event(INVALIDATE_AREA, invalidate_area);
lvgl_define_event(RESOLUTION_CHANGED, resolution_changed);
lvgl_define_event(COLOR_FORMAT_CHANGED, color_format_changed);
lvgl_define_event(REFR_REQUEST, refr_request);
lvgl_define_event(REFR_START, refr_start);
lvgl_define_event(REFR_READY, refr_ready);
lvgl_define_event(RENDER_START, render_start);
lvgl_define_event(RENDER_READY, render_ready);
lvgl_define_event(FLUSH_START, flush_start);
lvgl_define_event(FLUSH_FINISH, flush_finish);
lvgl_define_event(VSYNC, vsync);
