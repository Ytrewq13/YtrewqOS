#ifndef GRAPHICS_FB_PIXELS_H
#define GRAPHICS_FB_PIXELS_H 1

#include <stdint.h>

#include "error_types.h"
#include "framebuf.h"
#include "graphics/console_font.h"

typedef enum {
    FB_PIX_ERRORS_BASE = 0x81000000,

    FB_PIX_SUCCESS              = FB_PIX_ERRORS_BASE | 0x00,

    FB_PIX_ERR_NULLPTR          = FB_PIX_ERRORS_BASE | 0x01,
    FB_PIX_ERR_DIMS_TOO_LARGE   = FB_PIX_ERRORS_BASE | 0x02,
} FRAMEBUF_PIX_ERROR;

struct position_t {
    uint32_t x;
    uint32_t y;
};

typedef struct {
    enum pix_order pix_order;
    /* Bits for each channel (RGBA) */
    uint32_t depth;
} pix_info_t;

typedef struct {
    void* ptr; /* Pointer to the start of the buffer */
    uint32_t w; /* Pixel width of the image buffer */
    uint32_t h; /* Pixel height of the image buffer */
    uint32_t pitch; /* Bytes per row */
    pix_info_t pix_info;
} display_buf_t;

typedef struct {
    void* ptr;
    size_t w;
    size_t h;
    size_t x;
    size_t y;
    pix_info_t pix_info;
    /* The bit offset of the left side of the region */
    uint_fast8_t start_bit_offset;
} display_region_t;

typedef struct {
    void* bitmask; // A contiguous block of memory containing a mask.
    size_t w; // The width of the area to be blitted.
    size_t h; // The height of the area to be blitted.
    uint64_t src_color; // The color to write when the mask is 1.
    void* dst; // The destination buffer.
    uint32_t pitch; // The byte-width of the destination buffer's rows.
    uint32_t depth; // The bit depth of the destination buffer
    size_t x; // The x position of the top left corner of the destination.
    size_t y; // The y position of the top left corner of the destination.
} blit_descriptor_t;

ERROR_TYPE set_pixel(uint32_t x, uint32_t y, const uint8_t* color);

#endif /* graphics_fb_pixels_h */
