#ifndef BIZCAT_FONT_H
#define BIZCAT_FONT_H 1

#include <stdint.h>

extern const uint8_t bizcat_font_glyphs[256][16];

const extern uint_least16_t char_width;
const extern uint_least16_t char_height;
const extern uint_least16_t char_spacing;
const extern uint_least16_t line_spacing;

#endif
