
#include "hw/mmio.h"
#include "hw/mbox.h"
#include "fs/block.h"
#include "kernel/timer.h"

enum EMMC_REGS {
    EMMC_BASE = MMIO_BASE | 0x00300000,

    /* Used for SD card specific command ACMD23 */
    EMMC_REGS_ARG2              = EMMC_BASE | 0x00,
    /* 31:16 BLKCNT
     * 15:10 Reserved (write 0, read don't care)
     *  9:0  BLKSIZE */
    EMMC_REGS_BLKSIZECNT        = EMMC_BASE | 0x04,
    /* Arguments for all commands except ACMD23. MUST be set before command is
     * issued using the CMDTM register. */
    EMMC_REGS_ARG1              = EMMC_BASE | 0x08,
    /* This register is used to issue commands to the eMMC module. Besides the
     * command, it also contains flags informing the eMMC module what card
     * response and type of data transfer to expect. Incorrect flags will
     * result in strange behaviour.
     * Bits:
     * 31:30 Reserved (write 0, read don't care)
     * 29:24 CMD_INDEX (index of the command to be issued to the card)
     * 23:22 CMD_TYPE:
     *      00: normal
     *      01: suspend (the current data transfer)
     *      10: resume (the last data transfer)
     *      11: abort (the current data transfer)
     * 21    CMD_ISDATA (0: no data xfer command, 1: data xfer command)
     * 20    CMD_IXCHK_EN (check that the response has the same index as the
     *                     command - 1 to enable)
     * 19    CMD_CRCCHK_EN (check the response's CRC - 1 to enable)
     * 18    Reserved (write 0, read don't care)
     * 17:16 CMD_RSPNS_TYPE (type of expected response from the card):
     *      00: no response
     *      01: 136 bit response
     *      10:  48 bit response
     *      11:  48 bit response using busy
     * 15:6  Reserved (write 0, read don't care)
     * 5     TM_MULTI_BLOCK (type of data transfer
     *                       0: single block, 1: multiple block)
     * 4     TM_DAT_DIR (direction of data transfer
     *                   0: from host to card, 1: from card to host)
     * 3:2   TM_AUTO_CMD_EN (set the command to be sent after completion of a
     *                       data transfer)
     *      00: no command
     *      01: CMD12
     *      10: CMD23
     *      11: reserved
     * 1     TM_BLKCNT_EN (enable the block counter for multiple block
     *                     transfers - 1 to enable)
     * 0     Reserved (write 0, read don't care)
     */
    EMMC_REGS_CMDTM             = EMMC_BASE | 0x0C,
    /* Bits 31:0 of the card's response */
    EMMC_REGS_RESP0             = EMMC_BASE | 0x10,
    /* Bits 63:32 of the card's response */
    EMMC_REGS_RESP1             = EMMC_BASE | 0x14,
    /* Bits 95:64 of the card's response */
    EMMC_REGS_RESP2             = EMMC_BASE | 0x18,
    /* Bits 127:96 of the card's response */
    EMMC_REGS_RESP3             = EMMC_BASE | 0x1C,
    /* This register is used to transfer data to/from the card */
    EMMC_REGS_DATA              = EMMC_BASE | 0x20,
    /* Information intended for debugging. Recommended to use the INTERRUPT
     * register instead. */
    EMMC_REGS_STATUS            = EMMC_BASE | 0x24,
    /* This register is used to configure the eMMC module. TODO */
    EMMC_REGS_CONTROL0          = EMMC_BASE | 0x28,
    /* This register is used to configure the eMMC module. TODO */
    EMMC_REGS_CONTROL1          = EMMC_BASE | 0x2C,
    /* This register holds the interrupt flags. TODO */
    EMMC_REGS_INTERRUPT         = EMMC_BASE | 0x30,
    /* This register is used to mask the flags in the INTERRUPT register. */
    EMMC_REGS_IRPT_MASK         = EMMC_BASE | 0x34,
    /* This register enables the flags in the INTERRUPT register. */
    EMMC_REGS_IRPT_EN           = EMMC_BASE | 0x38,
    /* This register is used to enable interrupts in the INTERRUPT register to
     * generate interrupts on the int_to_arm output. */
    EMMC_REGS_CONTROL2          = EMMC_BASE | 0x3C,

    /* Unsupported(?) but used by the implementation in emmc.c */
    EMMC_REGS_CAPABILITIES_0    = EMMC_BASE | 0x40,
    EMMC_REGS_CAPABILITIES_1    = EMMC_BASE | 0x44,

    /* Used to fake an interrupt for debugging. Set a bit to 1 to generate an
     * interrupt on that bit in the INTERRUPT register. */
    EMMC_REGS_FORCE_IRPT        = EMMC_BASE | 0x50,

    /* Configures after how many card clock cycles a timeout for eMMC cards in
     * boot mode is flagged. */
    EMMC_REGS_BOOT_TIMEOUT      = EMMC_BASE | 0x70,
    /* Selects which submodules are accessed by the debug bus. */
    EMMC_REGS_DBG_SEL           = EMMC_BASE | 0x74,

    /* TODO */
    EMMC_REGS_EXRDFIFO_CFG      = EMMC_BASE | 0x80,
    /* TODO */
    EMMC_REGS_EXRDFIFO_EN       = EMMC_BASE | 0x84,
    /* Delay the card clock when sampling the returning data and command
     * response from the card. Bits 31-3 reserved, bits 2-0:
     * - 000: 200ps typically
     * - 001: 400ps typically
     * - 010: 400ps typically
     * - 011: 600ps typically
     * - 100: 700ps typically
     * - 101: 900ps typically
     * - 110: 900ps typically
     * - 111: 1100ps typically
     * */
    EMMC_REGS_TUNE_STEP         = EMMC_BASE | 0x88,
    /* How many steps the sampling clock is delayed in SDR mode
     * (0 to 40 steps - bits 5:0). */
    EMMC_REGS_TUNE_STEPS_STD    = EMMC_BASE | 0x8C,
    /* How many steps the sampling clock is delayed in DDR mode
     * (0 to 40 steps - bits 5:0). */
    EMMC_REGS_TUNE_STEPS_DDR    = EMMC_BASE | 0x90,

    /* TODO */
    EMMC_REGS_SPI_INT_SPT       = EMMC_BASE | 0xF0,

    /* TODO */
    EMMC_REGS_SLOT_ISR_VER      = EMMC_BASE | 0xFC,
};

enum {
    EMMC_CMD0,   /* GO_IDLE_STATE, GO_PRE_IDLE_STATE, BOOT_INITIATION */
    EMMC_CMD1,   /* SEND_OP_COND */
    EMMC_CMD2,   /* ALL_SEND_CID */
    EMMC_CMD3,   /* SET_RELATIVE_ADDR */
    EMMC_CMD4,   /* SET_DSR */
    EMMC_CMD5,   /* SLEEP_AWAKE */
    EMMC_CMD6,   /* SWITCH */
    EMMC_CMD7,   /* SELECT/DESELECT_CARD */
    EMMC_CMD8,   /* SEND_EXT_CSD */
    EMMC_CMD9,   /* SEND_CSD */
    EMMC_CMD10,  /* SEND_CID */
    EMMC_CMD12,  /* STOP_TRANSMISSION */
    EMMC_CMD13,  /* SEND_STATUS */
    EMMC_CMD14,  /* BUSTEST_R */
    EMMC_CMD15,  /* GO_INACTIVE_STATE */
    EMMC_CMD19,  /* BUSTEST_W */
    EMMC_CMD16,  /* SET_BLOCKLEN */
    EMMC_CMD17,  /* READ_SINGLE_BLOCK */
    EMMC_CMD18,  /* READ_MULTIPLE_BLOCK */
    EMMC_CMD21,  /* SEND_TUNING_BLOCK */
    EMMC_CMD23,  /* SET_BLOCK_COUNT */
    EMMC_CMD23P, /* SET_BLOCK_COUNT */
    EMMC_CMD24,  /* WRITE_BLOCK */
    EMMC_CMD25,  /* WRITE_MULTIPLE_BLOCK */
    EMMC_CMD26,  /* PROGRAM_CID - RESERVED FOR MANUFACTURER */
    EMMC_CMD27,  /* PROGRAM_CSD */
    EMMC_CMD49,  /* SET_TIME */
};

struct emmc_block_dev
{
    struct block_device bd;
    uint32_t card_supports_sdhc;
    uint32_t card_supports_18v;
    uint32_t card_ocr;
    uint32_t card_rca;
    uint32_t last_interrupt;
    uint32_t last_error;

    struct sd_scr *scr;

    int failed_voltage_switch;

    uint32_t last_cmd_reg;
    uint32_t last_cmd;
    uint32_t last_cmd_success;
    uint32_t last_r0;
    uint32_t last_r1;
    uint32_t last_r2;
    uint32_t last_r3;

    void *buf;
    int blocks_to_transfer;
    size_t block_size;
    int use_sdma;
    int card_removal;
    uint32_t base_clock;
};

void emmc_set_base(uint32_t base);

int sd_card_init(struct block_device **dev);
int sd_read(struct block_device *dev, uint8_t *buf, size_t buf_size, uint32_t block_no);
#ifdef SD_WRITE_SUPPORT
int sd_write(struct block_device *dev, uint8_t *buf, size_t buf_size, uint32_t block_no);
#endif
