target remote localhost:1234
add-symbol-file kernel.elf 0x7c00
tui enable
list
focus cmd
