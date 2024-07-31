#!/bin/sh

set -e

IMAGE=dockcross/linux-arm64

cd "$(dirname "$0")"

LAST_ID=$(docker ps -q --filter ancestor=$IMAGE)

if [ ! -z $LAST_ID ]; then
    docker stop $LAST_ID
fi

cd ../..

docker run -it --rm \
    -v $(pwd):/work \
    -e BUILDER_UID=$(id -u) \
    -e BUILDER_GID=$(id -g) \
    -e BUILDER_USER="$(id -un)" \
    -e BUILDER_GROUP="$(id -gn)" \
    $IMAGE \
    scripts/dockcross/crossbuild.sh \
    "$@"
