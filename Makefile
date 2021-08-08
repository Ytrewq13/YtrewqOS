_CC_BIN = clang
_LD_BIN = ld.lld
_OBJCOPY_BIN = llvm-objcopy
_VM_BIN = qemu-system-aarch64

SRCDIR = src
OUTDIR = bin
OBJDIR = $(OUTDIR)/obj
INCDIR = include
LDDIR = $(SRCDIR)/tools/

KERNEL_ELF = $(OUTDIR)/kernel8.elf
KERNEL_IMG = $(OUTDIR)/kernel8.img
LINK_SCRIPT = $(LDDIR)/linker.ld

CC = $(_CC_BIN) --target=aarch64-elf
LD = $(_LD_BIN) -m aarch64elf -nostdlib
OBJCOPY = $(_OBJCOPY_BIN) -O binary
VM = $(_VM_BIN) -M raspi3 -serial null -serial stdio

CFLAGS = -I$(INCDIR) -Wall -O2 -ffreestanding -nostdlib -mcpu=cortex-a53+nosimd

# The source files (relative to ./src/)
_SRCS_ASM = boot/boot.S
_SRCS_C = kernel/kernel.c lib/framebuf.c lib/io/printf.c lib/mbox.c lib/uart.c
# Header files (relative to ./include/)
_DEPS_H = error_types.h framebuf.h gpio.h io/printf.h mbox.h mem.h mmio.h uart.h

SRCS = $(patsubst %,$(SRCDIR)/%,$(_SRCS_ASM) $(_SRCS_C))
OBJ_ASM = $(patsubst %.S,$(OBJDIR)/%.o,$(_SRCS_ASM))
OBJ_C = $(patsubst %.c,$(OBJDIR)/%.o,$(_SRCS_C))
OBJ = $(OBJ_ASM) $(OBJ_C)
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS_H))


# Where make searches for named files
VPATH = $(SRCDIR) $(INCDIR) $(OBJDIR) $(OUTDIR)



all: $(KERNEL_IMG)

dbg:
	@echo \$$\(SRCS\): $(SRCS)
	@echo \$$\(OBJ\):  $(OBJ)
	@echo \$$\(DEPS\): $(DEPS)

setup:
	@dirname $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS)) | xargs mkdir -p

# Rule for turning ASM source files into object files
$(OBJ_ASM): $(OBJDIR)/%.o: $(SRCDIR)/%.S $(DEPS) setup
	@echo "Building $@..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C source files into object files
$(OBJ_C): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS) setup
	@echo "Building $@..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for creating the .elf of the kernel
$(KERNEL_ELF): $(OBJ)
	@echo "Linking $@..."
	@$(LD) $(OBJ) -T $(LINK_SCRIPT) -o $(KERNEL_ELF)

# Rule for creating the kernel image
$(KERNEL_IMG): $(KERNEL_ELF)
	@echo "Objcopy-ing $@..."
	@$(OBJCOPY) $(KERNEL_ELF) $(KERNEL_IMG)

# Clean all intermediate files
clean:
	@rm -f $(KERNEL_ELF) $(OBJ)

# Clean ALL files (including the kernel image)
distclean: clean
	@rm -rf $(OBJDIR) $(KERNEL_IMG)

# Run the VM with the generated kernel
run: $(KERNEL_IMG)
	$(VM) -kernel $(KERNEL_IMG)

.PHONY: clean run kernel setup
