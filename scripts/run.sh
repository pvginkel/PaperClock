#!/bin/sh

set -e

cd "$(dirname "$0")/../bin"

sudo --preserve-env ./main
