/* kernel.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-08-10
 */
#include <stdint.h>

#include "framebuf.h"
#include "mbox.h"
#include "mem.h"
#include "uart.h"
#include "graphics/fb_pixels.h"
#include "graphics/console.h"
#include "fonts/bizcat_font.h"

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

    framebuf_dims_t display_info, virt_display_info;

    // Bit depth info
    uint32_t bit_depth, bit_depth_result;


    volatile uint32_t __attribute__((aligned(16))) mbox[36];

    // Set up serial console
    uart0_init();

    uart0_puts("Hello world\n");

//    while (1) uart0_putc(uart0_getc());

    // Firmware version
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_FIRMWARE_VER, 4, NULL,
                       &firmware_version) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (firmware version)!\n");
    else {
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
        uart0_printf("Board model: %d (%#x)\n", board_model, board_model);
    }
    // Board revision number
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_BOARD_REVISION, 4, NULL,
                       &board_revision) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (board revision)!\n");
    else {
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
        uart0_printf("Serial number: %lx\n", serial_number);
    }
    uart0_puts("\n");
    // CPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_ARM_MEMORY, 8, NULL,
                       &ARM_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (ARM memory)!\n");
    else {
        uart0_printf("ARM memory:\nBase address: %p\nSize: %x\n",
                    ARM_mem.base_addr, ARM_mem.size);
    }
    uart0_puts("\n");
    // GPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_GPU_MEMORY, 8, NULL,
                       &GPU_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (GPU memory)!\n");
    else {
        uart0_printf("GPU memory:\nBase address: %p\nSize: %x\n",
                    GPU_mem.base_addr, GPU_mem.size);
    }
    uart0_puts("\n");

    // Query GPU for Frame Buffer details
    // Physical display width/height
    if (fb_dims(FB_ATTR_GET, NULL, &display_info) != FB_SUCCESS)
        uart0_puts("Unable to query serial (Physical display dimensions)!\n");
    else {
        uart0_printf("Display (in memory): %dx%d\n", display_info.width,
                    display_info.height);
    }
    // Virtual display width/height
    if (fb_vdims(FB_ATTR_GET, NULL, &virt_display_info) != FB_SUCCESS)
        uart0_puts("Unable to query serial (Virtual display dimensions)!\n");
    else {
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

    if (fb_alloc(16) != FB_SUCCESS)
        uart0_puts("Error allocating Frame Buffer!\n");
    else {
        uart0_printf("Allocated Framebuffer at %p with size %d bytes.\n",
                    framebuf.base_addr, framebuf.size);
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
        uart0_printf("Error - unable to get bit depth!\nErr code: %#lx\n", err);
    } else {
        uart0_printf("Bit depth: %d\n", bit_depth_result);
    }

    uint32_t depth = 24;
    uint32_t pitch;

    fb_release();
    // TODO: implement a function to run multiple mailbox calls at once on the framebuffer.
    uart0_printf("Setting physical/virtual display size to 640x480, and bit depth to %d...\n", depth);
    mbox[0]  = 80;
    mbox[1]  = MBOX_REQUEST;
    mbox[2]  = MBOX_TAG_FB_SET_DIMS;      mbox[3]  = 8; mbox[4]  = 0; mbox[5]  = 640; mbox[6]  = 480;
    mbox[7]  = MBOX_TAG_FB_SET_VIRT_DIMS; mbox[8]  = 8; mbox[9]  = 0; mbox[10] = 640; mbox[11] = 480;
    mbox[12] = MBOX_TAG_FB_SET_DEPTH;     mbox[13] = 4; mbox[14] = 0; mbox[15] = depth; // (depth) bit pixel depth
    mbox[16] = MBOX_TAG_LAST;
    mbox[17] = 0; mbox[18] = 0; mbox[19] = 0;
    if (!mbox_call_raw((void*)mbox)) {
        uart0_printf("Error running combined VC mailbox call.\n");
        while (1) uart0_putc(uart0_getc());
    }
    if (mbox[15] != depth) {
        uart0_printf("Error setting pixel depth to %d, it is instead set to %d\n", depth, mbox[15]);
    }
    uart0_printf("Requesting a framebuffer...\n");
    if (fb_alloc(16) != FB_SUCCESS) {
        uart0_printf("Error requesting a framebuffer.\n");
        while (1) uart0_putc(uart0_getc());
    }
    uart0_printf("Got framebuffer at %p of size %d.\n", framebuf.base_addr, framebuf.size);
    fb_dims(FB_ATTR_GET, NULL, &display_info);
    uart0_printf("Display dimensions: %dx%d\n", display_info.width, display_info.height);
    fb_get_pitch(&pitch);
    uart0_printf("Pitch: %d\n", pitch);

    uart0_printf("\n\n");
    uint32_t color = 0xff00ff;
    console_init();
    console_set_fg_color(color);
    uint8_t c;
    for (c = 'A'; c <= 'Z'; c++)
        console_write_character(c);
    for (c = 'a'; c <= 'z'; c++)
        console_write_character(c);

    console_printf("\n\n");
    console_set_fg_color(0);
    console_set_bg_color(~0);
    console_printf("Hello there!\n");

    for (int i = 0; i < console_descriptor.rows*console_descriptor.cols; i++)
        console_putc(' ');
    console_descriptor.row = 0;
    console_descriptor.col = 0;
    console_printf("Hello there!\n");

    console_descriptor.bg_color = 0x0000ff;
    while (1) {
        console_descriptor.row = 0;
        console_descriptor.col = 0;
        console_descriptor.bg_color <<= 8;
        if ((console_descriptor.bg_color & 0xffffff) == 0) console_set_bg_color(0xff);
        for (int i = 0; i < console_descriptor.rows*console_descriptor.cols; i++)
            console_putc(' '); // TODO: console_putc() is very slow (implement some kind of bit blit!)
    }

    // echo everything back
    while (1) {
        c = uart0_getc();
        uart0_putc(c);
        console_putc(c);
    }
    // TODO: echo everything back to console (reading from keyboard)
}

/*
 * DONE:
 * - Implement arbitrary mailbox calls.
 * - Switch to UART0
 * - Display something to the screen
 *   TODO:
 * - Display a bitmap to the screen
 * - memset
 * - Graphics driver
 * - Get a basic shell
 *   - First function: print memory contents?
 * - Implement any needed Frame Buffer mailbox calls to get something onto the
 *   screen
 * - Load important values from the GPU using mailbox calls and store them in
 *   memory
 * - Memory management
 * - System calls?
 * - USB drivers?
 */
