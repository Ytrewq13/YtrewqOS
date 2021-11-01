#ifndef GRAPHICS_CONSOLE_FONT_H
#define GRAPHICS_CONSOLE_FONT_H 1

#include <stdint.h>

typedef void* console_glyph_t;

typedef struct {
    // Pixel width of the character glyphs
    uint_fast32_t width;
    uint_fast32_t height;
    // Padding is placed AFTER the character
    uint_fast32_t x_padding;
    uint_fast32_t y_padding;
    // Pointer to an array of length 127 for ASCII font glyphs
    console_glyph_t *glyphs;
} console_font_t;

// TODO
const extern uint8_t default_console_font_glyphs[128][16];

const extern console_font_t default_console_font;

#endif /* graphics_console_font_h */
