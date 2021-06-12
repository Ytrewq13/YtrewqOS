#!/bin/sh

# $1 is the source file
# $2 is the output file

# Number of sections used by binary
#SECTIONCOUNT=$((($(du -b "$1" | cut -f1)+511)/512))
# Number of sections requested by CMakeLists.txt
SECTORCOUNT=$((1048576/@SECTOR_SIZE@))
DISKSIZE=$((($(du -b "$1" | cut -f1)+511)/1024/1024))
if [ "$DISKSIZE" -lt 10 ]; then
    DISKSIZE=10
fi

rm -f "$2"
#bximage -q -mode=create -sectsize=@SECTOR_SIZE@ -hd="$DISKSIZE"M "$2" > /dev/null
dd if=/dev/zero of="$2" bs=@SECTOR_SIZE@ count="$SECTORCOUNT" status=none
dd if="$1" of="$2" conv=notrunc status=none
