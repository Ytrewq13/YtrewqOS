/* mbox.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-06-17
 */

#include <stdint.h>

#include "mmio.h"
#include "mbox.h"

extern void PUT32(uint64_t addr, uint32_t x);
extern uint32_t GET32(uint64_t addr);

/* mailbox message buffer */
// Why is the mailbox 36*32 bits? Is that pre-determined or just chosen by
// the example code? TODO
volatile uint32_t __attribute__((aligned(16))) mbox[36];

uint32_t mbox_call(uint8_t channel) {
    uint32_t r;
    //    ?????????? the address of mbox is 64-bit. Why are we casting it to 32 bits?
    r = (((uint32_t)((uint64_t)&mbox)&~0xF) | (channel&0xF)); // FIXME
    /* wait until we can write to the mailbox */
    while (GET32(MBOX_WRITE_STATUS) & MBOX_FULL) __asm volatile("nop");
    /* write the address of our message to the mailbox with channel identifier */
    PUT32(MBOX_WRITE, r);
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        while (GET32(MBOX_READ_STATUS) & MBOX_EMPTY) __asm volatile("nop");
        /* is it a response to our message? */
        if (r == GET32(MBOX_READ))
            /* is it a valid successful response? */
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}

/* Run a command on a channel in the mailbox
 * waits for a response if the second argument is true
 *
 * Returns 0 on success, non-zero if error (the non-zero values will be error
 * codes when I implement error-codes)
 */
uint32_t mbox_command(enum MBOX_CHANNELS channel, bool blocking, uint32_t* msg) {
    // Sanity check arguments
    if (!msg) return 1; // FIXME: MBOX_MSG_NULL_POINTER_ERROR
    if (msg[0] > sizeof(mbox))
        return 1; // FIXME: MBOX_MSG_TOO_LARGE_ERROR
    // TODO: write this function
    return 0;
}

// Get the board's unique serial number with a mailbox call
uint64_t get_serial_number(uint32_t* err) {
    mbox[0] = 8*4;                  // Length of the message
    mbox[1] = MBOX_REQUEST;         // This is a request message

    mbox[2] = MBOX_TAG_GET_SERIAL_NUMBER;   // Get serial number command
    mbox[3] = 8;                    // Buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // Clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST; // TODO: why is this 0 named?

    // Send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP_W))
        return ((uint64_t)mbox[6] << 32 | mbox[5]);
    *err = 1; // TODO: create enum of mbox error codes.
    return 0;
}
