target remote localhost:1234
add-symbol-file @ELFNAME@ @BOOT_ADDR@
tui enable
list
focus cmd
