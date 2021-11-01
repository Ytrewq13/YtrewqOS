#include "graphics/fb_pixels.h"

/* fb_pixels.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "uart.h"


ERROR_TYPE set_pixel(uint32_t x, uint32_t y, const uint8_t* color)
{
    uint8_t* fb = framebuf.mem.base_addr;
    uint32_t depth, pitch, byte_depth;
    size_t i, offset;
    framebuf_dims_t display_info = framebuf.dims;

    if (color == NULL) return FB_PIX_ERR_NULLPTR;

//    fb_dims(FB_ATTR_GET, NULL, &display_info);
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

/* Returns the *bit* offset of the pixel at (x, y) */
size_t px_offset(size_t x, size_t y, size_t bit_pitch, uint_fast8_t depth)
{ return y * bit_pitch + x * depth; }
