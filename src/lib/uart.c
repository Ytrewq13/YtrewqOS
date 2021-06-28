/* uart.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-06-16
 */

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "gpio.h"
#include "uart.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

/* The following is taken from example code found at
 * https://github.com/bztsrc/raspi3-tutorial/blob/master/03_uart1/uart.c
 * Modified slightly to use our enums instead of '#define's and the GET32()
 * and PUT32() functions defined in our boot.S file.
 */

void uart_init() {
    register uint32_t r;

    /* initialize UART */
    PUT32(AUX_ENABLE, GET32(AUX_ENABLE)|1); // Enable UART1, AUX mini uart
    PUT32(AUX_MU_CNTL, 0);
    PUT32(AUX_MU_LCR, 3);       // 8 bits
    PUT32(AUX_MU_MCR, 0);
    PUT32(AUX_MU_IER, 0);
    PUT32(AUX_MU_IIR, 0xC6);    // Disable interrupts
    PUT32(AUX_MU_BAUD, 270);    // 115200 baud
    /* map UART1 to GPIO pins */
    r = GET32(GPFSEL1);
    r &= ~((7<<12)|(7<<15));    // GPIO14, GPIO15
    r |= (2<<12)|(2<<15);       // alt5
    PUT32(GPFSEL1, r);
    PUT32(GPPUD, 0);            // Enable pins 14 and 15
    r = 150; while(r--) __asm volatile("nop"); // Delay 150
    PUT32(GPPUDCLK0, (1<<14)|(1<<15));
    r = 150; while(r--) __asm volatile("nop"); // Delay 150
    PUT32(GPPUDCLK0, 0);        // flush GPIO setup
    PUT32(AUX_MU_CNTL, 3);      // eable Tx, Rx
}

void uart_send(uint32_t c) {
    // Wait until we can send
    while (!(GET32(AUX_MU_LSR) & 0x20)) __asm volatile("nop");
    // Write the character to the buffer
    PUT32(AUX_MU_IO, c);
}

// TODO: uart_putc - a wrapper around uart_send that includes the check for '\n'
void uart_putc(uint32_t c) {
    if (c == '\n') uart_send('\r');
    uart_send(c);
}

char uart_getc() {
    char r;
    // Wait until something is in the buffer
    while(!(GET32(AUX_MU_LSR) & 0x01)) __asm volatile("nop");
    // Read it and return
    r = GET32(AUX_MU_IO);
    // Convert carriage return to newline
    return (r == '\r' ? '\n' : r);
}

void uart_puts(char* s) {
    if (!s) return;
    while (*s) {
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

// Put a wide character string
void uart_putls(wchar_t* s) {
    // TODO
}

// Print a number as decimal on the UART
void uart_dec(uint64_t d) {
    // Put previous digits
    if (d > 9) uart_dec(d/10);
    // Put the final digit
    uart_send('0' + (d % 10));
}

void uart_oct(uintmax_t u) {
    // TODO
}

void uart_hex4(uint8_t d) {
    register uint32_t n = d & 0xF;
    n += (n > 9) ? ('A' - 10) : '0';
    uart_send(n);
}

void uart_hex8(uint8_t d) {
    uart_hex4(d >> 4);
    uart_hex4(d);
}

void uart_hex16(uint16_t d) {
    uart_hex8(d >> 8);
    uart_hex8(d);
}

void uart_hex32(uint32_t d) {
    uart_hex16(d >> 16);
    uart_hex16(d);
}

void uart_hex64(uint64_t d) {
    uart_hex32(d >> 32);
    uart_hex32(d);
}

void uart_nhex(void *src, size_t n, const char *sep) {
    size_t i;
    for (i = 0; i < n; i++) {
        uart_hex8(*((uint8_t*)src+i));
        if (i != n-1) uart_puts((char*)sep);
    }
}

void uart_rnhex(void *src, size_t n, const char *sep) {
    size_t i;
    for (i = n; i > 0; i--) {
        uart_hex8(*((uint8_t*)src+i-1));
        if (i != 1) uart_puts((char*)sep);
    }
}

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

bool str_contains(const char* s, char c) {
    while (*s != 0) {
        if (*s++ == c)
            return true;
    }
    return false;
}

bool is_digit(char c) {return str_contains("0123456789", c);}

// TODO: move printf helpers & generic printf functions to a header file
// (e.g. "printf.h" or "io/printf.h")
const char* printf_parse_flags(const char* s, printf_format_t* f) {
    // FLAGS (-,+, ,#,0)
    if (str_contains("-+ #0", *s)) {
        f->items |= PRINTF_ITEM_FLAGS;
        while (str_contains("-+ #0", *s)) {
            switch (*s) {
                case '-': f->flags |= PRINTF_FLAGS_LEFT_JUSTIFY; break;
                case '+': f->flags |= PRINTF_FLAGS_PLUS_OR_MINUS; break;
                case ' ': f->flags |= PRINTF_FLAGS_SPACE_IF_NO_SIGN; break;
                case '#': f->flags |= PRINTF_FLAGS_PRECEDING_ID; break;
                case '0': f->flags |= PRINTF_FLAGS_LEFTPAD_ZEROES; break;
                // TODO: '0' should be ignored when '-' is present
                // TODO: Find out about other flags' behaviour
            }
            s++;
        }
    }
    return s;
}

const char* printf_parse_width(const char* s, printf_format_t* f) {
    // WIDTH (*,[number])
    if (!(*s == '*' || is_digit(*s))) return s;
    f->items |= PRINTF_ITEM_WIDTH;
    if (*s == '*') {
        f->width_as_arg = true;
        return s+1;
    }
    while (is_digit(*s)) {
        f->width = 10 * f->width + *(s++) - '0';
    }
    return s;
}

const char* printf_parse_precision(const char* s, printf_format_t* f) {
    // PRECISION (.*,.[number])
    if (*s == '.') {
        f->items |= PRINTF_ITEM_PRECISION;
        s++;
        while (*s == '*' || is_digit(*s)) {
            if (*s == '*') f->precision_as_arg = true;
            else f->precision = 10 * f->precision + *s - '0';
            s++;
        }
    }
    return s;
}

const char* printf_parse_length(const char* s, printf_format_t* f) {
    // LENGTH (hh,h,l,ll,j,z,t,L)
    if (str_contains("hljztL", *s)) {
        f->items |= PRINTF_ITEM_LENGTH;
        switch (*s) {
            case 'h':
                if (*(s+1) == 'h') {
                    f->length = PRINTF_LENGTH_SHORT2; s++;
                }
                else f->length = PRINTF_LENGTH_SHORT;
                break;
            case 'l':
                if (*(s+1) == 'l') {
                    f->length = PRINTF_LENGTH_LONG2;
                    s++;
                }
                else f->length = PRINTF_LENGTH_LONG;
                break;
            case 'j': f->length = PRINTF_LENGTH_LONGEST; break;
            case 'z': f->length = PRINTF_LENGTH_SIZE; break;
            case 't': f->length = PRINTF_LENGTH_PTRDIFF; break;
            case 'L': f->length = PRINTF_LENGTH_LONGDBL; break;
        }
        s++;
    }
    return s;
}

const char* printf_parse_specifier(const char* s, printf_format_t* f) {
    // SPECIFIER (%,d,i,u,o,x,X,f,F,e,E,g,G,a,A,c,s,p,n)
    if (str_contains("diuoxXfFeEgGaAcspn%", *s)) {
        switch (*s) {
            case 'd':
            case 'i': f->specifier = PRINTF_SPECIFIER_INT; break;
            case 'u': f->specifier = PRINTF_SPECIFIER_UNSIGNED_INT; break;
            case 'o': f->specifier = PRINTF_SPECIFIER_UNSIGNED_OCTAL; break;
            case 'x': f->specifier = PRINTF_SPECIFIER_HEX_LOWER; break;
            case 'X': f->specifier = PRINTF_SPECIFIER_HEX_UPPER; break;
            case 'f': // FIXME: implement these specifiers
            case 'F': // FIXME: implement these specifiers
            case 'e': // FIXME: implement these specifiers
            case 'E': // FIXME: implement these specifiers
            case 'g': // FIXME: implement these specifiers
            case 'G': // FIXME: implement these specifiers
            case 'a': // FIXME: implement these specifiers
            case 'A': f->specifier = PRINTF_SPECIFIER_INT; break;
            case 'c': f->specifier = PRINTF_SPECIFIER_CHAR; break;
            case 's': f->specifier = PRINTF_SPECIFIER_STRING; break;
            case 'p': f->specifier = PRINTF_SPECIFIER_POINTER; break;
            case 'n': f->specifier = PRINTF_SPECIFIER_GET_NCHARS; break;
            case '%': f->specifier = PRINTF_SPECIFIER_PERCENT; break;
        }
        s++;
    }
    return s;
}

/*
 * Decode the format of a printf format string.
 * %[flags][width][.precision][length]specifier
 * s should point to the character AFTER the percent sign ('%')
 * returns a pointer to the next character after this format specifier
 */
const char* printf_parse_format(const char* s, printf_format_t* f) {
    f->specifier = PRINTF_SPECIFIER_INT;
    f->flags = PRINTF_FLAGS_NONE;
    f->items = PRINTF_ITEMS_NONE;
    f->width = 0;
    f->precision = 0;
    f->width_as_arg = false;
    f->precision_as_arg = false;
    f->length = PRINTF_LENGTH_DEFAULT;

    if (s == NULL)
        return NULL;

    // FLAGS (-,+, ,#,0)
    s = printf_parse_flags(s, f);
    // WIDTH (*,[number])
    s = printf_parse_width(s, f);
    // PRECISION (.*,.[number])
    s = printf_parse_precision(s, f);
    // LENGTH (hh,h,l,ll,j,z,t,L)
    s = printf_parse_length(s, f);
    // SPECIFIER (%,d,i,u,o,x,X,f,F,e,E,g,G,a,A,c,s,p,n)
    s = printf_parse_specifier(s, f);
    return s;
}

/*
 * Print formatted.
 * TODO:
 * - unsigned/signed qualifiers
 * - integer length qualifier (e.g. %ld) (LENGTH: hh,h,l,ll,j,z,t,L)
 * - print width (e.g. %5d) (WIDTH: [number],*)
 * - pad with 0 (i.e. %04d) (FLAGS: -,+, ,#,0)
 * - Format specifiers:
 *   - pointers (%p)
 *   - hex (%x)
 *   - char (%c)
 *   - (%n) the corresponding argument must be a pointer to a signed int. The
 *   number of characters printed so far is stored in the pointed location.
 *   - others (TODO)
 * - Floats?
 * - Should return the total number of characters written?
 */
void uart_printf(char *s, ...) {
    va_list ap;
    char* str;
    int d;
    unsigned int ud;

    va_start(ap, s);

    while (*s != 0) {
        if (*s != '%') {
            if (*s == '\n')
                uart_send('\r');
            uart_send(*s);
            s++;
            continue;
        }
        s++;
        // TODO: reformat this (add a function that takes the string ptr and returns a struct with all the info)
        switch (*s) {
            case '%':
                uart_send(*s);
                break;
            case 'd':
                d = va_arg(ap, int);
                uart_dec(d);
                break;
            case 's':
                str = va_arg(ap, char*);
                uart_puts(str);
                break;
            case 'x':
                ud = va_arg(ap, unsigned int);
                uart_hex32(ud);
                break;
        }
        s++;
    }
    va_end(ap);
}

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


int generic_printf(void putc_fun(uint32_t), const char* format, va_list ap) {
    printf_format_t f;

    size_t count = 0; // Count how many characters have been printed

    while (*format) {
        if (*format != '%') {
            putc_fun(*format);
            format++;
            count++;
            continue;
        }
        // Call the parse function on the next character.
        format = printf_parse_format(format+1, &f);

//        uart_puts("\n");
//        uart_puts("PRINTF FORMAT\n");
//        uart_puts("=============\n");
//        uart_puts("Length: "); uart_dec(f.length); uart_putc('\n');

        switch (f.specifier) {
            case PRINTF_SPECIFIER_PERCENT:
//                uart_puts("Specifier: Percent\n");
                putc_fun('%'); count++; break;
            case PRINTF_SPECIFIER_INT:
//                uart_puts("Specifier: int\n");
                count += generic_printf_decimal(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_UNSIGNED_INT:
//                uart_puts("Specifier: uint\n");
                count += generic_printf_unsigned(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_UNSIGNED_OCTAL:
//                uart_puts("Specifier: octal\n");
                count += generic_printf_octal(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_HEX_LOWER:
            case PRINTF_SPECIFIER_HEX_UPPER:
//                uart_puts("Specifier: hex ("); uart_dec(f.specifier); uart_putc('\n');
                count += generic_printf_hex(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_CHAR:
//                uart_puts("Specifier: char\n");
                count += generic_printf_char(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_STRING:
//                uart_puts("Specifier: string\n");
                count += generic_printf_string(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_POINTER:
//                uart_puts("Specifier: pointer\n");
                count += generic_printf_pointer(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_GET_NCHARS:
//                uart_puts("Specifier: get number of characters\n");
                generic_printf_getcount(putc_fun, &f, count, &ap); break;
        }
    }

    return count; // FIXME: count every character sent
}

// TODO: rename this function after testing it.
int uart_printf2(const char* format, ...) {
    int count;
    va_list ap;

    va_start(ap, format);
    // Call the generic printf function, passing it the argument list and the
    // UART putc function
    count = generic_printf(uart_putc, format, ap);
    va_end(ap); // Tidy up the argument list
    return count;
}



// Signed integers
int generic_printf_decimal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    size_t i, j, n;

    intmax_t d;

    count = 0;
    switch (f->length) {
        case PRINTF_LENGTH_SHORT2: d = va_arg(*ap, int) & 0xFF; break;
        case PRINTF_LENGTH_SHORT: d = va_arg(*ap, int) & 0xFFFF; break;
        case PRINTF_LENGTH_LONG: d = va_arg(*ap, long int); break;
        case PRINTF_LENGTH_LONG2: d = va_arg(*ap, long long int); break;
        case PRINTF_LENGTH_LONGEST: d = va_arg(*ap, intmax_t); break;
        case PRINTF_LENGTH_SIZE: d = va_arg(*ap, size_t); break;
        case PRINTF_LENGTH_PTRDIFF: d = va_arg(*ap, ptrdiff_t); break;
        default: d = va_arg(*ap, int); break;
    }

    /* Get the number of characters needed to print the number (in `i`) */
    n = (d < 0) ? -d : d;  // n := abs(d)
    i = 0;
    while (n > 0) {
        n /= 10;
        i++;
    }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)

    // Put '-' for numbers < 0
    if (d < 0) {
        putc_fun('-'); count++;
    } else if (f->flags & PRINTF_FLAGS_PLUS_OR_MINUS) {
        putc_fun('+'); count++; // + flag
    } else if (f->flags & PRINTF_FLAGS_SPACE_IF_NO_SIGN) {
        putc_fun(' '); count++; // ' ' flag
    }

    if (d == 0) { putc_fun('0'); count++; }

    // TODO: there may be a better way to do this (this is O(n^2))
    while (i --> 0) {
        n = (d < 0) ? -d : d;       // n := abs(d)
        for (j = 0; j < i; j++) {
            n /= 10;
        }
        putc_fun('0' + (n % 10));
        count++;
    }
    return count;
}

// Unsigned integers
int generic_printf_unsigned(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    size_t i, j, n;

    uintmax_t u;

    count = 0;
    switch (f->length) {
        case PRINTF_LENGTH_SHORT2: u = va_arg(*ap, unsigned int) & 0xFF; break;
        case PRINTF_LENGTH_SHORT: u = va_arg(*ap, unsigned int) & 0xFFFF; break;
        case PRINTF_LENGTH_LONG: u = va_arg(*ap, unsigned long int); break;
        case PRINTF_LENGTH_LONG2: u = va_arg(*ap, unsigned long long int); break;
        case PRINTF_LENGTH_LONGEST: u = va_arg(*ap, intmax_t); break;
        case PRINTF_LENGTH_SIZE: u = va_arg(*ap, size_t); break;
        case PRINTF_LENGTH_PTRDIFF: u = va_arg(*ap, ptrdiff_t); break;
        default: u = va_arg(*ap, unsigned int); break;
    }

    /* Get the number of characters needed to print the number (in `i`) */
    n = u;
    i = 0;
    while (n > 0) {
        n /= 10;
        i++;
    }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)
    // NOTE: '+', ' ', and '#' flags are invalid for this specifier

    if (u == 0) { putc_fun('0'); count++; }

    // TODO: there may be a better way to do this (this is O(n^2))
    while (i --> 0) {
        n = u;
        for (j = 0; j < i; j++) {
            n /= 10;
        }
        putc_fun('0' + (n % 10));
        count++;
    }
    return count;
}

// Unsigned octal numbers
int generic_printf_octal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    size_t i, n;
    uintmax_t o;

    switch (f->length) {
        case PRINTF_LENGTH_SHORT2: o = va_arg(*ap, unsigned int) & 0xFF; break;
        case PRINTF_LENGTH_SHORT: o = va_arg(*ap, unsigned int) & 0xFFFF; break;
        case PRINTF_LENGTH_LONG: o = va_arg(*ap, unsigned long int); break;
        case PRINTF_LENGTH_LONG2: o = va_arg(*ap, unsigned long long int); break;
        case PRINTF_LENGTH_LONGEST: o = va_arg(*ap, intmax_t); break;
        case PRINTF_LENGTH_SIZE: o = va_arg(*ap, size_t); break;
        case PRINTF_LENGTH_PTRDIFF: o = va_arg(*ap, ptrdiff_t); break;
        default: o = va_arg(*ap, unsigned int); break;
    }
    count = 0;
    /* Get the number of characters needed to print the number (in `i`) */
    n = o;
    i = 0;
    while (n > 0) {
        n >>= 3;  // n /= 8
        i++;
    }

    // Handle '#' flag
    // TODO: if precision is 0 and the number is 0, print nothing
    if (f->flags & PRINTF_FLAGS_PRECEDING_ID) { putc_fun('0'); count++; }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)
    // NOTE: '+' and ' ' flags are invalid for this specifier
    if (o == 0) { putc_fun('0'); count++; }
    while (i --> 0) {
        n = o >> 3*i;
        putc_fun('0' + (n & 0b111));
        count++;
    }
    return count;
}

// Hex numbers (check f->specifier for lower/UPPERcase)
int generic_printf_hex(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    size_t i, n;
    uintmax_t x;
    bool uppercase;

    switch (f->length) {
        case PRINTF_LENGTH_SHORT2: x = va_arg(*ap, unsigned int) & 0xFF; break;
        case PRINTF_LENGTH_SHORT: x = va_arg(*ap, unsigned int) & 0xFFFF; break;
        case PRINTF_LENGTH_LONG: x = va_arg(*ap, unsigned long int); break;
        case PRINTF_LENGTH_LONG2: x = va_arg(*ap, unsigned long long int); break;
        case PRINTF_LENGTH_LONGEST: x = va_arg(*ap, intmax_t); break;
        case PRINTF_LENGTH_SIZE: x = va_arg(*ap, size_t); break;
        case PRINTF_LENGTH_PTRDIFF: x = va_arg(*ap, ptrdiff_t); break;
        default: x = va_arg(*ap, unsigned int); break;
    }
    count = 0;
    /* Get the number of characters needed to print the number (in `i`) */
    n = x;
    i = 0;
    while (n > 0) {
        n >>= 4;  // n /= 8
        i++;
    }

    if (f->specifier == PRINTF_SPECIFIER_HEX_LOWER) uppercase = false;
    else uppercase = true;

    // Handle '#' flag
    // TODO: if precision is 0 and the number is 0, print nothing
    if (f->flags & PRINTF_FLAGS_PRECEDING_ID) {
        putc_fun('0'); count++;
        if (uppercase) putc_fun('X');
        else putc_fun('x');
        count++;
    }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)
    // NOTE: '+' and ' ' flags are invalid for this specifier
    if (x == 0) { putc_fun('0'); count++; }
    while (i --> 0) {
        n = x >> 4*i;
        if ((n & 0xF) > 9) {
            if (f->specifier == PRINTF_SPECIFIER_HEX_LOWER)
                putc_fun('a' - 10 + (n & 0xF)); // HEX_LOWER
            else putc_fun('A' - 10 + (n & 0xF)); // HEX_UPPER
        } else putc_fun('0' + (n & 0xF));
        count++;
    }
    return count;
}

// Floats (TODO)
int generic_printf_float(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    /* FLOATS are not supported yet.
     * IEEE 754 floating point numbers are stupidly complex, so printing them
     * is very difficult. I will implement this last (after everything else
     * is working, including the width and precision sub-specifiers).
     *
     * - Extract the integer part
     *   - How would we even do this?
     * - print the decimal point
     * - Evaluate the fractional part to the desired precision (default
     *   precision is 6)
     */
    return -1;
}

// Chars
int generic_printf_char(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    wchar_t c;
    switch (f->length) {
        case PRINTF_LENGTH_LONG: c = va_arg(*ap, int) & 0xFF; break;
        default: c = va_arg(*ap, wchar_t); break;
    }
    putc_fun(c);
    return 1;
}

// Strings
int generic_printf_string(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    const char* s;
    const wchar_t* ls;

    count = 0;
    switch (f->length) {
        case PRINTF_LENGTH_LONG:
            ls = va_arg(*ap, wchar_t*);
//            uart_puts("Address: 0x"); uart_hex64((ptrdiff_t)ls); uart_puts("\n");
            if (!ls) return -1; // NULL pointer
            while (*ls) { putc_fun(*ls); ls++; count++; }
            break;
        default:
            s = va_arg(*ap, char*);
//            uart_puts("Address: 0x"); uart_hex64((ptrdiff_t)s); uart_puts("\n");
            if (!s) return -1; // NULL pointer
            while (*s) { putc_fun(*s); s++; count++; }
            break;
    }
    return count;
}

// Pointer
int generic_printf_pointer(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap) {
    size_t count;
    size_t i, n;
    ptrdiff_t p;
    // The LENGTH sub-specifier is not supported for the pointer specifier
    p = (ptrdiff_t)va_arg(*ap, void*);

    /* FLAGS
     * NOT supported: '+', ' ', '#', '0'
     * Supported: '-' TODO
     */

    /* Get the number of characters needed to print the number (in `i`) */
    n = p;
    i = 0;
    while (n > 0) {
        n >>= 4;  // n /= 8
        i++;
    }

    count = 0;
    // All pointers need a "0x" prefix
    putc_fun('0'); count++;
    putc_fun('x'); count++;

    if (p == 0) { putc_fun('0'); count++; }
    while (i --> 0) {
        n = p >> 4*i;
        if ((n & 0xF) > 9) {
            if (f->specifier == PRINTF_SPECIFIER_HEX_LOWER)
                putc_fun('a' - 10 + (n & 0xF)); // HEX_LOWER
            else putc_fun('A' - 10 + (n & 0xF)); // HEX_UPPER
        } else putc_fun('0' + (n & 0xF));
        count++;
    }
    return count;
}

// Get number of chars (pointer in arguments list)
int generic_printf_getcount(void putc_fun(uint32_t), const printf_format_t* f, size_t c, va_list* ap) {
    void* p;
    p = va_arg(*ap, void*);
    if (!p) return -1; // NULL pointer
    switch (f->length) {
        // Long double (L) is not valid here
        case PRINTF_LENGTH_SHORT2: *(signed char*)p = c; break;
        case PRINTF_LENGTH_SHORT: *(short int*)p = c; break;
        case PRINTF_LENGTH_LONG: *(long int*)p = c; break;
        case PRINTF_LENGTH_LONG2: *(long long int*)p = c; break;
        case PRINTF_LENGTH_LONGEST: *(intmax_t*)p = c; break;
        case PRINTF_LENGTH_SIZE: *(size_t*)p = c; break;
        case PRINTF_LENGTH_PTRDIFF: *(ptrdiff_t*)p = c; break;
        default: *(int*)p = c; break;
    }
    return 0;
}
