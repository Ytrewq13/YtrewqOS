#include "io/printf.h"


// TODO: move string functions to a string.h header
bool str_contains(const char* s, char c)
{
    while (*s != 0) {
        if (*s++ == c) return true;
    }
    return false;
}

// TODO: move char functions to a chars.h header?
bool is_digit(char c) { return str_contains("0123456789", c); }

const char* printf_parse_flags(const char* s, printf_format_t* f)
{
    // FLAGS (-,+, ,#,0)
    if (str_contains("-+ #0", *s)) {
        f->items |= PRINTF_ITEM_FLAGS;
        while (str_contains("-+ #0", *s)) {
            switch (*s) {
                case '-': f->flags |= PRINTF_FLAGS_LEFT_JUSTIFY; break;
                case '+': f->flags |= PRINTF_FLAGS_PLUS_OR_MINUS; break;
                case ' ': f->flags |= PRINTF_FLAGS_SPACE_IF_NO_SIGN; break;
                case '#': f->flags |= PRINTF_FLAGS_PRECEDING_ID; break;
                case '0':
                    f->flags |= PRINTF_FLAGS_LEFTPAD_ZEROES;
                    break;
                    // TODO: '0' should be ignored when '-' is present
                    // TODO: Find out about other flags' behaviour
            }
            s++;
        }
    }
    return s;
}

const char* printf_parse_width(const char* s, printf_format_t* f)
{
    // WIDTH (*,[number])
    if (!(*s == '*' || is_digit(*s))) return s;
    f->items |= PRINTF_ITEM_WIDTH;
    if (*s == '*') {
        f->width_as_arg = true;
        return s + 1;
    }
    while (is_digit(*s)) { f->width = 10 * f->width + *(s++) - '0'; }
    return s;
}

const char* printf_parse_precision(const char* s, printf_format_t* f)
{
    // PRECISION (.*,.[number])
    if (*s == '.') {
        f->items |= PRINTF_ITEM_PRECISION;
        s++;
        while (*s == '*' || is_digit(*s)) {
            if (*s == '*') f->precision_as_arg = true;
            else
                f->precision = 10 * f->precision + *s - '0';
            s++;
        }
    }
    return s;
}

const char* printf_parse_length(const char* s, printf_format_t* f)
{
    // LENGTH (hh,h,l,ll,j,z,t,L)
    if (str_contains("hljztL", *s)) {
        f->items |= PRINTF_ITEM_LENGTH;
        switch (*s) {
            case 'h':
                if (*(s + 1) == 'h') {
                    f->length = PRINTF_LENGTH_SHORT2;
                    s++;
                } else
                    f->length = PRINTF_LENGTH_SHORT;
                break;
            case 'l':
                if (*(s + 1) == 'l') {
                    f->length = PRINTF_LENGTH_LONG2;
                    s++;
                } else
                    f->length = PRINTF_LENGTH_LONG;
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

const char* printf_parse_specifier(const char* s, printf_format_t* f)
{
    // SPECIFIER (%,d,i,u,o,x,X,f,F,e,E,g,G,a,A,c,s,p,n)
    if (str_contains("diuoxXfFeEgGaAcspn%", *s)) {
        switch (*s) {
            case 'd':
            case 'i': f->specifier = PRINTF_SPECIFIER_INT; break;
            case 'u': f->specifier = PRINTF_SPECIFIER_UNSIGNED_INT; break;
            case 'o': f->specifier = PRINTF_SPECIFIER_UNSIGNED_OCTAL; break;
            case 'x': f->specifier = PRINTF_SPECIFIER_HEX_LOWER; break;
            case 'X': f->specifier = PRINTF_SPECIFIER_HEX_UPPER; break;
            case 'f':  // FIXME: implement these specifiers
            case 'F':  // FIXME: implement these specifiers
            case 'e':  // FIXME: implement these specifiers
            case 'E':  // FIXME: implement these specifiers
            case 'g':  // FIXME: implement these specifiers
            case 'G':  // FIXME: implement these specifiers
            case 'a':  // FIXME: implement these specifiers
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
const char* printf_parse_format(const char* s, printf_format_t* f)
{
    f->specifier        = PRINTF_SPECIFIER_INT;
    f->flags            = PRINTF_FLAGS_NONE;
    f->items            = PRINTF_ITEMS_NONE;
    f->width            = 0;
    f->precision        = 0;
    f->width_as_arg     = false;
    f->precision_as_arg = false;
    f->length           = PRINTF_LENGTH_DEFAULT;

    if (s == NULL) return NULL;

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


int generic_printf(void putc_fun(uint32_t), const char* format, va_list ap)
{
    printf_format_t f;

    size_t count = 0;  // Count how many characters have been printed

    while (*format) {
        if (*format != '%') {
            putc_fun(*format);
            count++;
            format++;
            continue;
        }
        // Call the parse function on the next character.
        format = printf_parse_format(format + 1, &f);

        switch (f.specifier) {
            case PRINTF_SPECIFIER_PERCENT:
                putc_fun('%');
                count++;
                break;
            case PRINTF_SPECIFIER_INT: count += generic_printf_decimal(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_UNSIGNED_INT:
                count += generic_printf_unsigned(putc_fun, &f, &ap);
                break;
            case PRINTF_SPECIFIER_UNSIGNED_OCTAL:
                count += generic_printf_octal(putc_fun, &f, &ap);
                break;
            case PRINTF_SPECIFIER_HEX_LOWER:
            case PRINTF_SPECIFIER_HEX_UPPER: count += generic_printf_hex(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_CHAR: count += generic_printf_char(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_STRING: count += generic_printf_string(putc_fun, &f, &ap); break;
            case PRINTF_SPECIFIER_POINTER:
                count += generic_printf_pointer(putc_fun, &f, &ap);
                break;
            case PRINTF_SPECIFIER_GET_NCHARS:
                generic_printf_getcount(putc_fun, &f, count, &ap);
                break;
        }
    }

    return count;
}

// Signed integers
int generic_printf_decimal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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
        putc_fun('-');
        count++;
    } else if (f->flags & PRINTF_FLAGS_PLUS_OR_MINUS) {
        putc_fun('+');
        count++;  // + flag
    } else if (f->flags & PRINTF_FLAGS_SPACE_IF_NO_SIGN) {
        putc_fun(' ');
        count++;  // ' ' flag
    }

    if (d == 0) {
        putc_fun('0');
        count++;
    }

    // TODO: there may be a better way to do this (this is O(n^2))
    while (i-- > 0) {
        n = (d < 0) ? -d : d;  // n := abs(d)
        for (j = 0; j < i; j++) { n /= 10; }
        putc_fun('0' + (n % 10));
        count++;
    }
    return count;
}

// Unsigned integers
int generic_printf_unsigned(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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

    if (u == 0) {
        putc_fun('0');
        count++;
    }

    // TODO: there may be a better way to do this (this is O(n^2))
    while (i-- > 0) {
        n = u;
        for (j = 0; j < i; j++) { n /= 10; }
        putc_fun('0' + (n % 10));
        count++;
    }
    return count;
}

// Unsigned octal numbers
int generic_printf_octal(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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
    if (f->flags & PRINTF_FLAGS_PRECEDING_ID) {
        putc_fun('0');
        count++;
    }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)
    // NOTE: '+' and ' ' flags are invalid for this specifier
    if (o == 0) {
        putc_fun('0');
        count++;
    }
    while (i-- > 0) {
        n = o >> 3 * i;
        putc_fun('0' + (n & 0b111));
        count++;
    }
    return count;
}

// Hex numbers (check f->specifier for lower/UPPERcase)
int generic_printf_hex(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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
    else
        uppercase = true;

    // Handle '#' flag
    // TODO: if precision is 0 and the number is 0, print nothing
    if (f->flags & PRINTF_FLAGS_PRECEDING_ID) {
        putc_fun('0');
        count++;
        if (uppercase) {
            putc_fun('X');
            count++;
        } else {
            putc_fun('x');
            count++;
        }
    }

    // TODO: Handle '-' and '0' flags and width attribute
    // ('0' flag ignored when '-' flag present)
    // NOTE: '+' and ' ' flags are invalid for this specifier
    if (x == 0) {
        putc_fun('0');
        count++;
    }
    while (i-- > 0) {
        n = x >> 4 * i;
        if ((n & 0xF) > 9) {
            if (f->specifier == PRINTF_SPECIFIER_HEX_LOWER) {
                // HEX_LOWER
                putc_fun('a' - 10 + (n & 0xF));
                count++;
            } else {
                // HEX_UPPER
                putc_fun('A' - 10 + (n & 0xF));
                count++;
            }
        } else {
            putc_fun('0' + (n & 0xF));
            count++;
        }
    }
    return count;
}

// Floats (TODO)
int generic_printf_float(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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
int generic_printf_char(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
    wchar_t c;
    switch (f->length) {
        case PRINTF_LENGTH_LONG: c = va_arg(*ap, int) & 0xFF; break;
        default: c = va_arg(*ap, wchar_t); break;
    }
    putc_fun(c);
    return 1;
}

// Strings
int generic_printf_string(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
    size_t count;
    const char* s;
    const wchar_t* ls;

    count = 0;
    switch (f->length) {
        case PRINTF_LENGTH_LONG:
            ls = va_arg(*ap, wchar_t*);
            //            uart_puts("Address: 0x"); uart_hex64((ptrdiff_t)ls);
            //            uart_puts("\n");
            if (!ls) return -1;  // NULL pointer
            while (*ls) {
                putc_fun(*ls);
                count++;
                ls++;
            }
            break;
        default:
            s = va_arg(*ap, char*);
            //            uart_puts("Address: 0x"); uart_hex64((ptrdiff_t)s);
            //            uart_puts("\n");
            if (!s) return -1;  // NULL pointer
            while (*s) {
                putc_fun(*s);
                count++;
                s++;
            }
            break;
    }
    return count;
}

// Pointer
int generic_printf_pointer(void putc_fun(uint32_t), const printf_format_t* f, va_list* ap)
{
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
    putc_fun('0');
    count++;
    putc_fun('x');
    count++;

    if (p == 0) {
        putc_fun('0');
        count++;
    }
    while (i-- > 0) {
        n = p >> 4 * i;
        if ((n & 0xF) > 9) {
            putc_fun('a' - 10 + (n & 0xF));
            count++;
        } else {
            putc_fun('0' + (n & 0xF));
            count++;
        }
    }
    return count;
}

// Get number of chars (pointer in arguments list)
int generic_printf_getcount(void putc_fun(uint32_t), const printf_format_t* f, size_t c,
                            va_list* ap)
{
    void* p;
    p = va_arg(*ap, void*);
    if (!p) return -1;  // NULL pointer
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
