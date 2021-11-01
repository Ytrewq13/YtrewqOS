#ifndef CONSOLE_H
#define CONSOLE_H 1

#include "fonts/bizcat_font.h"
#include "error_types.h"
#include "fb_pixels.h"

typedef struct {
    uint32_t color;
    uint32_t col;
    uint32_t row;
    uint32_t cols;
    uint32_t rows;
} console_info_t;

extern console_info_t console_descriptor;

typedef enum {
    CONSOLE_ERRORS_BASE = 0x82000000,

    CONSOLE_SUCCESS = CONSOLE_ERRORS_BASE | 0x00,
} CONSOLE_ERR;

ERROR_TYPE console_init();

ERROR_TYPE console_write_from_bitmap_with_color(const uint8_t* bitmap,
                                                uint32_t bmp_w, uint32_t bmp_h,
                                                uint32_t x, uint32_t y,
                                                const uint8_t* color);

ERROR_TYPE console_write_character_at_pos(uint8_t c, uint32_t x, uint32_t y);
ERROR_TYPE console_write_character(uint8_t c);

void console_set_color(const uint32_t color);

#endif /* console_h */
