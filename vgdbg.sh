#! /bin/bash

source build.sh

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         -s \
         build/out/bin/trdbld


