/* Copyright (C) 2013 by John Cronin <jncronin@tysos.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* This file's contents were originally copied from
 * https://github.com/jncronin/rpi-boot/blob/master/heap.c
 * The original file was GPL'd (2.0), as is this project.
 */


#include <stdint.h>
#include <stddef.h>

#define MAX_BRK (((uintptr_t)&_start - 0x8000) + 0xf0000)

#define MAX_BUF 0x100000

extern char _start, __end;

uintptr_t cur_brk = 0;
uintptr_t cur_buf = MAX_BRK;

uintptr_t alloc_buf(size_t size)
{
    uintptr_t old_buf = cur_buf;

    cur_buf += size;
    if(cur_buf > MAX_BUF) {
        cur_buf = old_buf;
        return 0;
    }
    // Align up to a 512 byte value
    if(cur_buf & 0x1ff) {
        cur_buf &= ~0x1ffl;
        cur_buf += 0x200;
    }
    return old_buf;
}

/* Increment the program break (or decrement for a negative value)
 * The program break will always be aligned on a page (4096 byte)
 * boundary when initialised, so if the value isn't a multiple of 0x1000 then
 * the break will be moved forwards to the next page.
 */
void *sbrk(ptrdiff_t increment)
{
    // sbrk returns the previous brk value

    // First set up cur_brk if not done already
    if(cur_brk == 0) {
        // printf("HEAP: initializing at %p\n", &__end);
        cur_brk = (uintptr_t)&__end;
        // If not aligned on a page boundary, move to the next page.
        if(cur_brk & 0xfff) {
            cur_brk &= ~0xfffl;
            cur_brk += 0x1000;
        }
    }

    uintptr_t old_brk = cur_brk;

    cur_brk += increment;
    if(cur_brk >= MAX_BRK) {
        cur_brk = old_brk;
        return (void*)-1;
    }
    return (void*)old_brk;
}

