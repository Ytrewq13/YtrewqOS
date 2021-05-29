.text
home:
#mov $our_string, %eax # $our_string is expecting to be loaded at 0x7c00
call write_hello  # This is relative? - jumps to 0x5ec0
jmp .

.data
our_string: .asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"

.org 0x200

# DONE:
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
