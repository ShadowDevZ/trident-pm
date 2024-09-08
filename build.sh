#! /bin/sh

if [ "$EUID" -eq 0 ]; then
  echo "This script cannot be run as root for safety reasons"; exit 1
fi


rm -rf build/*

if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
    exit 0
fi


#in case it doesnt exist cuz rm force wont trigger error
mkdir build

cmake -S . -B build 
make -C build
echo
echo
./build/out/bin/trdbld
