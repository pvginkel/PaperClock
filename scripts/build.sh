#!/bin/sh

cd "$(dirname "$0")/.."

set -e

if [ ! -d build ]; then mkdir build; fi

cd build

#if [ -f CMakeCache.txt ]; then rm CMakeCache.txt; fi

if [ "$1" = "debug" ]; then
  # The version info is rewritten so that it doesn't have a build timestamp
  # in it. This prevents some files from getting rebuilt on every run.

  CURRENT_VERSIONINFO=$(cat ../lib/paho/src/VersionInfo.h.in)
  NEW_VERSIONINFO="
#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#define BUILD_TIMESTAMP \"develop\"
#define CLIENT_VERSION  \"@CLIENT_VERSION@\"

#endif /* VERSIONINFO_H */
"

  if [ "$CURRENT_VERSIONINFO" != "$NEW_VERSIONINFO" ]; then
    echo "$NEW_VERSIONINFO" > ../lib/paho/src/VersionInfo.h.in
  fi

  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make -j
else
  git reset ../lib/paho/src/VersionInfo.h.in >/dev/null 2>&1

  cmake ..
  make
fi
