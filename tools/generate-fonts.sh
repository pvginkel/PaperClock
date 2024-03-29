#!/bin/sh

cd "$(dirname "$0")"

pnpm install
pnpm run generate-fonts
