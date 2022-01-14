/* mbox.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-08-10
 */

#include "drivers/hw/mmio.h"
#include "drivers/hw/mbox.h"

#include <stdint.h>

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

/* Start a mailbox command.
 */
ERROR_TYPE mbox_command_start(enum MBOX_CHANNELS channel, uint32_t* mbox)
{
    uint32_t r;

    /* Sanity check arguments */
    if (!mbox) return MBOX_ERR_NULLPTR;
    if ((uint64_t)mbox & 0xF) return MBOX_ERR_PTR_ALIGN;

    /* Pack the address and channel together */
    r = (uint64_t)mbox | channel;
    /* Wait until we can write to the mailbox */
    while (GET32(MBOX_WRITE_STATUS) & MBOX_FULL) __asm volatile("nop");
    /* Write the address of our message to the mailbox with channel identifier
     */
    PUT32(MBOX_WRITE, r);

    return MBOX_SUCCESS;
}

/* Wait for a mailbox command to finish.
 */
ERROR_TYPE mbox_command_wait(enum MBOX_CHANNELS channel, uint32_t* mbox)
{
    uint32_t r;

    /* Sanity check arguments */
    if (!mbox) return MBOX_ERR_NULLPTR;
    if ((uint64_t)mbox & 0xF) return MBOX_ERR_PTR_ALIGN;

    /* Pack the address and channel together */
    r = (uint64_t)mbox | channel;

    /* Wait for the response. */
    while (1) {
        /* Wait until we can read something */
        while (GET32(MBOX_READ_STATUS) & MBOX_EMPTY) __asm volatile("nop");
        /* Check if the response is for the message we are waiting for */
        if (r == GET32(MBOX_READ) && mbox[1] == MBOX_RESPONSE)
            return MBOX_SUCCESS;
        else
            return MBOX_ERR_UNKNOWN;  // TODO
    }
}

ERROR_TYPE mbox_prop_call_internal(void* mbox, enum MBOX_TAG_IDENTIFIERS tag_id,
                                   size_t buf_size, uint32_t req, void* input,
                                   void* result)
{
    ERROR_TYPE err;
    uint32_t i, buf_count, buf_min, buf_max, msg_size;
    uint8_t* res = result;

    buf_count = (buf_size + 3) / 4;  // Number of 32-bit entries in the buffer
    buf_min   = 5;
    buf_max   = buf_min - 1 + buf_count;

    msg_size = (buf_max + 2) * 4;  // Message size (in bytes)

    if ((uint64_t)mbox & 0xF) return MBOX_ERR_PTR_ALIGN;

    // Fill the values in the mbox
    ((uint32_t*)mbox)[0] = msg_size;
    ((uint32_t*)mbox)[1] = MBOX_REQUEST;
    // Tag start
    ((uint32_t*)mbox)[2] = tag_id;
    ((uint32_t*)mbox)[3] = buf_size;
    ((uint32_t*)mbox)[4] = req;

    // Zero/prepare the value buffer.
    for (i = buf_min; i < buf_max; i++) {
        if (input)  // Copy the input
            ((uint32_t*)mbox)[i] = ((uint32_t*)input)[i - buf_min];
        else  // Zero the buffer
            ((uint32_t*)mbox)[i] = 0;
    }
    ((uint32_t*)mbox)[buf_max + 1] = MBOX_TAG_LAST;

    // Send the message to the GPU and receive answer
    if ((err = mbox_command_start(MBOX_CH_PROP_W, mbox)) != MBOX_SUCCESS)
        return err;
    if ((err = mbox_command_wait(MBOX_CH_PROP_W, mbox)) != MBOX_SUCCESS)
        return err;
    // Copy the result
    if (result != NULL)
        for (i = 0; i < buf_size; i++) res[i] = ((uint8_t*)mbox)[4 * buf_min + i];
    return MBOX_SUCCESS;
}

/* Function to perform a mailbox call and put the value returned into *result
 * result should point to a space at least buf_size bytes large.
 * input should be either NULL (then we zero the buffer) or a multiple of 4
 * bytes long (at least buf_size bytes).
 */
ERROR_TYPE mbox_prop_call(void* mbox, enum MBOX_TAG_IDENTIFIERS tag_id,
                          size_t buf_size, void* input, void* result)
{
    return mbox_prop_call_internal(mbox, tag_id, buf_size, 0, input, result);
}

ERROR_TYPE mbox_call_raw(void* mbox)
{
    uint32_t r;

    /* Sanity check arguments */
    if (!mbox) return MBOX_ERR_NULLPTR;
    if ((uint64_t)mbox & 0xF) return MBOX_ERR_PTR_ALIGN;

    /* Pack the address and channel together */
    r = (uint64_t)mbox | MBOX_CH_PROP_W;
    /* Wait until we can write to the mailbox */
    while (GET32(MBOX_WRITE_STATUS) & MBOX_FULL) __asm volatile("nop");
    /* Write the address of our message to the mailbox with channel identifier
     */
    PUT32(MBOX_WRITE, r);

    /* Wait for the response. */
    while (true) {
        /* Wait until we can read something */
        while (GET32(MBOX_READ_STATUS) & MBOX_EMPTY) __asm volatile("nop");
        /* Check if the response is for the message we are waiting for */
        if (GET32(MBOX_READ) == r)
            return ((uint32_t*)mbox)[1] == MBOX_RESPONSE_SUCCESS;
        // TODO: Why doesn't this work if we return MBOX_SUCCESS/MBOX_ERR_UNKNOWN?
    }
}

/* What to put in the mailbox for the property interface channel (from
 * https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface):
 *
 * - u32: The size of the message in bytes (including the header values, end
 * tag, and padding - i.e. the size of the whole message)
 * - u32: Request/Response code (from enum MBOX_REQUEST_RESPONSE)
 * - u8+: Sequence of concatenated "tags" - This specifies what we want from
 *   the GPU. Tags are processed in order unless an interface requires
 *   multiple tags for a single operation.
 *   The format of these "tags" is as follows:
 *   - u32: Tag identifier (one of enum MBOX_TAG_IDENTIFIERS)
 *   - u32: Value buffer size in bytes
 *   - u32: Request/Response codes:
 *          - bit 31 clear: Request, bits 30:0 reserved
 *          - bit 31 set: Response, bits 30:0 value length in bytes
 *   - u8+: Value buffer
 *   - u8+: Padding to align the tag to 32 bits
 * - u32: The end tag (MBOX_TAG_LAST) (0x00000000)
 * - u8+: Padding TODO: is an amount of padding ever required?
 */
