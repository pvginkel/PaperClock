FILE(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers -Wno-switch -Wno-deprecated-enum-enum-conversion")
endif()

add_custom_target(
    generate_secrets_header ALL
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/../build/config/tzinfo.inc
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../build/config/tzinfo.inc
    COMMAND pip install -q pytz
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/generate-tzinfo.py" "${CMAKE_CURRENT_SOURCE_DIR}/../build/config/tzinfo.inc" "Europe/Amsterdam"
)
