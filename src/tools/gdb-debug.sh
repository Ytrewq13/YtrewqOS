#!/bin/sh

# $1 is disk file
# $2 is elf file with debug symbols
# $3 is gdb script file

@QEMU_BIN@ -s -S @QEMU_ARGS_2@ -kernel "@BINNAME@" &
@GDB_BIN@ "@ELFNAME@" -command="@GDB_SCRIPTFILE@"
