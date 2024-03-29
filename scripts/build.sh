#!/bin/sh

cd "$(dirname "$0")/.."

set -e

if [ ! -d build ]; then mkdir build; fi

cd build

if [ -f CMakeCache.txt ]; then rm CMakeCache.txt; fi

if grep -q 'Raspbian' /etc/os-release; then
  cmake ..
  make
else
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make -j
fi
