#pragma once

#define _USE_MATH_DEFINES

#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#ifndef LV_SIMULATOR

#include <sys/unistd.h>

#endif

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <queue>
#include <ranges>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <vector>

#include "cJSON.h"
#include "lvgl.h"

using namespace std;

#if LV_SIMULATOR
#define FONTS_PREFIX "../../fonts/"
#else
#define FONTS_PREFIX ""
#endif

#include "Callback.h"
#include "RefCounted.h"
#include "fonts.h"
#include "lvgl_events.h"
#include "result.h"
#include "settings.h"
#include "support.h"