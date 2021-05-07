ENTRY(_start)
OUTPUT_FORMAT(binary)
STARTUP(kernel.o)
SEARCH_DIR(include)
OUTPUT(kernel.bin)

SECTIONS
{
    . = 0x7c00;
    .text : { *(.text) }
    /* Shouldn't put all files (Stop using wildcard)? */
}
