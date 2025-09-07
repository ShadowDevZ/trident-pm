#!/bin/sh

# ==========================
# Safety check
# ==========================
if [ "$EUID" -eq 0 ]; then
    echo "This script cannot be run as root for safety reasons"
    exit 1
fi

# ==========================
# Functions
# ==========================
clean() {
    echo "Cleaning build directory..."
    rm -rf build/*
}

configure() {
    BUILD_TYPE="$1"
    if [ -z "$BUILD_TYPE" ]; then
        BUILD_TYPE="Debug"   # default if not provided
    fi
    echo "Configuring project with CMake (Build type: $BUILD_TYPE)..."
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
}

build() {
    echo "Building project..."
    cmake --build build || {
        echo "!!! If you are building for the first time, run '$0 regen [debug|release]'"
        exit 1
    }
}

run() {
    echo "---------------"
    ./build/out/bin/trdbld
}

# ==========================
# Main logic
# ==========================
mkdir -p build

TARGET="${1:-run}"   # default target is 'run'
BUILD_TYPE_ARG="$2"

case "$TARGET" in
    regen)
        if [ -n "$BUILD_TYPE_ARG" ]; then
            case "$BUILD_TYPE_ARG" in
                debug|Debug) BUILD_TYPE="Debug" ;;
                release|Release) BUILD_TYPE="Release" ;;
                *)
                    echo "Unknown build type: $BUILD_TYPE_ARG"
                    echo "Valid types: debug | release"
                    exit 1
                    ;;
            esac
        else
            BUILD_TYPE="Debug" # default
        fi
        clean
        configure "$BUILD_TYPE"
        build
        ;;
    clean)
        clean
        ;;
    build)
        build
        ;;
    run)
        build
        run
        ;;
    *)
        echo "Unknown target: $TARGET"
        echo "Usage: $0 [clean|regen [debug|release]|build|run]"
        exit 1
        ;;
esac

echo
