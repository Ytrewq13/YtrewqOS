_CC_BIN = clang
_LD_BIN = ld.lld
_OBJCOPY_BIN = llvm-objcopy
_VM_BIN = qemu-system-aarch64

SRCDIR = src
OUTDIR = build
BINDIR = $(OUTDIR)/bin
OBJDIR = $(OUTDIR)/obj
INCDIR = include
LDDIR = $(SRCDIR)/tools/

CCDB = compile_commands.json
TAGS = tags

KERNEL_ELF = $(BINDIR)/kernel8.elf
KERNEL_IMG = $(BINDIR)/kernel8.img
LINK_SCRIPT = $(LDDIR)/linker.ld

CC = $(_CC_BIN) --target=aarch64-elf
LD = $(_LD_BIN) -m aarch64elf -nostdlib
OBJCOPY = $(_OBJCOPY_BIN) -O binary
VM = $(_VM_BIN) -M raspi3 -serial stdio

CFLAGS = -I$(INCDIR) -Wall -O0 -ffreestanding -nostdlib -mcpu=cortex-a53+nosimd

# The source files (relative to ./src/)
_SRCS_ASM = boot/boot.S
_SRCS_C = kernel/kernel.c lib/framebuf.c lib/io/printf.c lib/mbox.c lib/peripherals/GPIO/miniUART.c lib/peripherals/GPIO/uart_PL011.c
# Header files (relative to ./include/)
_DEPS_H = error_types.h framebuf.h gpio.h io/printf.h mbox.h mem.h mmio.h uart.h

SRCS = $(addprefix $(SRCDIR)/,$(_SRCS_ASM) $(_SRCS_C))
OBJ_ASM = $(patsubst %.S,$(OBJDIR)/%.o,$(_SRCS_ASM))
OBJ_C = $(patsubst %.c,$(OBJDIR)/%.o,$(_SRCS_C))
OBJ = $(OBJ_ASM) $(OBJ_C)
DEPS = $(addprefix $(INCDIR)/,$(_DEPS_H))

# All the directories that the object files will be placed into
OBJDIRS = $(sort $(dir $(OBJ)))


# Where make searches for named files
VPATH = $(SRCDIR) $(INCDIR) $(OBJDIR) $(BINDIR)
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.S $(SRCDIR)
vpath %.o $(OBJDIR)



all: $(KERNEL_IMG)

dbg:
	@echo \$$\(SRCS\): $(SRCS)
	@echo \$$\(OBJ\):  $(OBJ)
	@echo \$$\(OBJDIRS\): $(OBJDIRS)
	@echo \$$\(DEPS\): $(DEPS)


# Rule for the tags file
tags:
	@ctags -o $(TAGS) -R $(SRCDIR) $(INCDIR)

# Rule to make compile_commands.json (note: runs make)
$(CCDB): fullclean
	bear --output $@ -- make
	$(MAKE) distclean

# Rule to make build directories if they don't exist
$(OUTDIR) $(OBJDIRS) $(BINDIR):
	@mkdir -p $@

setup: $(OUTDIR) $(OBJDIRS) $(BINDIR)

# Rule for turning ASM source files into object files
$(OBJ_ASM): $(OBJDIR)/%.o: $(SRCDIR)/%.S $(DEPS) | setup
	@echo "CC [ASM] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C source files into object files
$(OBJ_C): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS) | setup
	@echo "CC [ C ] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for creating the .elf of the kernel
$(KERNEL_ELF): $(OBJ) | $(BINDIR)
	@echo "LD       $@"
	@$(LD) $(OBJ) -T $(LINK_SCRIPT) -o $(KERNEL_ELF)

# Rule for creating the kernel image
$(KERNEL_IMG): $(KERNEL_ELF)
	@echo "Objcopy  $@"
	@$(OBJCOPY) $(KERNEL_ELF) $(KERNEL_IMG)

# Clean all intermediate files
clean:
	@rm -f $(KERNEL_ELF)
	@rm -rf $(OBJDIR)

# Clean ALL files (including the kernel image)
distclean: clean
	@rm -f $(KERNEL_IMG)
	@rm -df $(BINDIR)
	@rm -df $(OUTDIR)

# Reset dir tree to git repo state
reset: fullclean
	@rm -f $(CCDB)
	@rm -f $(TAGS)

# Run the VM with the generated kernel
run: $(KERNEL_IMG)
	$(VM) -kernel $(KERNEL_IMG)

.PHONY: clean distclean fullclean run kernel setup tags
