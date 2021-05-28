/*ENTRY(_start)*/
/*OUTPUT_FORMAT(binary)*/
/*OUTPUT(kernel.bin)*/

SECTIONS
{
    /DISCARD/ : {
        *(.note.gnu.property)
    }
    .text 0x7c00 : {
        *boot(.text)  /* The 16-bit bootloader goes first. */
        *boot32(.text)  /* 32-bit mode bootloader. */
        *(.text)  /* Everything else. */
    }
    .data : {
        *boot(.data)
        *boot32(.data)
        *(.data)
    }
    .bss : {
        *(.bss)
        *(COMMON)
    }
}
