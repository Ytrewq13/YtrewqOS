#ifndef PRINTF_H
#define PRINTF_H 1

#include <stdarg.h>

#include "drivers/hw/uart.h"
#include "drivers/graphics/console.h"
#include "io/printf.h"

int printf(const char *fmt, ...);

#endif /* printf_h */
