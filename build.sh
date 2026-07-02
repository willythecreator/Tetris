#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="$(cd "$(dirname "$0")" && pwd)/build"
SOURCE_DIR="$(cd "$(dirname "$0")" && pwd)"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S "$SOURCE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -j "$(nproc)"
