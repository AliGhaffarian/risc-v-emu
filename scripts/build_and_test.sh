#!/bin/sh

set -e

mkdir build || true
make -C build clean
cmake -DEMU_BUILD_TESTING=On -D EMU_BUILD_CHECKS=On -S . -B build
make -C build
make -C build test ARGS="--output-on-failure"
