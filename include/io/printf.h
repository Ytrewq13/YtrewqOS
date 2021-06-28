#ifndef IO_PRINTF_H
#define IO_PRINTF_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

// TODO: Move this to a printf-specific header file
enum PRINTF_SPECIFIERS {
    PRINTF_SPECIFIER_INT                = 1 <<  0, // d/i
    PRINTF_SPECIFIER_UNSIGNED_INT       = 1 <<  1, // u
    PRINTF_SPECIFIER_UNSIGNED_OCTAL     = 1 <<  2, // o
    PRINTF_SPECIFIER_HEX_LOWER          = 1 <<  3, // x
    PRINTF_SPECIFIER_HEX_UPPER          = 1 <<  4, // X
    // TODO: f, F, e, E, g, G, a, A
    PRINTF_SPECIFIER_CHAR               = 1 << 13, // c
    PRINTF_SPECIFIER_STRING             = 1 << 14, // s
    PRINTF_SPECIFIER_POINTER            = 1 << 15, // p
    PRINTF_SPECIFIER_GET_NCHARS         = 1 << 16, // n
    PRINTF_SPECIFIER_PERCENT            = 1 << 17, // %
};

enum PRINTF_FLAGS {
    PRINTF_FLAGS_NONE = 0,
    PRINTF_FLAGS_LEFT_JUSTIFY       = 1 << 0, // -
    PRINTF_FLAGS_PLUS_OR_MINUS      = 1 << 1, // +
    PRINTF_FLAGS_SPACE_IF_NO_SIGN   = 1 << 2, // ' '
    PRINTF_FLAGS_PRECEDING_ID       = 1 << 3, // # ('0x' for hex, '0' for octal, etc.)
    PRINTF_FLAGS_LEFTPAD_ZEROES     = 1 << 4, // 0
};

enum PRINTF_LENGTHS {
    PRINTF_LENGTH_DEFAULT,  // no length specifier
    PRINTF_LENGTH_SHORT2,   // hh
    PRINTF_LENGTH_SHORT,    // h
    PRINTF_LENGTH_LONG,     // l
    PRINTF_LENGTH_LONG2,    // ll
    PRINTF_LENGTH_LONGEST,  // j
    PRINTF_LENGTH_SIZE,     // z
    PRINTF_LENGTH_PTRDIFF,  // t
    PRINTF_LENGTH_LONGDBL,  // L
};

enum PRINTF_ITEMS {
    PRINTF_ITEMS_NONE = 0,
    PRINTF_ITEM_FLAGS       = 1 << 0,
    PRINTF_ITEM_WIDTH       = 1 << 1,
    PRINTF_ITEM_PRECISION   = 1 << 2,
    PRINTF_ITEM_LENGTH      = 1 << 3,
};

// Information about a single '%XXXX' printf format specifier
typedef struct {
    enum PRINTF_SPECIFIERS specifier;
    uint32_t flags; // Some combination of `enum PRINTF_FLAGS`
    uint32_t items; // Which items were present
    bool width_as_arg;
    bool precision_as_arg;
    uint32_t width;
    uint32_t precision;
    enum PRINTF_LENGTHS length;
} printf_format_t;


int generic_printf(void putc_fun(uint32_t), const char* format, va_list ap);

// Signed integers
int generic_printf_decimal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Unsigned integers
int generic_printf_unsigned(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Unsigned octal numbers
int generic_printf_octal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Hex numbers (check f->specifier for lower/UPPERcase)
int generic_printf_hex(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Floats
int generic_printf_float(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Chars
int generic_printf_char(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// String
int generic_printf_string(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Pointer
int generic_printf_pointer(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap);
// Get number of chars (pointer in arguments list)
int generic_printf_getcount(void putc_fun(uint32_t), const printf_format_t* f, size_t c, va_list* ap);


#endif /* io_printf_h */
