.code16
.intel_syntax noprefix

# Data about sector reads (for bootstrapping to the C kernel)
# TODO: choose an address with plenty of free space - 0x7c00 is where sector 1
# is placed
# The stack begins at 0x6f00 (grows downwards)
.equ BOOT_LD_ADDR, 0x7c00
.global LD_ADDR
.equ LD_ADDR, 0x7e00
.equ DRV_ID, 0x80

.text
.global _start
.type _start, @function
_start:
lea eax, hello
call write
# Load the second sector (should load the rest of the bootloader)
mov al, 0x01  # Number of sectors to read
mov bx, LD_ADDR  # Destination to write to
mov ch, 0x00  # Cylinder to read from
mov cl, 0x02  # Sector to start from
mov dh, 0x00  # Head to read from
mov dl, DRV_ID  # Drive ID
push dx
push cx
push bx
push ax
call load_sector
call sec2_func # Calling a function that is in the second sector
jmp .


# A function that loads sectors from disk:
# Takes 4 arguments, which are on the stack like so:
# ax - High byte doesn't matter,
#      low byte is the number of sectors to read
# bx - The address to load the data at
# cx - High byte is the cylinder number to read from,
#      low byte is the start sector (1-indexed)
# dx - High byte is the head to read from,
#      low byte is the drive ID (usually $0x80)
# Push to the stack in reverse order
load_sector:
    push ebp      /* Save the old base pointer value. */
    mov ebp, esp /* Set the new base pointer value. */
    push ax
    push bx
    push cx
    push dx
    mov dx, [ebp + 2*6] # FIXME: Does this work consistently?
    mov cx, [ebp + 2*5]
    mov bx, [ebp + 2*4]
    mov ax, [ebp + 2*3]
mov ah, 0x02  # Select 'Drive read' function
int 0x13 # Execute the drive services interrupt
jnc load_sector_end  # Carry flag is set if we failed
failed:
lea eax, failed_message
call write
jmp .
load_sector_end:
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov esp, ebp
    pop ebp
ret

nop
nop
nop
.global write_hello
.type write_hello, @function
write_hello:
/* Subroutine Prologue */
  push ebp      /* Save the old base pointer value. */
  mov ebp, esp /* Set the new base pointer value. */
  /* Save the values of registers that the function will modify. This function
   * uses EAX. */
  push eax
  /* (no need to save EBX, EBP, or ESP) */
lea eax, hello
call write
 /* Subroutine Epilogue */
  pop eax       /* Recover register values. */
  mov esp, ebp /* Deallocate the local variable. */
  pop ebp       /* Restore the caller's base pointer value. */
ret
nop
nop
nop


.global write
.type write, @function
write:
/* Subroutine Prologue */
push ebp      /* Save the old base pointer value. */
mov ebp, esp /* Set the new base pointer value. */
/* Save the values of registers that the function will modify. This function
 * uses EAX and EBX. */
push eax
push ebx
write_check:
    /* Subroutine code. */
    cmpb [eax], 0  # Strings are null-terminated
    je endwrite
    cmpb [eax], '\n'  # Compare string character with '\n'
    je write_newline
write_continue:
    movb bl, [eax]
    push eax
    movb al, bl
    call putc
    pop eax
    inc eax
    jmp write_check
write_newline:  # '\n' needs a '\r' before it
    push eax
    movb al, '\r'
    call putc
    pop eax
    jmp write_continue
endwrite:
 /* Subroutine Epilogue */
  pop ebx       /* Recover register values. */
  pop eax       /* Recover register values. */
  mov esp, ebp /* Deallocate the local variable. */
  pop ebp       /* Restore the caller's base pointer value. */
ret

# A function to write a character to the screen using the BIOS interrupt system
.global putc
.type putc, @function
putc:
    push ebp
    mov ebp, esp
    push eax
mov ah, 0x0e
int 0x10
    pop eax
    mov esp, ebp
    pop ebp
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
