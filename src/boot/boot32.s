# Some test bytes - can be removed after confirming this always ends up in the
# second sector.
.byte 0x69
.byte 0x42
.byte 0x69
# TODO:
# - Call a function in the boot sector from here
# - Call a function here from the boot sector
# - Bootstrap up to long mode
#   - Get into protected mode with one sector
#     - Setup 32-bit GDT
#     - Load the GDT
#     - Anything else?
#     - Perform a 32-bit jump to enter protected mode?
#   - Get into long mode with another sector
#     - Setup a 64-bit GDT
#     - Load the GDT
#     - Anything else?
#     - Perform a long jump to enter long mode
