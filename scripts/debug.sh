#!/bin/sh

set -e

cd "$(dirname "$0")"

./build.sh

cd ../bin

sudo fbset -fb /dev/fb0 -g 1024 600 1024 600 32
sudo gdb -x ../scripts/commands.gdb --quiet
