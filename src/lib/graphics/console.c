#include "graphics/console.h"

/* console.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "uart.h"

console_info_t console_descriptor;

ERROR_TYPE console_init()
{
    console_descriptor.color = ~0;
    console_descriptor.col = 0;
    console_descriptor.row = 0;

    console_descriptor.cols = framebuf.dims.width / char_width;
    console_descriptor.rows = framebuf.dims.height / char_height;

    // TODO: store the text written so we can implement scrolling (and scrollback?)

    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_from_bitmap_with_color(const uint8_t* bitmap,
                                                uint32_t bmp_w, uint32_t bmp_h,
                                                uint32_t x, uint32_t y,
                                                const uint8_t* color)
{
    ERROR_TYPE err;
    uint32_t fb_x, fb_y;
    uint32_t cur_x, cur_y;
    for (cur_y = 0; cur_y < bmp_h; cur_y++) {
        for (cur_x = 0; cur_x < bmp_w; cur_x++) {
            if ((bitmap[cur_y*bmp_w/8] >> (8-cur_x)) & 1) {
                fb_x = x * bmp_w + cur_x;
                fb_y = y * bmp_h + cur_y;
                err = set_pixel(fb_x, fb_y, color);
                if (err != FB_PIX_SUCCESS) return err;
            }
        }
    }
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_character_at_pos(uint8_t c, uint32_t x, uint32_t y)
{
    ERROR_TYPE err;
    const uint8_t* bitmap = bizcat_font_glyphs[c];

    err = console_write_from_bitmap_with_color(bitmap, char_width, char_height,
            x, y, (uint8_t*)&console_descriptor.color);
    if (err != CONSOLE_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_character(uint8_t c)
{
    ERROR_TYPE err;
    if (console_descriptor.col == console_descriptor.cols) {
        console_descriptor.col = 0;
        console_descriptor.row++;
    }
    if (console_descriptor.row == console_descriptor.rows)
        console_descriptor.row = 0;
    err = console_write_character_at_pos(c, console_descriptor.col,
            console_descriptor.row);
    if (err != CONSOLE_SUCCESS) return err;
    console_descriptor.col++;
    return CONSOLE_SUCCESS;
}

void console_set_color(const uint32_t color)
{ console_descriptor.color = color; }
