SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x80000;
    __start = .;
    __text_start = .;
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)  /* Everything else. */
    }
    __text_end = .;

    . = ALIGN(2048);
    __exceptions_start = .;
    .exceptions : {
        KEEP(*(.text.exceptions))
        *(.text.exceptions) /* The exception vector table */
    }
    __exceptions_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    __rodata_end = .;

    __data_start = .;
    PROVIDE(_data = .);
    .data : {
        *(.data .data.* .gnu.linkonce.d*)
    }
    __data_end = .;

    __bss_start = .;
    .bss (NOLOAD) : {
        . = ALIGN(16);
        *(.bss .bss.*)
        *(COMMON)
    }
    __bss_end = .;
    __bss_size = __bss_end - __bss_start;
    __end = .;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
