#ifndef PRINTF_H
#define PRINTF_H 1

#include <stdarg.h>

#include "hw/uart.h"
#include "graphics/console.h"
#include "io/printf.h"

int printf(const char *fmt, ...);

#endif /* printf_h */
