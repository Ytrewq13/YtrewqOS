# Simple boot sector
# Run this by compiling it:
# $ as -o boot.o boot.s
# $ ld -Ttext 0 -e _start -o boot.elf boot.o
# $ objcopy -O binary boot.elf boot.bin
# Then run it on an x86 64 bit machine:
# $ qemu-system-x86_64 boot.bin
.code16

# Data about sector reads (for bootstrapping to the C kernel)
# TODO: choose an address with plenty of free space - 0x7c00 is where sector 1
# is placed
.equ KERN_LD_ADDR, 0x6000
.equ DRIVE_ID, 0x80

.text
.global _start
.type _start, @function
_start:
mov $hello, %eax
call write

# Load the second sector
# TODO: turn this into a function using the stack
load_sector:
mov $0x02, %ah  # Select 'Drive read' function
mov $KERN_LD_ADDR, %bx  # Destination to write to
mov $1, %al  # Number of sectors to read
mov $DRIVE_ID, %dl  # Drive ID
mov $2, %cl  # Sector to start from
mov $0, %dh  # Head to read from
mov $0, %ch  # Cylinder to read from
int $0x13
jc failed  # Clear flag is set if we failed
success:
mov $KERN_LD_ADDR, %eax  # At the moment, Sector 2 contains only a string for printing
call write  # Once we put the kernel in sector 2+, we will jump into the code
#jmp KERN_LD_ADDR # TODO: does this work?
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
  /* Save the values of registers that the function will modify. This function
   * uses EAX. */
  push %eax
  /* (no need to save EBX, EBP, or ESP) */
mov $hello, %eax
call write
 /* Subroutine Epilogue */
  pop %eax       /* Recover register values. */
  mov %ebp, %esp /* Deallocate the local variable. */
  pop %ebp       /* Restore the caller's base pointer value. */
ret
nop
nop
nop


.global write
.type write, @function
write:
/* Subroutine Prologue */
push %ebp      /* Save the old base pointer value. */
mov %esp, %ebp /* Set the new base pointer value. */
/* Save the values of registers that the function will modify. This function
 * uses EAX and EBX. */
push %eax
push %ebx
write_check:
    /* Subroutine code. */
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
    jmp write_check
write_newline:  # '\n' needs a '\r' before it
    push %eax
    movb $'\r', %al
    call putc
    pop %eax
    jmp write_continue
endwrite:
 /* Subroutine Epilogue */
  pop %ebx       /* Recover register values. */
  pop %eax       /* Recover register values. */
  mov %ebp, %esp /* Deallocate the local variable. */
  pop %ebp       /* Restore the caller's base pointer value. */
ret

# A function to write a character to the screen using the BIOS interrupt system
.global putc
.type putc, @function
putc:
    push %ebp
    mov %esp, %ebp
    push %eax
mov $0x0e, %ah
int $0x10
    pop %eax
    mov %ebp, %esp
    pop %ebp
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
