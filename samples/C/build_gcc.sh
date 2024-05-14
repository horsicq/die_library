#!/bin/bash

FILE=/usr/lib/libdie.so
if test -f "$FILE"; then
    gcc ./simple.c -o simple -I ../../src/include/ -l:libdie.so
else
    echo "libdie.so is not installed or cannot be found in /usr/lib directory"
    echo "please run 'gcc ./simple.c -o simple -I ../../src/include/ -L<path to libdie.so> -l:libdie.so'"
    exit 1
fi
