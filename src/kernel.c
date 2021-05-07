#include <stdio.h>

/* kernel.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-05-07
 */

/* TODO:
 * - Some king of basic program (maybe call a function from the bootloader?)
 * - Load this binary using the bootloader
 * - Bootloader jump into this kernel
 */

int main(int argc, char **argv)
{
    printf("Hello from kernel.c!\n");
    return 0;
}
