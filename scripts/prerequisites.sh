#!/bin/sh

set -e

cd "$(dirname "$0")/.."

ROOT="$(pwd)"

build_lg() {
    echo "Building lg..."

    cd $ROOT/lib/lg
    mkdir -p $ROOT/build/lib/lg

    make -j$(nproc)
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

    ./configure --prefix=$ROOT/build/lib/libbacktrace
    make -j$(nproc)
    make -j$(nproc) install
}

main() {
    mkdir -p $ROOT/build

    build_lg
    build_freetype
    build_libbacktrace
}

main "$@"
