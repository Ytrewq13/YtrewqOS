#ifndef CONSOLE_H
#define CONSOLE_H 1

#include <stdarg.h>
#include <stdbool.h>

#include "config.def.h"
#include "error_types.h"
#include "fonts/bizcat_font.h"
#include "fb_pixels.h"

#define HAVE_CONSOLE_PUTC 1
extern bool console_putc_enabled;

typedef struct {
    uint32_t fg_color;
    uint32_t bg_color;
    uint32_t fg_reset_color;
    uint32_t bg_reset_color;
    uint32_t col;
    uint32_t row;
    uint32_t cols;
    uint32_t rows;
    uint8_t enabled;
} console_info_t;

extern console_info_t console_descriptor;

typedef enum {
    CONSOLE_ERRORS_BASE = 0x82000000,

    CONSOLE_SUCCESS = CONSOLE_ERRORS_BASE | 0x00,
    CONSOLE_ERR_ARG_TOO_LARGE = CONSOLE_ERRORS_BASE | 0x01,
    CONSOLE_ERR_INVALID_ARGS = CONSOLE_ERRORS_BASE | 0x02,
} CONSOLE_ERR;

ERROR_TYPE console_init();

ERROR_TYPE console_write_from_bitmap_with_colors(
    const uint8_t* bitmap, const uint32_t bmp_w, const uint32_t bmp_h,
    const uint32_t x, const uint32_t y, const uint8_t* fg, const uint8_t* bg);

const uint32_t console_get_fg_color();
const uint32_t console_get_bg_color();
void console_set_fg_color(const uint32_t color);
void console_set_bg_color(const uint32_t color);

void console_set_tmp_fg_color(const uint32_t color);
void console_set_tmp_bg_color(const uint32_t color);
void console_reset_colors();

ERROR_TYPE console_write_character_at_pos(const uint8_t c, const uint32_t x,
                                          const uint32_t y);
ERROR_TYPE console_write_character(const uint8_t c);
ERROR_TYPE console_erase_character();
ERROR_TYPE console_erase_row(const uint32_t rnum);
ERROR_TYPE console_swap_row(const uint32_t rnum_a, const uint32_t rnum_b);
ERROR_TYPE console_scroll_down();

void console_next_pos();
void console_prev_pos();

void console_putc(const uint32_t c);
int console_printf(const char* format, ...);

#endif /* console_h */
