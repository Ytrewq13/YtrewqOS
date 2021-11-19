#include "graphics/fb_pixels.h"

/* fb_pixels.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "hw/uart.h"
#include "string.h"


ERROR_TYPE set_pixel(const uint32_t x, const uint32_t y, const uint8_t* color)
{
    uint8_t* fb = (uint8_t*)(uint64_t)framebuf.base_addr;
    uint32_t depth, pitch, byte_depth;
    size_t i, offset;
    framebuf_dims_t display_info;

    if (color == NULL) return FB_PIX_ERR_NULLPTR;

    fb_dims(FB_ATTR_GET, NULL, &display_info);
    fb_bit_depth(FB_ATTR_GET, NULL, &depth);
    byte_depth = depth >> 3; /* depth/8 */
    fb_get_pitch(&pitch);

    if (x > display_info.width || y > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, x, y);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }

    offset = (y * pitch) + (x * byte_depth);
    for (i = 0; i < byte_depth; i++) {
        fb[offset++] = color[i];
    }
    return FB_PIX_SUCCESS;
}

// TODO: decide if color can always fit in a uint32_t
ERROR_TYPE set_rectangle(const uint32_t x, const uint32_t y,
                         const uint32_t width, const uint32_t height,
                         const uint32_t color)
{
    void* fb = (void*)(uint64_t)framebuf.base_addr;
    uint32_t depth, pitch, byte_depth;
    size_t i, j, offset;
    framebuf_dims_t display_info;

    fb_dims(FB_ATTR_GET, NULL, &display_info);
    fb_bit_depth(FB_ATTR_GET, NULL, &depth);
    byte_depth = depth >> 3; /* depth/8 */
    fb_get_pitch(&pitch);

    if (x > display_info.width || y > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, x, y);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }

    for (j = 0; j < height; j++) {
        offset = ((y+j) * pitch) + (x * byte_depth);
        if (byte_depth == 1)
            memset(fb+offset, color & 0xff, width);
        else
            for (i = 0; i < width; i++) {
                offset += byte_depth;
                memcpy(fb+offset, &color, byte_depth);
            }
    }
    return FB_PIX_SUCCESS;
}

/* Returns the *bit* offset of the pixel at (x, y) */
size_t px_offset(size_t x, size_t y, size_t bit_pitch, uint_fast8_t depth)
{ return y * bit_pitch + x * depth; }
