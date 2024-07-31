#!/bin/sh

set -e

cd "$(dirname "$0")/../.."

ROOT="$(pwd)"

build_lg() {
    echo "Building lg..."

    cd $ROOT/lib/lg
    mkdir -p $ROOT/build/lib/lg

    make -j$(nproc) CROSS_PREFIX=$CROSS_COMPILE
    make -j$(nproc) DESTDIR=$ROOT prefix=/build/lib/lg install
}

build_freetype() {
    echo "Building freetype..."

    cd $ROOT/lib/freetype
    mkdir -p $ROOT/build/lib/freetype
    mkdir -p build
    cd build

    cmake -DCMAKE_INSTALL_PREFIX:PATH=$ROOT/build/lib/freetype ..
    make -j$(nproc) install
}

build_libbacktrace() {
    echo "Building libbacktrace..."

    cd $ROOT/lib/libbacktrace
    mkdir -p $ROOT/build/lib/libbacktrace

    ./configure --build "$(gcc -dumpmachine)" --host "$CROSS_TRIPLE" --prefix=$ROOT/build/lib/libbacktrace
    make -j$(nproc)
    make -j$(nproc) install
}

build_bcm2835() {
    echo "Building bcm2835..."

    cd $ROOT/lib/bcm2835
    mkdir -p $ROOT/build/lib/bcm2835

    ./configure --build "$(gcc -dumpmachine)" --host "$CROSS_TRIPLE" --prefix=$ROOT/build/lib/bcm2835
    make -j$(nproc)
    make -j$(nproc) install
}

build_app() {
    echo "Building app..."

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
    build_app "$GPIOLIB"
}

main "$@"
