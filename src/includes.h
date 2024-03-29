#pragma once

#define _USE_MATH_DEFINES

#include <curl/curl.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <unicode/locid.h>
#include <unicode/normlzr.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>

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

#include "Callback.h"
#include "Messages.h"
#include "RefCounted.h"
#include "fonts.h"
#include "lvgl_events.h"
#include "result.h"
#include "settings.h"
#include "support.h"