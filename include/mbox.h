#ifndef MBOX_H
#define MBOX_H

#include <stdint.h>
#include <stdbool.h>

#include "mmio.h"

extern volatile uint32_t mbox[36];

enum {
    MBOX_REQUEST = 0 // TODO: Find out what this does, and what other values are valid.
};

/* channels */
enum MBOX_CHANNELS {
    MBOX_CH_POWER   = 0b0000, // Power management
    MBOX_CH_FB      = 0b0001, // Framebuffer (deprecated)
    MBOX_CH_VUART   = 0b0010, // Virtual UART
    MBOX_CH_VCHIQ   = 0b0011, // VCHIQ
    MBOX_CH_LEDS    = 0b0100, // LEDs
    MBOX_CH_BTNS    = 0b0101, // Buttons
    MBOX_CH_TOUCH   = 0b0110, // Touch screen
    MBOX_CH_COUNT   = 0b0111, // ?
    MBOX_CH_PROP_W  = 0b1000, // Property tags (ARM -> GPU)
    MBOX_CH_PROP_R  = 0b1001  // Property tags (ARM <- GPU)
};

/* tags */
enum MBOX_TAGS {
    // Firmware & board info
    MBOX_TAG_GET_FIRMWARE_VER   = 0x00001,
    MBOX_TAG_GET_BOARD_MODEL    = 0x10001,
    MBOX_TAG_GET_BOARD_REVISION = 0x10002,
    MBOX_TAG_GET_MAC_ADDRESS    = 0x10003,
    MBOX_TAG_GET_SERIAL_NUMBER  = 0x10004,

    // Get start addresses and sizes of CPU and GPU memory
    MBOX_TAG_GET_ARM_MEMORY     = 0x10005,
    MBOX_TAG_GET_GPU_MEMORY     = 0x10006,

    // Get all clocks that exist (top down breadth first order)
    MBOX_TAG_GET_CLOCKS         = 0x10007,

    // Power device info: SD card, UART, SPI, etc.
    MBOX_TAG_GET_POWER_STATE    = 0x20001,
    MBOX_TAG_GET_POWER_TIMING   = 0x20002,
    MBOX_TAG_SET_POWER_STATE    = 0x28001,

    // Clocks
    MBOX_TAG_GET_CLK_STATE      = 0x30001,
    MBOX_TAG_SET_CLK_STATE      = 0x38001,
    MBOX_TAG_GET_CLK_RATE       = 0x30002,
    MBOX_TAG_GET_MAX_CLK_RATE   = 0x30004,
    MBOX_TAG_GET_MIN_CLK_RATE   = 0x30007,
    MBOX_TAG_SET_CLK_RATE       = 0x38002,

    // Voltage
    MBOX_TAG_GET_VOLTAGE        = 0x30003,
    MBOX_TAG_GET_MAX_VOLTAGE    = 0x30005,
    MBOX_TAG_GET_MIN_VOLTAGE    = 0x30008,
    MBOX_TAG_SET_VOLTAGE        = 0x38003,

    // Temperature (TODO)
    MBOX_TAG_GET_TEMP           = 0x30006,
    MBOX_TAG_GET_MAX_TEMP       = 0x3000A,

    // TODO
    MBOX_TAG_GET_TURBO          = 0x30009,
    MBOX_TAG_SET_TURBO          = 0x38009,

    // Memory management (TODO)
    MBOX_TAG_ALLOC_MEM          = 0x3000C,
    MBOX_TAG_MEM_LOCK           = 0x3000D,
    MBOX_TAG_MEM_UNLOCK         = 0x3000E,
    MBOX_TAG_MEM_RELEASE        = 0x3000F,

    // Execute code on the GPU (blocking)
    MBOX_TAG_EXEC               = 0x30010,

    // TODO
    MBOX_TAG_GET_DISP_MEMHANDLE = 0x30014,
    MBOX_TAG_GET_EDID_BLOCK     = 0x30020,

    // Frame Buffer (TODO)
    MBOX_TAG_FB_ALLOC           = 0x40001, // Allocate the Frame Buffer
    MBOX_TAG_FB_RELEASE         = 0x48001, // Release the Frame Buffer
    MBOX_TAG_FB_BLANK_SCREEN    = 0x40002, // Blank the screen (TODO: test this)
    MBOX_TAG_FB_GET_DIMS        = 0x40003, // Dimensions (width/height, pixels) of the pysical FB in memory
    MBOX_TAG_FB_TEST_DIMS       = 0x44003,
    MBOX_TAG_FB_SET_DIMS        = 0x48003,
    MBOX_TAG_FB_GET_VIRT_DIMS   = 0x40004, // Dimensions of the FB sent to the display
    MBOX_TAG_FB_TEST_VIRT_DIMS  = 0x44004,
    MBOX_TAG_FB_SET_VIRT_DIMS   = 0x48004,
    MBOX_TAG_FB_GET_DEPTH       = 0x40005, // TODO (does this mean bit-depth?)
    MBOX_TAG_FB_TEST_DEPTH      = 0x44005,
    MBOX_TAG_FB_SET_DEPTH       = 0x48005,
    MBOX_TAG_FB_GET_PIX_ORD     = 0x40006, // RGB/BGR (TODO)
    MBOX_TAG_FB_TEST_PIX_ORD    = 0x44006,
    MBOX_TAG_FB_SET_PIX_ORD     = 0x48006,
    MBOX_TAG_FB_GET_ALPHA_MODE  = 0x40007, // TODO
    MBOX_TAG_FB_TEST_ALPHA_MODE = 0x44007,
    MBOX_TAG_FB_SET_ALPHA_MODE  = 0x48007,
    MBOX_TAG_FB_GET_PITCH       = 0x40008, // TODO
    MBOX_TAG_FB_GET_VOFFSET     = 0x40009, // TODO
    MBOX_TAG_FB_TEST_VOFFSET    = 0x44009,
    MBOX_TAG_FB_SET_VOFFSET     = 0x48009,
    MBOX_TAG_FB_GET_OVERSCAN    = 0x4000A, // TODO
    MBOX_TAG_FB_TEST_OVERSCAN   = 0x4400A,
    MBOX_TAG_FB_SET_OVERSCAN    = 0x4800A,
    MBOX_TAG_FB_GET_PALETTE     = 0x4000B, // TODO
    MBOX_TAG_FB_TEST_PALETTE    = 0x4400B,
    MBOX_TAG_FB_SET_PALETTE     = 0x4800B,

    // Get commandline string (TODO)
    MBOX_TAG_GET_COMMANDLINE    = 0x50001,

    // TODO
    MBOX_TAG_GET_DMA_CHANNELS   = 0x60001,

    // TODO
    MBOX_TAG_SET_CURSOR_INFO    = 0x08010,
    MBOX_TAG_SET_CURSOR_STATE   = 0x08011,

    MBOX_TAG_LAST       = 0 // What does this entry do?
};

/* For mailbox documentation, see
 * https://github.com/raspberrypi/firmware/wiki/Mailboxes
 */

enum MBOX_REGS {
    MBOX_READ_BASE      = MMIO_BASE | 0xB880, // Read by ARM (read-only)
    MBOX_READ           = MBOX_READ_BASE + 0x00,
    MBOX_READ_PEEK      = MBOX_READ_BASE + 0x10,
    MBOX_READ_SENDER    = MBOX_READ_BASE + 0x14,
    MBOX_READ_STATUS    = MBOX_READ_BASE + 0x18,
    MBOX_READ_CONFIG    = MBOX_READ_BASE + 0x1C,

    MBOX_WRITE_BASE     = MMIO_BASE | 0xB8A0, // Read by GPU (write-only)
    MBOX_WRITE          = MBOX_WRITE_BASE + 0x00,
    MBOX_WRITE_PEEK     = MBOX_WRITE_BASE + 0x10,
    MBOX_WRITE_SENDER   = MBOX_WRITE_BASE + 0x14,
    MBOX_WRITE_STATUS   = MBOX_WRITE_BASE + 0x18,
    MBOX_WRITE_CONFIG   = MBOX_WRITE_BASE + 0x1C,
};

enum MBOX_BITS {
    MBOX_RESPONSE   = 0x80000000,
    MBOX_FULL       = 0x80000000,
    MBOX_EMPTY      = 0x40000000
};

uint32_t mbox_call(uint8_t channel);

uint32_t mbox_command(enum MBOX_CHANNELS channel, bool blocking, uint32_t* msg);

uint64_t get_serial_number(uint32_t* err);

#endif
