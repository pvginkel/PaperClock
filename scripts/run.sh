#!/bin/sh

set -e

cd "$(dirname "$0")/../bin"

sudo --preserve-env LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH ./main
