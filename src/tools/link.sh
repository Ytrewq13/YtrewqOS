#!/bin/sh

#CMakeFiles/boot.o.dir/src/boot.s.o CMakeFiles/kernel.o.dir/src/kernel.c.o
#/usr/bin/cc -T/home/sam/Documents/programming/OS-playground/YtrewqOS/src/tools/linker.ld CMakeFiles/boot.o.dir/src/boot.s.o CMakeFiles/kernel.o.dir/src/kernel.c.o -o kernel.bin

shift 3
ld "$@"
