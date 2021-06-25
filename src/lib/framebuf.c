#include "framebuf.h"

// Allocate the Frame Buffer
ERROR_TYPE fb_alloc(uint32_t alignment, mem_info_t* result) {
    volatile uint32_t __attribute__((aligned(16))) mbox[8];
    uint32_t response[2];
    ERROR_TYPE err;

    err = mbox_prop_call((void*)mbox, MBOX_TAG_FB_ALLOC, 8, &alignment, response);
    if (err != MBOX_SUCCESS)
        return err;

    result->base_addr = response[0];
    result->size = response[1];

    return FB_SUCCESS;
}

// Release and free the Frame Buffer
ERROR_TYPE fb_release() {
    volatile uint32_t __attribute__((aligned(16))) mbox[6];
    ERROR_TYPE err;

    err = mbox_prop_call((void*)mbox, MBOX_TAG_FB_RELEASE, 0, NULL, NULL);
    if (err != MBOX_SUCCESS)
        return err;

    return FB_SUCCESS;
}


ERROR_TYPE fb_dims(framebuf_attr_action act, framebuf_dims_t* dims, framebuf_dims_t* result) {
    size_t msize = 8; // Buffer size is 8 bytes, mailbox is 4*8 bytes
    volatile uint32_t __attribute__((aligned(16))) mbox[msize];
    ERROR_TYPE err;
    enum MBOX_TAG_IDENTIFIERS tag;

    switch (act) {
        case FB_ATTR_GET:
            tag = MBOX_TAG_FB_GET_DIMS;
            break;
        case FB_ATTR_SET:
            tag = MBOX_TAG_FB_SET_DIMS;
            break;
        case FB_ATTR_TEST:
        default:
            tag = MBOX_TAG_FB_TEST_DIMS;
            break;
    }

    err = mbox_prop_call((void*)mbox, tag, (msize-6)*4, dims, result);
    if (err != MBOX_SUCCESS)
        return err;
    return FB_SUCCESS;
}

ERROR_TYPE fb_vdims(framebuf_attr_action act, framebuf_dims_t* dims, framebuf_dims_t *result) {
    size_t msize = 8; // Buffer size is 8 bytes, mailbox is 4*8 bytes
    volatile uint32_t __attribute__((aligned(16))) mbox[msize];
    ERROR_TYPE err;
    enum MBOX_TAG_IDENTIFIERS tag;

    switch (act) {
        case FB_ATTR_GET:
            tag = MBOX_TAG_FB_GET_VIRT_DIMS;
            break;
        case FB_ATTR_SET:
            tag = MBOX_TAG_FB_SET_VIRT_DIMS;
            break;
        case FB_ATTR_TEST:
        default:
            tag = MBOX_TAG_FB_TEST_VIRT_DIMS;
            break;
    }

    err = mbox_prop_call((void*)mbox, tag, (msize-6)*4, dims, result);
    if (err != MBOX_SUCCESS)
        return err;
    return FB_SUCCESS;
}

ERROR_TYPE fb_blank_screen(bool state) {
    volatile uint32_t __attribute__((aligned(16))) mbox[7];
    ERROR_TYPE err;
    uint32_t input, result;

    // false -> state off
    // true  -> state on
    input = state ? 1 : 0;

    err = mbox_prop_call((void*)mbox, MBOX_TAG_FB_BLANK_SCREEN, 4, &input, &result);
    if (err != MBOX_SUCCESS)
        return err;
    if (result != input) // What causes the clear screen to fail?
        return FB_ERR_UNKNOWN;
    return FB_SUCCESS;
}


/*
 * For GET, depth can be NULL
 */
ERROR_TYPE fb_bit_depth(framebuf_attr_action act, uint32_t* depth, uint32_t* result) {
    volatile uint32_t __attribute__((aligned(16))) mbox[7];
    enum MBOX_TAG_IDENTIFIERS tag;
    ERROR_TYPE err;

    if (result == NULL) return FB_ERR_NULLPTR;

    switch (act) {
        case FB_ATTR_GET:
            tag = MBOX_TAG_FB_GET_DEPTH;
            break;
        case FB_ATTR_SET:
            tag = MBOX_TAG_FB_SET_DEPTH;
            if (depth == NULL) return FB_ERR_NULLPTR;
            break;
        case FB_ATTR_TEST:
        default:
            tag = MBOX_TAG_FB_TEST_DEPTH;
            if (depth == NULL) return FB_ERR_NULLPTR;
            break;
    }

    err = mbox_prop_call((void*)mbox, tag, 4, depth, result);
    if (err != MBOX_SUCCESS)
        return err;
    return FB_SUCCESS;
}
