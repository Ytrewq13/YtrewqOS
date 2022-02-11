/* kernel/main.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2022-01-01
 */
#include <stdint.h>

// TODO: Reduce header files included here
// - Master header file for all includes?
// - Header file for collections of headers? (e.g. graphics.h, hw.h, etc.)
#include "config.def.h"
#include "errno.h"
#include "fonts/bizcat_font.h"
#include "drivers/framebuf.h"
#include "drivers/graphics/console.h"
#include "drivers/graphics/fb_pixels.h"
#include "drivers/hw/eMMC.h"
#include "drivers/hw/mbox.h"
#include "drivers/hw/mem.h"
#include "drivers/hw/uart.h"
#include "fs/fat.h"
#include "kernel/test.h"
#include "printf.h"
#include "stdlib.h"
#include "string.h"


// TODO: Declare these functions in header files and remove the "extern"s
extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);
extern uint64_t GET_EL();
extern int syscall(long nr, ...);

void delay(size_t time)
{
    size_t i;
    for (i = 0; i < time; i++) __asm volatile("nop");
}

void kernel_main()
{
    // TODO: re-organise variable declarations
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

    uint64_t el = GET_EL();
    printf("Current Exception Level: %ld\n", el);

    uint32_t color = 0xffffff;
    console_init();
    console_set_fg_color(color);

    // Firmware version
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_FIRMWARE_VER, 4, NULL,
                       &firmware_version) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (firmware version)!\n");
    else {
        printf("Firmware version: %x\n", firmware_version);
    }

    // Board model number
    // TODO: this tag doesn't work for some reason - it always puts the same
    // value as the previous call. Is this also a problem on hardware or is the
    // MBOX_TAG_GET_BOARD_MODEL tag broken on QEMU's Pi?
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_BOARD_MODEL, 4, NULL,
                       &board_model) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (board model)!\n");
    else {
        printf("Board model: %d (%#x)\n", board_model, board_model);
    }
    // Board revision number
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_BOARD_REVISION, 4, NULL,
                       &board_revision) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (board revision)!\n");
    else {
        printf("Board revision: %d\n", board_revision);
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
        printf("Serial number: %lx\n", serial_number);
    }
    uart0_puts("\n");
    // CPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_ARM_MEMORY, 8, NULL,
                       &ARM_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (ARM memory)!\n");
    else {
        printf("ARM memory:\nBase address: %p\nSize: %x\n",
                    ARM_mem.base_addr, ARM_mem.size);
    }
    uart0_puts("\n");
    // GPU memory base address and size
    if (mbox_prop_call((void*)mbox, MBOX_TAG_GET_GPU_MEMORY, 8, NULL,
                       &GPU_mem) != MBOX_SUCCESS)
        uart0_puts("Unable to query serial (GPU memory)!\n");
    else {
        printf("GPU memory:\nBase address: %p\nSize: %x\n",
                    GPU_mem.base_addr, GPU_mem.size);
    }
    uart0_puts("\n");

    // Query GPU for Frame Buffer details
    // Physical display width/height
    if (fb_dims(FB_ATTR_GET, NULL, &display_info) != FB_SUCCESS)
        uart0_puts("Unable to query serial (Physical display dimensions)!\n");
    else {
        printf("Display (in memory): %dx%d\n", display_info.width,
                    display_info.height);
    }
    // Virtual display width/height
    if (fb_vdims(FB_ATTR_GET, NULL, &virt_display_info) != FB_SUCCESS)
        uart0_puts("Unable to query serial (Virtual display dimensions)!\n");
    else {
        printf("Display (to monitor): %dx%d\n", virt_display_info.width,
                    virt_display_info.height);
    }

    printf("\n");

    ERROR_TYPE err;
    if ((err = fb_bit_depth(FB_ATTR_GET, &bit_depth, &bit_depth_result)) !=
        FB_SUCCESS) {
        printf("Error - unable to get bit depth!\nErr code: %#lx\n", err);
    } else {
        printf("Bit depth: %d\n", bit_depth_result);
    }

    uint32_t depth = 24;
    uint32_t pitch;

    fb_release();
    // TODO: implement a function to run multiple mailbox calls at once on the framebuffer.
    // - mbox_init_buffer()  <- mallocs a buffer which is stored internally
    // - mbox_add_tag()  <- repeat n times... (reallocs buffer if it grows too large) (checks if the same tag has already been used and updates the args if so)
    // - mbox_send_call()  <- calls mbox_call_raw() using the internal buffer
    // - mbox_get_results()  <- get all results at once? Just return buffer ptr?
    // - mbox_free_buffer()
    printf("Setting physical/virtual display size to 640x480, and bit depth to %d...\n", depth);
    mbox[0]  = 80;
    mbox[1]  = MBOX_REQUEST;
    mbox[2]  = MBOX_TAG_FB_SET_DIMS;      mbox[3]  = 8;   mbox[4]  = 0;
                                          mbox[5]  = 640; mbox[6]  = 480;
    mbox[7]  = MBOX_TAG_FB_SET_VIRT_DIMS; mbox[8]  = 8;   mbox[9]  = 0;
                                          mbox[10] = 640; mbox[11] = 480;
    mbox[12] = MBOX_TAG_FB_SET_DEPTH;     mbox[13] = 4; mbox[14] = 0;
                                  mbox[15] = depth; // (depth) bit pixel depth
    mbox[16] = MBOX_TAG_LAST;
    mbox[17] = 0; mbox[18] = 0; mbox[19] = 0;
    if (!mbox_call_raw((void*)mbox)) {
        printf("Error running combined VC mailbox call.\n");
        // TODO: enter emergency shell (accepts some commands to restart,
        // shutdown, etc.)
        while (1) uart0_putc(uart0_getc());
    }
    if (mbox[15] != depth) {
        printf("Error setting pixel depth to %d, it is instead set to %d\n",
               depth, mbox[15]);
    }
    printf("Requesting a framebuffer...\n");
    if (fb_alloc(16) != FB_SUCCESS) {
        printf("Error requesting a framebuffer.\n");
        // TODO: enter emergency shell (accepts some commands to restart,
        // shutdown, etc.)
        while (1) uart0_putc(uart0_getc());
    }

    // Get the screen dimensions
    fb_dims(FB_ATTR_GET, NULL, &display_info);

    // Draw the background color onto the framebuffer
    console_set_bg_color(BG_COLOR);
    console_set_fg_color(FG_COLOR);
    set_rectangle(0, 0, display_info.width, display_info.height, BG_COLOR);

    printf("Got framebuffer at %p of size %d.\n", framebuf.base_addr, framebuf.size);
    printf("Display dimensions: %dx%d\n", display_info.width, display_info.height);
    fb_get_pitch(&pitch);
    printf("Pitch: %d\n", pitch);

    printf("\n\n");

    // Run all kernel unit tests
    tests_stat_t tests_status;
    test_all(&tests_status);
    printf("\n");
    printf("Test results (%lu total tests)\n", tests_status.total);
    printf("Passed: ");
    console_set_tmp_fg_color(CONFIG_COLOR_TEST_PASS);
    printf("%lu", tests_status.passed);
    console_reset_colors();
    printf(", Warnings: ");
    console_set_tmp_fg_color(CONFIG_COLOR_TEST_WARN);
    printf("%lu", tests_status.warned);
    console_reset_colors();
    printf(", Failed: ");
    console_set_tmp_fg_color(CONFIG_COLOR_TEST_FAIL);
    printf("%lu", tests_status.failed);
    console_reset_colors();
    printf(", Skipped: ");
    console_set_tmp_fg_color(CONFIG_COLOR_TEST_SKIP);
    printf("%lu", tests_status.skipped);
    console_reset_colors();
    printf("\n");

    printf("\n");

    // TODO: refactor all of this into unit tests
    uint8_t c;

    el = GET_EL();
    printf("Current Exception Level: %ld\n", el);
    int i = 442;
    int syscall_ret;
    while (i > 0) {
        printf("Running syscall(%#x)...\n", i);
        syscall_ret = syscall(i);
        printf("System call returned %d\n", syscall_ret);
        i -= 53;
    }
    printf("Running syscall(%#x)...\n", i);
    syscall_ret = syscall(i);
    printf("System call returned %d\n", syscall_ret);
    if (syscall_ret != 0) {
        printf("errno: %d\n", errno);
    }

    printf("\n");

    // Test malloc()
    // TODO: write tests for these components
    size_t malloc_size = 100*sizeof(int);
    void *allocd_mem;
    void *a, *b;
    allocd_mem = malloc(malloc_size);
    printf("malloc(%d) returned %p\n", malloc_size, allocd_mem);
    free(allocd_mem);
    malloc_size = 257;
    allocd_mem = malloc(malloc_size);
    printf("malloc(%d) returned %p\n", malloc_size, allocd_mem);
    free(allocd_mem);
    malloc_size = 4094;
    a = malloc(malloc_size);
    printf("malloc(%d) returned %p\n", malloc_size, a);
    b = malloc(malloc_size);
    printf("malloc(%d) returned %p\n", malloc_size, b);
    free(a);
    free(b);
    printf("Finished freeing all malloc'd memory\n");
    printf("\n");

    /* Notes for reading/writing SD card:
     * - buffer size must be an exact multiple of block size
     *   - block size is available in `dev->block_size`
     * - SD card data persists after writing (even when VM is shut down), so
     *   sd_write() should be called only when there is a sync() syscall
     * - sd_card_init() has hidden malloc() calls which leak memory. Call
     *   sd_card_cleanup() on `dev` to free these blocks.
     */
    // Try to write something to the SD card
    struct block_device *dev;
    err = sd_card_init(&dev);
    if (err != 0) {
        printf("ERR (");
        console_set_tmp_fg_color(CONFIG_COLOR_TEST_FAIL);
        printf("FATAL");
        console_reset_colors();
        printf("): failed to initialise SD card - sd_card_init() returned %d\n",
                err);
        while (1) {}
    }

    // TODO: read filesystem info from SD card
    struct exfat_superblock exfat_info;
    err = exfat_read_boot_block(dev, &exfat_info);
    if (err != 0) {
        printf("ERR (");
        console_set_tmp_fg_color(CONFIG_COLOR_TEST_FAIL);
        printf("FATAL");
        console_reset_colors();
        printf("): Failed to read SD boot block: '%s' (Error %d)\n",
                strerror(errno), errno);
        while (1) {}
    }
    // FIXME: printf width specifiers
    printf("SD card exFAT properties (from boot block)\n");
    printf("Volume serial number    %#x\n", exfat_info.volume_serialnumber);
    printf("FS version              %#hx\n", exfat_info.fs_revision);
    printf("Sector size             %ld\n", exfat_info.sectorsize);
    printf("Cluster size            %#ld\n", exfat_info.sectorsize *
            exfat_info.clustersize);
    printf("Sectors count           %#ld\n", exfat_info.volume_length);
    printf("Clusters Count          %#d\n", exfat_info.cluster_count);
    printf("First sector            %#ld\n", exfat_info.partition_offset);
    printf("FAT first sector        %#d\n", exfat_info.fat_offset);
    printf("Fat sectors count       %#d\n", exfat_info.fat_length);
    printf("First cluster sector    %#d\n", exfat_info.clusterheap_offset);
    printf("Root directory cluster  %d\n", exfat_info.rootdir_start);
    printf("FATs count              %hhd\n", exfat_info.fat_cnt);
    printf("Allocated space         %hhd%%\n", exfat_info.use_percent);
//    u8 active_fat;  // Which fat is active
//    printf("");
//    bool volume_dirty;
//    printf("");
//    bool media_failure;
//    printf("");
//    bool clear_to_zero;
//    printf("");

    // echo everything back
    while (1) {
        c = uart0_getc();
        uart0_putc(c);
        console_putc(c);
    }
    // TODO: reading from keyboard - USB?
}

/*
 * DONE:
 * - Implement arbitrary mailbox calls.
 * - Switch to UART0
 * - Display something to the screen
 * - Display a bitmap to the screen
 * - memset
 * - Graphics driver
 * - Implement any needed Frame Buffer mailbox calls to get something onto the
 *   screen
 * - Filesystem:
 *   - SD card
 * - System calls:
 *   - syscall interface
 * - Memory management:
 *   - malloc, etc.
 *   TODO:
 * - Filesystem:
 *   - FAT filesystem
 *   - Virtual FS
 * - System calls:
 *   - implement basic syscalls
 *   - implement/port more obscure syscalls
 * - Memory management:
 *   - Track memory usage?
 * - USB drivers?
 */
