/* fb_pixels.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "graphics/fb_pixels.h"
#include <stdint.h>

#include "framebuf.h"
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

/* Swap two *non-overlapping* rectangles on the frame buffer. Uses malloc().
 */
ERROR_TYPE fb_swap_rectangle(const uint32_t xa, const uint32_t ya, const
        uint32_t xb, const uint32_t yb, const uint32_t w, const uint32_t h)
{
    void *fb = (void*)(uintptr_t)framebuf.base_addr;
    uint32_t depth, pitch, byte_depth;
    uint8_t *buf;
    void *a_ptr, *b_ptr;
    size_t j, buf_sz, offset_a, offset_b;
    framebuf_dims_t display_info;

    fb_dims(FB_ATTR_GET, NULL, &display_info);
    fb_bit_depth(FB_ATTR_GET, NULL, &depth);
    byte_depth = depth >> 3;
    fb_get_pitch(&pitch);

    if (xa > display_info.width || ya > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, xa, ya);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }
    if (xb > display_info.width || yb > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, xb, yb);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }

    if (xa == 0 && xb == 0 && w == display_info.width) {
        // Copy the whole rectangle at once, rows are contiguous
        buf_sz = byte_depth * w * h;
        buf = malloc(buf_sz);

        offset_a = ya * pitch;
        offset_b = yb * pitch;
        a_ptr = fb + offset_a;
        b_ptr = fb + offset_b;
        memcpy(buf, a_ptr, buf_sz);
        memcpy(a_ptr, b_ptr, buf_sz);
        memcpy(b_ptr, buf, buf_sz);

        free(buf);
        return FB_PIX_SUCCESS;
    }

    // Swap rectangles row-by-row (rows are not contiguous in memory)
    buf_sz = byte_depth * w;
    buf = malloc(buf_sz);
    for (j = 0; j < h; j++) {
        offset_a = ((ya+j) * pitch) + (xa * byte_depth);
        offset_b = ((yb+j) * pitch) + (xb * byte_depth);
        a_ptr = fb + offset_a;
        b_ptr = fb + offset_b;
        memcpy(buf, a_ptr, buf_sz);
        memcpy(a_ptr, b_ptr, buf_sz);
        memcpy(b_ptr, buf, buf_sz);
    }
    free(buf);
    return FB_PIX_SUCCESS;
}

/* Copy a rectangle of framebuffer from one location to another
 * *non-overlapping* location. */
ERROR_TYPE fb_cpy_rectangle(const uint32_t x_src, const uint32_t y_src, const
        uint32_t x_dst, const uint32_t y_dst, const uint32_t w,
        const uint32_t h)
{
    void *fb = (void*)(uintptr_t)framebuf.base_addr;
    uint32_t depth, pitch, byte_depth;
    void *src_ptr, *dst_ptr;
    size_t j, offset_src, offset_dst;
    framebuf_dims_t display_info;

    fb_dims(FB_ATTR_GET, NULL, &display_info);
    fb_bit_depth(FB_ATTR_GET, NULL, &depth);
    byte_depth = depth >> 3;
    fb_get_pitch(&pitch);

    if (x_src > display_info.width || y_src > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, x_src, y_src);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }
    if (x_dst > display_info.width || y_dst > display_info.height) {
        uart0_printf("Error! pos too large!\n");
        uart0_printf("Dims: (%d, %d), pos: (%d, %d)\n", display_info.width,
                display_info.height, x_dst, y_dst);
        return FB_PIX_ERR_DIMS_TOO_LARGE;
    }

    // If rows are contiguous, copy all at once
    if (x_src == 0 && x_dst == 0 && w == display_info.width) {
        offset_src = y_src * pitch;
        offset_dst = y_dst * pitch;
        src_ptr = fb + offset_src;
        dst_ptr = fb + offset_dst;
        memcpy(dst_ptr, src_ptr, h*pitch);
        return FB_PIX_SUCCESS;
    }

    // Copy row-by-row, rows are not contiguous
    for (j = 0; j < h; j++) {
        offset_src = ((y_src+j) * pitch) + (x_src * byte_depth);
        offset_dst = ((y_dst+j) * pitch) + (x_dst * byte_depth);
        src_ptr = fb + offset_src;
        dst_ptr = fb + offset_dst;
        memcpy(dst_ptr, src_ptr, w*byte_depth);
    }
    return FB_PIX_SUCCESS;
}

/* Returns the *bit* offset of the pixel at (x, y) */
size_t px_offset(size_t x, size_t y, size_t bit_pitch, uint_fast8_t depth)
{ return y * bit_pitch + x * depth; }
