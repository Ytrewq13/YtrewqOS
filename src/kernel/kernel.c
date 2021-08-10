/* kernel.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-06-18
 */
#include <stdint.h>

#include "framebuf.h"
#include "mbox.h"
#include "mem.h"
#include "uart.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);


void delay(size_t time)
{
    size_t i;
    for (i = 0; i < time; i++) __asm volatile("nop");
}

void kernel_main()
{
    uint32_t firmware_version;
    uint64_t serial_number;
    uint32_t board_model;
    uint32_t board_revision;
    uint64_t MAC_address;

    mem_info_t ARM_mem;
    mem_info_t GPU_mem;

    // Structs for display dimensions
    struct {
        uint32_t width;
        uint32_t height;
    } display_info, virt_display_info;

    // Frame Buffer info struct
    mem_info_t fb_info;

    // Bit depth info
    uint32_t bit_depth, bit_depth_result;


    volatile uint32_t __attribute__((aligned(16))) mbox[36];

    // Set up serial console
    uart0_init();

    uart0_puts("Hello world\n");

    // Firmware version
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_FIRMWARE_VER, 4, NULL,
                       &firmware_version) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (firmware version)!\n");
    else {
        //        uart0_puts("Firmware version: ");
        //        uart0_hex32(firmware_version);
        //        uart0_puts("\n");
        uart0_printf("Firmware version: %x\n", firmware_version);
    }

    // Board model number
    // FIXME: this tag doesn't work for some reason - it always puts the same
    // value as the previous call. Is this also a problem on hardware or is the
    // MBOX_TAG_GET_BOARD_MODEL tag broken on QEMU's Pi?
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_BOARD_MODEL, 4, NULL,
                       &board_model) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (board model)!\n");
    else {
        //        uart0_puts("Board model: ");
        //        uart0_dec(board_model);
        //        uart0_puts(" (0x");
        //        uart0_hex32(board_model);
        //        uart0_puts(")");
        //        uart0_puts("\n");
        uart0_printf("Board model: %d (%#x)\n", board_model, board_model);
    }
    // Board revision number
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_BOARD_REVISION, 4, NULL,
                       &board_revision) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (board revision)!\n");
    else {
        //        uart0_puts("Board revision: ");
        //        uart0_dec(board_revision);
        //        uart0_puts("\n");
        uart0_printf("Board revision: %d\n", board_revision);
    }
    // MAC address
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_MAC_ADDRESS, 6, NULL,
                       &MAC_address) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (MAC address)!\n");
    else {
        uart0_puts("MAC address: ");
        uart0_rnhex(&MAC_address, 6, ":");
        uart0_puts("\n");
    }
    // Board serial number
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_SERIAL_NUMBER, 8, NULL,
                       &serial_number) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (serial number)!\n");
    else {
        //        uart0_puts("Serial number: ");
        //        uart0_hex64(serial_number);
        //        uart0_puts("\n");
        uart0_printf("Serial number: %lx\n", serial_number);
    }
    uart0_puts("\n");
    // CPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_ARM_MEMORY, 8, NULL,
                       &ARM_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (ARM memory)!\n");
    else {
        //        uart0_puts("ARM memory:\n");
        //        uart0_puts("Base address: 0x");
        //        uart0_hex32(ARM_mem.base_addr);
        //        uart0_puts("\nSize: ");
        //        uart0_hex32(ARM_mem.size);
        //        uart0_puts("\n");
        uart0_printf("ARM memory:\nBase address: %p\nSize: %x\n",
                    ARM_mem.base_addr, ARM_mem.size);
    }
    uart0_puts("\n");
    // GPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_GPU_MEMORY, 8, NULL,
                       &GPU_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (GPU memory)!\n");
    else {
        //        uart0_puts("GPU memory:\n");
        //        uart0_puts("Base address: 0x");
        //        uart0_hex32(GPU_mem.base_addr);
        //        uart0_puts("\nSize: ");
        //        uart0_hex32(GPU_mem.size);
        //        uart0_puts("\n");
        uart0_printf("GPU memory:\nBase address: %p\nSize: %x\n",
                    GPU_mem.base_addr, GPU_mem.size);
    }
    uart0_puts("\n");

    // Query GPU for Frame Buffer details
    // Physical display width/height
    if (mbox_prop_call((void*)mbox, MBOX_TAG_FB_GET_DIMS, 8, NULL,
                       &display_info) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (Physical display dimensions)!\n");
    else {
        //        uart0_puts("Display (in memory): ");
        //        uart0_dec(display_info.width);
        //        uart0_puts("x");
        //        uart0_dec(display_info.height);
        //        uart0_puts("\n");
        uart0_printf("Display (in memory): %dx%d\n", display_info.width,
                    display_info.height);
    }
    // Virtual display width/height
    if (mbox_prop_call((void*)mbox, MBOX_TAG_FB_GET_VIRT_DIMS, 8, NULL,
                       &virt_display_info) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (Virtual display dimensions)!\n");
    else {
        //        uart0_puts("Display (to monitor): ");
        //        uart0_dec(virt_display_info.width);
        //        uart0_puts("x");
        //        uart0_dec(virt_display_info.height);
        //        uart0_puts("\n");
        uart0_printf("Display (to monitor): %dx%d\n", virt_display_info.width,
                    virt_display_info.height);
    }
    // TODO: Make error handling less verbose (uart0_panic?)

    uart0_puts("\n");

    if (fb_blank_screen(false) != FB_SUCCESS)
        uart0_puts("Unable to blank screen (off)!\n");
    if (fb_blank_screen(true) != FB_SUCCESS)
        uart0_puts("Unable to blank screen (on)!\n");

    uart0_puts("Attempting to free and then allocate the frame buffer...\n");

    if (fb_release() != FB_SUCCESS)
        uart0_puts("Error releasing Frame Buffer!\n");

    if (fb_alloc(16, &fb_info) != FB_SUCCESS)
        uart0_puts("Error allocating Frame Buffer!\n");
    else {
        //        uart0_puts("Allocated Framebuffer at 0x");
        //        uart0_hex32(fb_info.base_addr);
        //        uart0_puts(" with size ");
        //        uart0_dec(fb_info.size);
        //        uart0_puts(" bytes.\n");
        uart0_printf("Allocated Framebuffer at %p with size %d bytes.\n",
                    fb_info.base_addr, fb_info.size);
    }

    if (fb_blank_screen(false) != FB_SUCCESS)
        uart0_puts("Unable to blank screen (off)!\n");
    if (fb_blank_screen(true) != FB_SUCCESS)
        uart0_puts("Unable to blank screen (on)!\n");

    uart0_puts("Releasing the framebuffer again...\n");

    if (fb_release() != FB_SUCCESS)
        uart0_puts("Error releasing Frame Buffer!\n");

    ERROR_TYPE err;
    if ((err = fb_bit_depth(FB_ATTR_GET, &bit_depth, &bit_depth_result)) !=
        FB_SUCCESS) {
//        uart0_puts("Error - unable to get bit depth!\n");
//        uart0_puts("Err code: 0x");
//        uart0_hex64(err);
//        uart0_puts("\n");
        uart0_printf("Error - unable to get bit depth!\nErr code: %#lx\n", err);
    } else {
//        uart0_puts("Bit depth: ");
//        uart0_dec(bit_depth_result);
//        uart0_puts("\n");
        uart0_printf("Bit depth: %d\n", bit_depth_result);
    }

    const char* test_str = "Hi";

    int n;

    uart0_puts("\nTesting uart0_printf...\n");
    uart0_printf("Hello, world!\n");
    uart0_printf("int string: %d %s\n", 7, test_str);
    uart0_printf("long-uint char short-octal: %lu %c %#ho\n",
                (long unsigned int)67, 'F', (short unsigned int)54);
    uart0_printf("pointer: %p\n", test_str);
    uart0_printf("hex-UPPER percent hex-lower: %#x %% %#X\n", 54687, 54687);
    // Does printf return the correct number every time? Yes
    n = uart0_printf("");
    uart0_printf(" %d\n", n);
    n = uart0_printf("abc\n");
    uart0_printf(" %d\n", n);
    n = uart0_printf("%d", -1);
    uart0_printf(" %d\n", n);
    n = uart0_printf("%u", 3);
    uart0_printf(" %d\n", n);
    n = uart0_printf("%#o", 3);
    uart0_printf(" %d\n", n);
    n = uart0_printf("%x", 4);
    uart0_printf(" %d\n", n);
    n = uart0_printf("%#X", 4);
    uart0_printf(" %d\n", n);
    n = uart0_printf("%c", 'c');
    uart0_printf(" %d\n", n);
    n = uart0_printf("%s", "s");
    uart0_printf(" %d\n", n);
    n = uart0_printf("%p", test_str);
    uart0_printf(" %d\n", n);
    // Does the %n specifier work? Yes
    n = 0;
    uart0_printf("four%n", &n);
    uart0_printf(": %d\n", n);
    uart0_printf("\n");
    // TODO: change all the uart0_puts nonsense to use uart0_printf earlier in
    // this kernel_main function.

    // echo everything back
    while (1) uart0_send(uart0_getc());
}

/*
 * DONE:
 * - Implement arbitrary mailbox calls.
 * TODO:
 * - Implement any needed Frame Buffer mailbox calls to get something onto the
 *   screen
 * - Load important values from the GPU using mailbox calls and store them in
 *   memory
 * - Display something to the screen
 * - Memory management
 * - System calls?
 * - USB drivers?
 */
