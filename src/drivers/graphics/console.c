/* console.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-11-01
 */

#include "drivers/graphics/console.h"
#include "error_types.h"
#include "fonts/bizcat_font.h"
#include "drivers/graphics/fb_pixels.h"
#include "io/printf.h"
#include "drivers/hw/uart.h"

console_info_t console_descriptor;

bool console_putc_enabled = false;

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

    console_descriptor.enabled = 1;
    console_putc_enabled = true;

    // TODO: store the text written so we can implement scrolling (and scrollback?)

    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_from_bitmap_with_colors(
    const uint8_t* bitmap, const uint32_t bmp_w, const uint32_t bmp_h,
    const uint32_t x, const uint32_t y, const uint8_t* fg, const uint8_t* bg)
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
//            } else {
//                err = set_pixel(fb_x, fb_y, bg);
//                if (err != FB_PIX_SUCCESS) return err;
            }
        }
    }
    return CONSOLE_SUCCESS;
}

const uint32_t console_get_fg_color()
{ return console_descriptor.fg_color; }

const uint32_t console_get_bg_color()
{ return console_descriptor.bg_color; }

void console_set_fg_color(const uint32_t color)
{ console_descriptor.fg_color = color; }

void console_set_bg_color(const uint32_t color)
{ console_descriptor.bg_color = color; }

// Stores the fg and bg colors into the variables used to remember them during
// a temporary color change.
static void __console_store_reset_colors()
{
    console_descriptor.fg_reset_color = console_descriptor.fg_color;
    console_descriptor.bg_reset_color = console_descriptor.bg_color;
}

void console_set_tmp_fg_color(const uint32_t color)
{
    __console_store_reset_colors();
    console_set_fg_color(color);
}

void console_set_tmp_bg_color(const uint32_t color)
{
    __console_store_reset_colors();
    console_set_bg_color(color);
}

void console_reset_colors()
{
    console_descriptor.fg_color = console_descriptor.fg_reset_color;
    console_descriptor.bg_color = console_descriptor.bg_reset_color;
}

ERROR_TYPE console_write_character_at_pos(const uint8_t c, const uint32_t x,
                                          const uint32_t y)
{
    ERROR_TYPE err;
    const uint8_t* bitmap = bizcat_font_glyphs[c];

    err = console_write_from_bitmap_with_colors(bitmap, char_width, char_height,
            x, y, (uint8_t*)&console_descriptor.fg_color,
            (uint8_t*)&console_descriptor.bg_color);
    if (err != CONSOLE_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_write_character(const uint8_t c)
{
    ERROR_TYPE err;
    err = console_write_character_at_pos(c, console_descriptor.col,
            console_descriptor.row);
    if (err != CONSOLE_SUCCESS) return err;
    console_next_pos();
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_erase_character()
{
    ERROR_TYPE err;
    uint32_t x, y, w, h, color;
    x = console_descriptor.col * char_width;
    y = console_descriptor.row * char_height;
    w = char_width;
    h = char_height;
    color = console_descriptor.bg_color;

    err = set_rectangle(x, y, w, h, color);
    if (err != FB_PIX_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_erase_row(const uint32_t rnum)
{
    ERROR_TYPE err;
    uint32_t x, y, w, h, color;
    x = 0;
    y = rnum * char_height;
    w = console_descriptor.cols * char_width;
    h = char_height;
    color = console_descriptor.bg_color;

    err = set_rectangle(x, y, w, h, color);
    if (err != FB_PIX_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_swap_row(const uint32_t rnum_a, const uint32_t rnum_b)
{
    ERROR_TYPE err;
    uint32_t w, h;
    uint32_t y_a, y_b;
    if (rnum_a == rnum_b) return CONSOLE_ERR_INVALID_ARGS;
    y_a = rnum_a * char_height;
    y_b = rnum_b * char_height;
    w = console_descriptor.cols * char_width;
    h = char_height;
    err = fb_swap_rectangle(0, y_a, 0, y_b, w, h);
    if (err != FB_PIX_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

ERROR_TYPE console_scroll_down(uint32_t rows_count)
{
    ERROR_TYPE err;
    uint32_t y, w, h;
    uint32_t bg_color;
    y = rows_count * char_height;
    w = console_descriptor.cols * char_width;
    h = (console_descriptor.rows - rows_count) * char_height;
    bg_color = console_get_bg_color();
    // Copy everything to the top
    err = fb_cpy_rectangle(0, y, 0, 0, w, h);
    if (err != FB_PIX_SUCCESS) return err;
    // Erase the rows that have been removed from the bottom
    y = (console_descriptor.rows - rows_count) * char_height;
    h = rows_count * char_height;
    err = set_rectangle(0, y, w, h, bg_color);
    if (err != FB_PIX_SUCCESS) return err;
    return CONSOLE_SUCCESS;
}

void console_next_pos()
{
    console_descriptor.col++;
    if (console_descriptor.col == console_descriptor.cols) {
        console_descriptor.col = 0;
        console_descriptor.row++;
        if (console_descriptor.row == console_descriptor.rows) {
            console_descriptor.row--;
            console_scroll_down(1);
        }
    }
}

void console_prev_pos()
{
    // TODO
}

void console_putc(const uint32_t c)
{
    size_t scroll_limit = console_descriptor.rows -
        CONFIG_CONSOLE_MIN_ROWS_BELOW;
    if (!(console_descriptor.enabled && framebuf_enabled))
        return;
    if (c == '\n') {
        console_descriptor.col = 0;
        console_descriptor.row++;
        if (console_descriptor.row == scroll_limit) {
            console_descriptor.row--;
            console_scroll_down(1);
        }
    }
    else {
        console_erase_character();
        console_write_character(c);
    }
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
