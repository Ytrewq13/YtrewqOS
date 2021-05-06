# Simple boot sector
# Run this by compiling it:
# $ as -o boot.o boot.s
# $ ld -Ttext 0 -e _start -o boot.elf boot.o
# $ objcopy -O binary boot.elf boot.bin
# Then run it on an x86 64 bit machine:
# $ qemu-system-x86_64 boot.bin
.code16

.text
.global _start
.type _start, @function
_start:
mov $hello, %eax
call write

# This Works
# TODO:
# - Put some compiled C code in the second sector (and after)
# - Switch into 32/64 bit mode
# - Leave real mode
# - Jump into the Kernel (which is in sector 2)
#   - Where would the entry point be?
load_sector:
mov $0x02, %ah  # Select 'Drive read' function
mov $0x6000, %bx  # Destination to write to   (TODO: choose an address with plenty of free space - 0x7c00 is where sector 1 is placed)
mov $1, %al  # Number of sectors to read
mov $0x80, %dl  # Drive ID
mov $2, %cl  # Sector to start from
mov $0, %dh  # Head to read from
mov $0, %ch  # Cylinder to read from
int $0x13
jc failed  # Clear flag is set if we failed
mov $0x6000, %eax  # At the moment, Sector 2 contains only a string for printing
call write  # Once we put the kernel in sector 2+, we will jump into the code
jmp .
failed:
mov $'N', %al
call putc
jmp .

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

total:
.org 0x1fe  # Fill with zeros so that the next byte is at 0x1fe (510)
.byte 0x55, 0xaa  # Finished filling the boot sector

.asciz "AAAAAAAAAA\n"
.org 0x400, 65 # A sector of all 'A'
