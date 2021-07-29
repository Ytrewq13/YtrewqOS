#ifndef FRAMEBUF_H
#define FRAMEBUF_H 1

#include <stdbool.h>
#include <stddef.h>

// TODO: figure out build settings to use angle brackets for includes
#include "error_types.h"
#include "mbox.h"
#include "mem.h"

typedef enum {
    FB_ERRORS_BASE = 0x80000000,  // TODO: Centralise and DOCUMENT these

    FB_SUCCESS = FB_ERRORS_BASE | 0x00,

    FB_ERR_UNKNOWN = FB_ERRORS_BASE | 0x01,
    FB_ERR_NULLPTR = FB_ERRORS_BASE | 0x02,
} FRAMEBUF_ERROR;

typedef struct {
    size_t width;
    size_t height;
} framebuf_dims_t;

typedef enum {
    FB_ATTR_GET  = 0,
    FB_ATTR_TEST = 1,
    FB_ATTR_SET  = 2
} framebuf_attr_action;

// Allocate the Frame Buffer
ERROR_TYPE fb_alloc(uint32_t alignment, mem_info_t* result);
// Release and free the Frame Buffer
ERROR_TYPE fb_release();
// Initialise the Frame Buffer (set dimensions and then allocate)
// TODO: is any kind of initialisation required?
ERROR_TYPE fb_init(framebuf_dims_t dims, framebuf_dims_t virt_dims);

// Get/Test/Set Dimensions (in memory)
ERROR_TYPE fb_dims(framebuf_attr_action act, framebuf_dims_t* dims,
                   framebuf_dims_t* result);

// Get/Test/Set Virtual Dimensions (to display)
ERROR_TYPE fb_vdims(framebuf_attr_action act, framebuf_dims_t* dims,
                    framebuf_dims_t* result);

// Blank the screen to a given state (need to test what the state does)
ERROR_TYPE fb_blank_screen(bool state);

// Get/Set/Test bit depth
ERROR_TYPE fb_bit_depth(framebuf_attr_action act, uint32_t* depth,
                        uint32_t* result);

// DONE:
// - dims
// - vdims
// - blank_screen
// TODO:
// - etc.

#endif
