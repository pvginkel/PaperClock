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

# pytz goes into the build tree rather than the interpreter's site-packages: in the
# espressif/idf image `pip` is the root-owned IDF virtualenv's pip, so a plain
# `pip install` only works when the build runs as root. It does in CI, but not in a
# dev container running as an ordinary user, where it fails with EACCES on
# /opt/esp/python_env/.../site-packages (and --user is no help — the home is
# read-only there). Installing into ${CMAKE_CURRENT_SOURCE_DIR}/../build works for
# both, and build/ is already ignored.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../build/config/tzinfo.inc
    COMMAND pip install -q --target "${CMAKE_CURRENT_SOURCE_DIR}/../build/python-packages" pytz
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/../build/python-packages" python "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/generate-tzinfo.py" "${CMAKE_CURRENT_SOURCE_DIR}/../build/config/tzinfo.inc" "Europe/Amsterdam"
)
