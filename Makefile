all: compile

compile: link
	objcopy -O binary boot.elf boot.bin

link: assemble
	ld -Ttext 0x7c00 -e _start -o boot.elf boot.o

assemble: boot.s
	as -o boot.o boot.s

clean:
	rm -f boot.o boot.elf

full-clean: clean
	rm -f boot.bin

run: boot.bin
	qemu-system-x86_64 -drive format=raw,file=boot.bin

boot.bin: compile

# TODO:
# - Put magic filenames into variables
# - LINKER SCRIPT!!!
# - Create separate kernel in C
