#ifndef FRAMEBUF_H
#define FRAMEBUF_H 1

#include <stdbool.h>
#include <stddef.h>

// TODO: figure out build settings to use angle brackets for includes
#include "error_types.h"
#include "hw/mbox.h"
#include "hw/mem.h"

typedef enum {
    FB_ERRORS_BASE = 0x80000000,  // TODO: Centralise and DOCUMENT these

    FB_SUCCESS = FB_ERRORS_BASE | 0x00,

    FB_ERR_UNKNOWN = FB_ERRORS_BASE | 0x01,
    FB_ERR_NULLPTR = FB_ERRORS_BASE | 0x02,
} FRAMEBUF_ERROR;

typedef struct {
    uint32_t width;
    uint32_t height;
} framebuf_dims_t;

typedef enum {
    FB_ATTR_GET  = 0,
    FB_ATTR_TEST = 1,
    FB_ATTR_SET  = 2
} framebuf_attr_action;

enum pix_order {
    PIX_ORDER_RGBA = 0,
    PIX_ORDER_BGRA = 1,
};


/* This is the framebuffer. */
extern mem_info_t framebuf;
extern uint8_t framebuf_enabled;

// Allocate the Frame Buffer
ERROR_TYPE fb_alloc(uint32_t alignment);
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

// Get byte pitch (bytes per row of framebuf)
ERROR_TYPE fb_get_pitch(uint32_t* result);

// Get/Set/Test the pixel order
ERROR_TYPE fb_pix_order(framebuf_attr_action act, enum pix_order* pix_ord,
                        enum pix_order* result);

// DONE:
// - dims
// - vdims
// - blank_screen
// TODO:
// - etc.

#endif
