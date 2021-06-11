.code16
.intel_syntax noprefix

.text
home2:
call write_hello
jmp .

# A function in the 2nd sector
.global sec2_func
.type sec2_func, @function
sec2_func:
    push ebp
    mov ebp, esp
    push eax
    # Function body start
lea eax, our_string
call write
#call write_hello
    # Function body end
    pop eax
    mov esp, ebp
    pop ebp
ret
# sec2_func end

.data
our_string: .asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"

.org 0x200

# DONE:
# - Call a function in the boot sector from here
# - Call a function here from the boot sector
# TODO:
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
