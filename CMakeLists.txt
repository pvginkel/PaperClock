cmake_minimum_required(VERSION 3.10)
project(KitchenDisplay)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lib/lvgl)
add_subdirectory(lib/cJSON)
add_subdirectory(lib/cmark)

target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR}/src)

FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(main ${SOURCES})

target_link_libraries(main lvgl m pthread cmark cjson curl crypto icuuc icui18n backtrace)

target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/lib/lvgl ${PROJECT_SOURCE_DIR}/lib/cJSON ${PROJECT_SOURCE_DIR}/lib/cmark/src ${PROJECT_SOURCE_DIR}/build/lib/cmark/src)
