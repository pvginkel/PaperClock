# Instructions on installing FreeType:
# https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/INSTALL.UNIX



cmake_minimum_required(VERSION 3.10)
project(PaperClock)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lib/lvgl)

set(ENABLE_CJSON_TEST OFF CACHE BOOL "Enable building cJSON test" FORCE)
add_subdirectory(lib/cJSON)

set(PAHO_ENABLE_TESTING FALSE CACHE BOOL "Build tests and run" FORCE)
set(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. " FORCE)
add_subdirectory(lib/paho)

target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR}/src /usr/local/include/freetype2)

FILE(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(main ${SOURCES})

target_compile_definitions(main PRIVATE LGPIO=1)

target_link_libraries(main lvgl m pthread cjson backtrace freetype lgpio paho-mqtt3c)

target_include_directories(
    main PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib/lvgl
    ${PROJECT_SOURCE_DIR}/lib/cJSON
    ${PROJECT_SOURCE_DIR}/lib/paho/src
)
