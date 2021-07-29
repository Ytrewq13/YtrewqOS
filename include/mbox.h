#ifndef MBOX_H
#define MBOX_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "error_types.h"
#include "mmio.h"

extern volatile uint32_t mbox[36];

typedef enum {
    // TODO: Add an offset to all of these values
    MBOX_ERRORS_BASE = 0x40000000,

    MBOX_SUCCESS = MBOX_ERRORS_BASE | 0x00,

    MBOX_ERR_UNKNOWN       = MBOX_ERRORS_BASE | 0x01,
    MBOX_ERR_NULLPTR       = MBOX_ERRORS_BASE | 0x02,
    MBOX_ERR_MSG_TOO_LARGE = MBOX_ERRORS_BASE | 0x03,
    MBOX_ERR_PTR_ALIGN     = MBOX_ERRORS_BASE | 0x04,
} MBOX_ERROR_CODE;

enum MBOX_REQUEST_RESPONSE {
    // Request codes
    MBOX_REQUEST = 0x00000000,
    // All other values reserved.
    // Response codes
    MBOX_RESPONSE_SUCCESS     = 0x80000000,
    MBOX_RESPONSE_PARSE_ERROR = 0x80000001,
    // All other values reserved.
};

/* channels */
enum MBOX_CHANNELS {
    MBOX_CH_POWER  = 0b0000,  // Power management
    MBOX_CH_FB     = 0b0001,  // Framebuffer (DEPRECATED)
    MBOX_CH_VUART  = 0b0010,  // Virtual UART
    MBOX_CH_VCHIQ  = 0b0011,  // VCHIQ
    MBOX_CH_LEDS   = 0b0100,  // LEDs
    MBOX_CH_BTNS   = 0b0101,  // Buttons
    MBOX_CH_TOUCH  = 0b0110,  // Touch screen
    MBOX_CH_COUNT  = 0b0111,  // ?
    MBOX_CH_PROP_W = 0b1000,  // Property tags (ARM -> GPU)
    MBOX_CH_PROP_R = 0b1001   // Property tags (ARM <- GPU) (NOT CURRENTLY USED)
};

/* tags */
enum MBOX_TAG_IDENTIFIERS {
    // Firmware & board info
    MBOX_TAG_GET_FIRMWARE_VER   = 0x00000001,
    MBOX_TAG_GET_BOARD_MODEL    = 0x00010001,
    MBOX_TAG_GET_BOARD_REVISION = 0x00010002,
    MBOX_TAG_GET_MAC_ADDRESS    = 0x00010003,
    MBOX_TAG_GET_SERIAL_NUMBER  = 0x00010004,

    // Get start addresses and sizes of CPU and GPU memory
    MBOX_TAG_GET_ARM_MEMORY = 0x00010005,
    MBOX_TAG_GET_GPU_MEMORY = 0x00010006,

    // Get all clocks that exist (top down breadth first order)
    MBOX_TAG_GET_CLOCKS = 0x00010007,

    // Power device info: SD card, UART, SPI, etc.
    MBOX_TAG_GET_POWER_STATE  = 0x00020001,
    MBOX_TAG_GET_POWER_TIMING = 0x00020002,
    MBOX_TAG_SET_POWER_STATE  = 0x00028001,

    // Clocks
    MBOX_TAG_GET_CLK_STATE    = 0x00030001,
    MBOX_TAG_SET_CLK_STATE    = 0x00038001,
    MBOX_TAG_GET_CLK_RATE     = 0x00030002,
    MBOX_TAG_GET_MAX_CLK_RATE = 0x00030004,
    MBOX_TAG_GET_MIN_CLK_RATE = 0x00030007,
    MBOX_TAG_SET_CLK_RATE     = 0x00038002,

    // Voltage
    MBOX_TAG_GET_VOLTAGE     = 0x00030003,
    MBOX_TAG_GET_MAX_VOLTAGE = 0x00030005,
    MBOX_TAG_GET_MIN_VOLTAGE = 0x00030008,
    MBOX_TAG_SET_VOLTAGE     = 0x00038003,

    // Temperature (TODO)
    MBOX_TAG_GET_TEMP     = 0x00030006,
    MBOX_TAG_GET_MAX_TEMP = 0x0003000A,

    // TODO
    MBOX_TAG_GET_TURBO = 0x00030009,
    MBOX_TAG_SET_TURBO = 0x00038009,

    // Memory management (TODO)
    MBOX_TAG_ALLOC_MEM   = 0x0003000C,
    MBOX_TAG_MEM_LOCK    = 0x0003000D,
    MBOX_TAG_MEM_UNLOCK  = 0x0003000E,
    MBOX_TAG_MEM_RELEASE = 0x0003000F,

    // Execute code on the GPU (blocking)
    MBOX_TAG_EXEC = 0x00030010,

    // TODO
    MBOX_TAG_GET_DISP_MEMHANDLE = 0x00030014,
    MBOX_TAG_GET_EDID_BLOCK     = 0x00030020,

    // Frame Buffer (TODO)
    MBOX_TAG_FB_ALLOC = 0x00040001,
    // Allocate the Frame Buffer
    // Request (4):
    // - uint32: alignment in bytes
    // Response (8):
    // - uint32: base address in bytes
    // - uint32: size in bytes
    MBOX_TAG_FB_RELEASE = 0x00048001,
    // Release the Frame Buffer
    // Request (0)
    // Response (0)
    MBOX_TAG_FB_BLANK_SCREEN = 0x00040002,
    // Blank the screen (TODO: test this)
    // Request (4):
    // - uint32: state
    //   - bit 0: 0=off, 1=on
    // Response (4):
    // - uint32: state
    //   - bit 0: 0=off, 1=on

    // Dimensions of the pysical FB in memory
    MBOX_TAG_FB_GET_DIMS = 0x00040003,
    // Request (0)
    // Response (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    MBOX_TAG_FB_TEST_DIMS = 0x00044003,
    // Test if a set of dimensions is supported without modifying anything.
    // Request (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    // Response (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    // Response is the same as request if the configuration is supported.
    MBOX_TAG_FB_SET_DIMS = 0x00048003,
    // Same as TEST, but does modify the value.

    // Dimensions of the FB sent to the display
    MBOX_TAG_FB_GET_VIRT_DIMS = 0x00040004,
    // Request (0)
    // Response (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    MBOX_TAG_FB_TEST_VIRT_DIMS = 0x00044004,
    // Test if a set of dimensions is supported without modifying anything.
    // Request (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    // Response (8):
    // - uint32: width in pixels
    // - uint32: height in pixels
    // Response is the same as request if the configuration is supported.
    MBOX_TAG_FB_SET_VIRT_DIMS = 0x00048004,
    // Same as TEST, but does modify the value.

    // Bit depth of the FB
    MBOX_TAG_FB_GET_DEPTH = 0x00040005,
    // Request (0)
    // Response (4):
    // - uint32: bits per pixel
    MBOX_TAG_FB_TEST_DEPTH = 0x00044005,
    // Test if a configuration is supported without modifying anything.
    // Request (4):
    // - uint32: bits per pixel
    // Response (4):
    // - uint32: bits per pixel
    // Response is the same as request if the configuration is supported.
    MBOX_TAG_FB_SET_DEPTH = 0x00048005,
    // Same as TEST, but does modify the value.

    // Pixel order of the FB (RGB vs BGR)
    MBOX_TAG_FB_GET_PIX_ORD = 0x00040006,
    // Request (0)
    // Response (4):
    // - uint32: state
    //   - bit 0: 0 = BGR(A), 1 = RGB(A)
    MBOX_TAG_FB_TEST_PIX_ORD = 0x00044006,
    // Test if a configuration is supported without modifying anything.
    // Request (4):
    // - uint32: state
    // Response (4):
    // - uint32: state
    // Response is the same as request if the configuration is supported.
    MBOX_TAG_FB_SET_PIX_ORD = 0x00048006,
    // Same as TEST, but does modify the value.

    // Alpha mode of the FB
    MBOX_TAG_FB_GET_ALPHA_MODE = 0x00040007,  // 0 = Enabled (0 = opaque)
    // Request (0)
    // Response (4):
    // - uint32: state
    //   - 0x0: Alpha channel enabled (0 = fully opaque)
    //   - 0x1: Alpha channel reversed (0 = fully transparent)
    //   - 0x2: Alpha channel ignored
    MBOX_TAG_FB_TEST_ALPHA_MODE = 0x00044007,  // 1 = Reversed (0 = transparent)
    // Test if a configuration is supported without modifying anything.
    // Request (4):
    // - uint32: state
    // Response (4):
    // - uint32: state
    // Response is the same as request if the configuration is supported.
    MBOX_TAG_FB_SET_ALPHA_MODE = 0x00048007,  // 2 = Ignored
    // Same as TEST, but does modify the value.

    MBOX_TAG_FB_GET_PITCH = 0x00040008,  // TODO: (bytes per line?) - meaning?

    // Virtual offset
    // TODO: how is this value used?
    // - Offset into the physical bufer?
    // - Offset onto the display?
    // - Something else?
    MBOX_TAG_FB_GET_VOFFSET  = 0x00040009,  // x-offset (pixels, uint32)
    MBOX_TAG_FB_TEST_VOFFSET = 0x00044009,  // y-offset (pixels, uint32)
    MBOX_TAG_FB_SET_VOFFSET  = 0x00048009,
    // Same as TEST, but does modify the value.

    MBOX_TAG_FB_GET_OVERSCAN =
        0x0004000A,  // Top, bottom, left, right (all uint32)
    MBOX_TAG_FB_TEST_OVERSCAN = 0x0004400A,
    MBOX_TAG_FB_SET_OVERSCAN  = 0x0004800A,
    // Same as TEST, but does modify the value.

    MBOX_TAG_FB_GET_PALETTE = 0x0004000B,  // 256 uint32_t palette values
    MBOX_TAG_FB_TEST_PALETTE =
        0x0004400B,  // offset (first value to set), length(1-256), palette
                     // values (all uint32)
    MBOX_TAG_FB_SET_PALETTE = 0x0004800B,
    // Same as TEST, but does modify the value.

    // Get commandline string (TODO)
    MBOX_TAG_GET_COMMANDLINE = 0x00050001,

    // TODO
    MBOX_TAG_GET_DMA_CHANNELS = 0x00060001,

    // TODO
    MBOX_TAG_SET_CURSOR_INFO  = 0x00008010,
    MBOX_TAG_SET_CURSOR_STATE = 0x00008011,

    // This is used to end the tag part of the mbox request (the final 33 bits
    // must be 0).
    MBOX_TAG_LAST = 0x00000000
};

/* For mailbox documentation, see
 * https://github.com/raspberrypi/firmware/wiki/Mailboxes
 */

enum MBOX_REGS {
    MBOX_READ_BASE   = MMIO_BASE | 0xB880,  // Read by ARM (read-only)
    MBOX_READ        = MBOX_READ_BASE + 0x00,
    MBOX_READ_PEEK   = MBOX_READ_BASE + 0x10,
    MBOX_READ_SENDER = MBOX_READ_BASE + 0x14,
    MBOX_READ_STATUS = MBOX_READ_BASE + 0x18,
    MBOX_READ_CONFIG = MBOX_READ_BASE + 0x1C,

    MBOX_WRITE_BASE   = MMIO_BASE | 0xB8A0,  // Read by GPU (write-only)
    MBOX_WRITE        = MBOX_WRITE_BASE + 0x00,
    MBOX_WRITE_PEEK   = MBOX_WRITE_BASE + 0x10,
    MBOX_WRITE_SENDER = MBOX_WRITE_BASE + 0x14,
    MBOX_WRITE_STATUS = MBOX_WRITE_BASE + 0x18,
    MBOX_WRITE_CONFIG = MBOX_WRITE_BASE + 0x1C,
};

enum MBOX_BITS {
    MBOX_RESPONSE = 0x80000000,
    MBOX_FULL     = 0x80000000,
    MBOX_EMPTY    = 0x40000000
};

// TODO: We are transitioning to these functions instead of mbox_call
ERROR_TYPE mbox_command_start(enum MBOX_CHANNELS chnl, uint32_t* mbox);
ERROR_TYPE mbox_command_wait(enum MBOX_CHANNELS chnl, uint32_t* mbox);

// Generic property call
// Input value: should be copied into the value buffer instead of zeroing the
// buffer. Should be a nullptr or a multiple of 4 bytes long.
ERROR_TYPE mbox_prop_call(void* mbox, enum MBOX_TAG_IDENTIFIERS tag_id,
                          size_t buf_size, void* input, void* result);

#endif
