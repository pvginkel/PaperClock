#pragma once

static constexpr auto NORMAL_FONT = &lv_font_sans_28;
static constexpr auto LARGE_FONT = &lv_font_sans_50_bold;

void lv_obj_set_bounds(lv_obj_t* obj, int x, int y, int width, int height, lv_text_align_t align);

class LvglUI;

class LvglUICookie {
    uint32_t _cookie;

public:
    LvglUICookie(uint32_t cookie) : _cookie(cookie) {}

    bool is_valid() const;
};

class LvglUI {
    static atomic<uint32_t> _current_cookie;

    vector<lv_obj_t*> _loading_circles;

    friend LvglUICookie;

public:
    LvglUI() {}
    LvglUI(const LvglUI& other) = delete;
    LvglUI(LvglUI&& other) noexcept = delete;
    LvglUI& operator=(const LvglUI& other) = delete;
    LvglUI& operator=(LvglUI&& other) noexcept = delete;
    virtual ~LvglUI();

    void begin();
    void render();

protected:
    virtual void do_render(lv_obj_t* parent) = 0;
    virtual void do_begin() {}

    lv_coord_t pw(double value) const { return lv_coord_t(LV_HOR_RES * (value / 100)); }
    lv_coord_t ph(double value) const { return lv_coord_t(LV_VER_RES * (value / 100)); }
    void render_loading_ui(lv_obj_t* parent);
    static void loading_animation_callback(void* var, int32_t v);
    void remove_loading_ui();
    void reset_outer_container_styles(lv_obj_t* cont);
    LvglUICookie get_cookie() const { return {_current_cookie}; }
    void style_icon_button(lv_obj_t* button);
};
