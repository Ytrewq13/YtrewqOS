#include <stdarg.h>

#include "graphics/console.h"
#include "io/printf.h"

/* console.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "uart.h"

console_info_t console_descriptor;

ERROR_TYPE console_init()
{
    ERROR_TYPE err;
    framebuf_dims_t fb_dimensions;

    console_descriptor.fg_color = ~0;
    console_descriptor.bg_color = 0;
    console_descriptor.col = 0;
    console_descriptor.row = 0;

    err = fb_dims(FB_ATTR_GET, NULL, &fb_dimensions);
    if (err != FB_SUCCESS) return err;

    console_descriptor.cols = fb_dimensions.width / char_width;
    console_descriptor.rows = fb_dimensions.height / char_height;

    // TODO: store the text written so we can implement scrolling (and scrollback?)

    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_from_bitmap_with_colors(const uint8_t* bitmap,
                                                 uint32_t bmp_w, uint32_t bmp_h,
                                                 uint32_t x, uint32_t y,
                                                 const uint8_t* fg,
                                                 const uint8_t* bg)
{
    ERROR_TYPE err;
    uint32_t fb_x, fb_y;
    uint32_t cur_x, cur_y;
    for (cur_y = 0; cur_y < bmp_h; cur_y++) {
        for (cur_x = 0; cur_x < bmp_w; cur_x++) {
            fb_x = x * bmp_w + cur_x;
            fb_y = y * bmp_h + cur_y;
            if ((bitmap[cur_y*bmp_w/8] >> (8-cur_x)) & 1) {
                err = set_pixel(fb_x, fb_y, fg);
                if (err != FB_PIX_SUCCESS) return err;
            } else {
                err = set_pixel(fb_x, fb_y, bg);
                if (err != FB_PIX_SUCCESS) return err;
            }
        }
    }
    return CONSOLE_SUCCESS;
}

void console_set_fg_color(const uint32_t color)
{ console_descriptor.fg_color = color; }

void console_set_bg_color(const uint32_t color)
{ console_descriptor.bg_color = color; }

ERROR_TYPE console_write_character_at_pos(uint8_t c, uint32_t x, uint32_t y)
{
    ERROR_TYPE err;
    const uint8_t* bitmap = bizcat_font_glyphs[c];

    err = console_write_from_bitmap_with_colors(bitmap, char_width, char_height,
            x, y, (uint8_t*)&console_descriptor.fg_color,
            (uint8_t*)&console_descriptor.bg_color);
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

void console_putc(uint32_t c)
{
    if (c == '\n') {
        console_descriptor.col = 0;
        console_descriptor.row++;
    }
    else console_write_character(c);
}

int console_printf(const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    // Call the generic printf function, passing it the argument list and the
    // console putc function
    count = generic_printf(console_putc, format, ap);
    va_end(ap);  // Tidy up the argument list
    return count;
}
