#!/bin/sh

mount_sd() {
    sd="$1"

    dev="$(udisksctl loop-setup --file "$sd" --no-user-interaction | grep -o '/dev/[^.]*')"
    echo "$dev"
    udisksctl mount -b "$dev" --no-user-interaction | sed 's/.*at //'
}

umount_sd() {
    dev="$1"

    udisksctl unmount -b "$dev" --no-user-interaction &> /dev/null
    udisksctl loop-delete -b "$dev" --no-user-interaction
}

skel="$1"
sd="$2"

tmp="$(mktemp)"

mount_sd "$sd" > "$tmp"
dev="$(head -n1 "$tmp")"
mp="$(tail -n1 "$tmp")"
rm "$tmp"
cp -ra "$skel/." "$mp"
#rsync -a "$skel/" "$mp/"
umount_sd "$dev"
