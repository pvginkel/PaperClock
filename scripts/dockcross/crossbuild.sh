#!/bin/sh

set -e

cd "$(dirname "$0")/../.."

ROOT="$(pwd)"

init_submodule() {
    cd $ROOT
    git submodule update --init  --depth 1 "$@"
}

build_lg() {
    if [ -d $ROOT/build/lib/lg ]; then
        return
    fi

    echo "Building lg..."

    init_submodule lib/lg

    cd $ROOT/lib/lg

    make -j$(nproc) CROSS_PREFIX=$CROSS_COMPILE
    make -j$(nproc) DESTDIR=$ROOT prefix=/build/lib/lg install
}

build_freetype() {
    if [ -d $ROOT/build/lib/freetype ]; then
        return
    fi

    echo "Building freetype..."

    init_submodule lib/freetype

    mkdir -p $ROOT/lib/freetype/build
    cd $ROOT/lib/freetype/build

    cmake -DCMAKE_INSTALL_PREFIX:PATH=$ROOT/build/lib/freetype ..
    make -j$(nproc) install
}

build_libbacktrace() {
    if [ -d $ROOT/build/lib/libbacktrace ]; then
        return
    fi

    echo "Building libbacktrace..."

    init_submodule lib/libbacktrace

    cd $ROOT/lib/libbacktrace

    ./configure --build "$(gcc -dumpmachine)" --host "$CROSS_TRIPLE" --prefix=$ROOT/build/lib/libbacktrace
    make -j$(nproc)
    make -j$(nproc) install
}

build_bcm2835() {
    if [ -d $ROOT/build/lib/bcm2835 ]; then
        return
    fi

    echo "Building bcm2835..."

    cd $ROOT/lib/bcm2835

    ./configure --build "$(gcc -dumpmachine)" --host "$CROSS_TRIPLE" --prefix=$ROOT/build/lib/bcm2835
    make -j$(nproc)
    make -j$(nproc) install
}

build_lvgl() {
    if [ -d $ROOT/build/lib/lvgl ]; then
        return
    fi

    echo "Building LVGL"

    init_submodule lib/lvgl

    mkdir -p $ROOT/lib/lvgl/build
    cd $ROOT/lib/lvgl/build

    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=$ROOT/build/lib/lvgl \
        -DLV_CONF_PATH=$ROOT/src/lv_conf.h \
        -DCMAKE_C_FLAGS="-I $ROOT/build/lib/freetype/include/freetype2" \
        ..
    make -j$(nproc) install
}

build_app() {
    echo "Building app..."

    init_submodule lib/cJSON lib/paho

    cd $ROOT/build
    cmake -DCMAKE_BUILD_TYPE=Debug -DGPIOLIB=$1 ..
    make -j$(nproc)
}

main() {
    mkdir -p $ROOT/build

    if [ "$1" = "lg" ]; then
        build_lg
        GPIOLIB=lg
    elif [ "$1" = "bcm" ]; then
        build_bcm2835
        GPIOLIB=bcm
    else
        echo "Specify lg or bcm at the command line"
        exit 1
    fi

    build_freetype
    build_libbacktrace
    build_lvgl
    build_app "$GPIOLIB"
}

main "$@"
