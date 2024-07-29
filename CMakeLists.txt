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
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    CACHE PATH "Output directory for shared libraries.")

add_subdirectory(lib/lvgl)

set(ENABLE_CJSON_TEST OFF CACHE BOOL "Enable building cJSON test" FORCE)
add_subdirectory(lib/cJSON)

set(PAHO_ENABLE_TESTING FALSE CACHE BOOL "Build tests and run" FORCE)
set(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. " FORCE)
add_subdirectory(lib/paho)

target_include_directories(
    lvgl PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/build/lib/freetype/include/freetype2
)

FILE(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(main ${SOURCES})

target_compile_definitions(main PRIVATE LGPIO=1)

find_library(BACKTRACE_LIB backtrace HINTS ${PROJECT_SOURCE_DIR}/build/lib/libbacktrace/lib)
find_library(LGPIO_LIB lgpio HINTS ${PROJECT_SOURCE_DIR}/build/lib/lg/lib)
find_library(FREETYPE_LIB freetype HINTS ${PROJECT_SOURCE_DIR}/build/lib/freetype/lib)

target_link_libraries(main lvgl m pthread cjson ${BACKTRACE_LIB} ${LGPIO_LIB} ${FREETYPE_LIB} paho-mqtt3c)

target_include_directories(
    main PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lib/lvgl
    ${PROJECT_SOURCE_DIR}/lib/cJSON
    ${PROJECT_SOURCE_DIR}/lib/paho/src
    ${PROJECT_SOURCE_DIR}/build/lib/lg/include
    ${PROJECT_SOURCE_DIR}/build/lib/libbacktrace/include
)

set(DEPENDENCIES
    ${PROJECT_SOURCE_DIR}/build/lib/libbacktrace/lib
    ${PROJECT_SOURCE_DIR}/build/lib/lg/lib
    ${PROJECT_SOURCE_DIR}/build/lib/freetype/lib
    ${PROJECT_SOURCE_DIR}/tools/fonts
)

add_custom_target(copy_dependencies ALL
    COMMENT "Copying dependencies to ${PROJECT_SOURCE_DIR}/bin"
)

foreach(LIBRARY ${DEPENDENCIES})
    add_custom_command(
        TARGET copy_dependencies
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${LIBRARY} ${PROJECT_SOURCE_DIR}/bin
        COMMENT "Copying files from ${LIBRARY} to ${PROJECT_SOURCE_DIR}/bin"
    )
endforeach()
