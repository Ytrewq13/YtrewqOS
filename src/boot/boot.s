# Simple boot sector
# Run this by compiling it:
# $ as -o boot.o boot.s
# $ ld -Ttext 0 -e _start -o boot.elf boot.o
# $ objcopy -O binary boot.elf boot.bin
# Then run it on an x86 64 bit machine:
# $ qemu-system-x86_64 boot.bin
.code16

.data
# Data about sector reads (for bootstrapping to the C kernel)
.equ KERN_LD_ADDR, 0x6000
.equ DRIVE_ID, 0x80

.text
.global _start
.type _start, @function
_start:
mov $hello, %eax
call write

# This Works
# TODO:
# - Put some compiled C code in the second sector (and after)
# - Switch into protected mode (32-bit)
#   - Disable interrupts, including NMI
#   - Enable the A20 line
#   - Load the GDT (Global Descriptor Table) with segment descriptors suitable
#     for code, data, and stack
#   - Change the lowest bit of the CR0/MSW register
# - Switch into long mode (64-bit)
# - Jump into the Kernel (which is in sector 2)
#   - Where would the entry point be?
load_sector:
mov $0x02, %ah  # Select 'Drive read' function
mov $KERN_LD_ADDR, %bx  # Destination to write to   (TODO: choose an address with plenty of free space - 0x7c00 is where sector 1 is placed)
mov $1, %al  # Number of sectors to read
mov $DRIVE_ID, %dl  # Drive ID
mov $2, %cl  # Sector to start from
mov $0, %dh  # Head to read from
mov $0, %ch  # Cylinder to read from
int $0x13
jc failed  # Clear flag is set if we failed
#mov KERN_LD_ADDR, %eax  # At the moment, Sector 2 contains only a string for printing
#call write  # Once we put the kernel in sector 2+, we will jump into the code
jmp KERN_LD_ADDR # TODO: does this work?
jmp .
failed:
mov $failed_message, %eax
call write
jmp .

nop
nop
nop
.global write_hello
.type write_hello, @function
write_hello:
/* Subroutine Prologue */
  push %ebp      /* Save the old base pointer value. */
  mov %esp, %ebp /* Set the new base pointer value. */
  push %edi      /* Save the values of registers that the function */
  push %esi      /* will modify. This function uses EDI and ESI. */
  /* (no need to save EBX, EBP, or ESP) */
mov $hello, %eax
call write
 /* Subroutine Epilogue */
  pop %esi       /* Recover register values. */
  pop %edi
  mov %ebp, %esp /* Deallocate the local variable. */
  pop %ebp       /* Restore the caller's base pointer value. */
ret
nop
nop
nop


# mov ah, 0x02             # Select 'Drive read' function
# mov bx, <destination>    # Destination to write to, in ES:BX
# mov al, <num_sectors>    # Number of sectors to read at a time
# mov dl, <drive_num>      # The external drive's ID
# mov cl, <start_sector>   # The sector to start reading from
# mov dh, <head>           # The head to read from
# mov ch, <cylinder>       # The cylinder to read from
# int 0x13                 # Drive services interrupt
# jc <error_handler>       # Jump to error handler on CF set

write:
cmpb $0, (%eax)  # Strings are null-terminated
je endwrite
cmpb $'\n', (%eax)  # Compare string character with '\n'
je write_newline
write_continue:
movb (%eax), %bl
push %eax
movb %bl, %al
call putc
pop %eax
inc %eax
jmp write
endwrite:
ret
write_newline:  # '\n' needs a '\r' before it
push %eax
movb $'\r', %al
call putc
pop %eax
jmp write_continue

putc:
mov $0x0e, %ah
int $0x10
ret

hello: .asciz "Hello World!\n"
failed_message: .asciz "Failed reading from Hard disk!\n"

# The Global Descriptor Table (For entering protected mode - this is discarded
# when we switch to real mode)
gdt:
gdt_null:
    .word 0x0000
    .word gdt
gdt_code:
    .word 0x0000        # Base 0:15
    .word 0xFFFF        # Limit 0:15
    .byte 0x00          # Base 24:31
    .byte 0b11001111    # Flags (Gr|Sz|XX|XX), Limit 16:19
    .byte 0b10011000    # Access byte  (Pr|Priv|S|Ex|DC|RW|Ac)
    .byte 0x00          # Base 16:23
gdt_end:

total:
.org 0x1fe, 0x90  # Fill with zeros so that the next byte is at 0x1fe (510)
.byte 0x55, 0xaa  # Finished filling the boot sector

#.asciz "AAAAAAAAAA\n"
#.org 0x400, 65 # A sector of all 'A'
