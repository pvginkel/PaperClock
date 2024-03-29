#include "includes.h"

#include "ClockUI.h"

#include "Application.h"

LOG_TAG(ClockUI);

void ClockUI::do_render(lv_obj_t* parent) {
}

void ClockUI::quit() { system("shutdown -h now"); }
