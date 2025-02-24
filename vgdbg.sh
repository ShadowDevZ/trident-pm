#! /bin/bash

source build.sh

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --log-file=valgrind-out.log -s \
         build/out/bin/trdbld 2>&1 /dev/null

cat valgrind-out.log
