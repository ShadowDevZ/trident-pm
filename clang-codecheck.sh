#!/bin/bash

BUILD_DIR="build"
SOURCE_DIR="src"

clang-format --dry-run --Werror $(find "$SOURCE_DIR" -name "*.cpp" -o -name "*.hpp" -o -name "*.h")
run-clang-tidy -p "$BUILD_DIR"
