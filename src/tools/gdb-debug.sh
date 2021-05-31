#!/bin/sh

# $1 is disk file
# $2 is elf file with debug symbols
# $3 is gdb script file

qemu-system-x86_64 -s -S -drive format=raw,file="$1" &
gdb "$2" -command="$3"
