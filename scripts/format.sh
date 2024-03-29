#!/bin/sh

cd "$(dirname "$0")"

find ../src -iname '*.c' -iname '*.cpp' -o -iname '*.h' | xargs clang-format -i
