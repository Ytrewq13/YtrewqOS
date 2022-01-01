_CC_BIN = clang
_LD_BIN = ld.lld
_OBJCOPY_BIN = llvm-objcopy
_VM_BIN = qemu-system-aarch64
_GDB_BIN = gdb-multiarch

DEBUG = 1

export SRCDIR = src
export OUTDIR = build
export BINDIR = $(OUTDIR)/bin
export OBJDIR = $(OUTDIR)/obj
export INCDIR = include
export TOOLSDIR = $(SRCDIR)/tools
LDDIR = $(SRCDIR)/link

CCDB = compile_commands.json
CFLAGS_FILE = compile_flags.txt
TAGS = tags

KERNEL_ELF = $(BINDIR)/kernel8.elf
KERNEL_IMG = $(BINDIR)/kernel8.img
SD_IMG = $(BINDIR)/sd.img
LINK_SCRIPT = $(LDDIR)/linker.ld
GDB_CMDFILE = $(TOOLSDIR)/commands.gdb
DEBUG_PIDFILE = $(BINDIR)/vm.PID

SUB_MAKE_MAKEFILE = $(CURDIR)/submake.mk

export CC = $(_CC_BIN) --target=aarch64-elf -mcpu=cortex-a53
LD = $(_LD_BIN) -m aarch64elf -nostdlib
OBJCOPY = $(_OBJCOPY_BIN) -O binary
VM = $(_VM_BIN) -machine raspi3b -serial stdio
VM_DBG = $(VM) -s -S
GDB = $(_GDB_BIN)

# Absolute versions of directories
export SRCDIR_ABS = $(CURDIR)/$(SRCDIR)
export OUTDIR_ABS = $(CURDIR)/$(OUTDIR)
export BINDIR_ABS = $(CURDIR)/$(BINDIR)
export OBJDIR_ABS = $(CURDIR)/$(OBJDIR)
export INCDIR_ABS = $(CURDIR)/$(INCDIR)

# Construct the value for CFLAGS
CF_INCLUDE = -I$(INCDIR_ABS)
CF_RESTRICT = -std=c99 -ffreestanding -nostdlib
ifeq ($(DEBUG), 1)
	CF_DBG = -g
	CF_OPTIM = -O0
#	CF_WARN = -Wpedantic -Werror  # TODO: enum is int, not long
	CF_WARN = -Wall
else
	CF_DBG =
	CF_OPTIM = -O2
	CF_WARN = -Wall
endif
CFLAGS += $(CF_DBG) $(CF_INCLUDE) $(CF_WARN) $(CF_OPTIM) $(CF_RESTRICT)
export CFLAGS

# The source files (relative to ./src/)
_SRCS_ASM = boot/boot.S \
			kernel/calls/calls.S \
			kernel/calls/exceptions.S
_SRCS_C = drivers/GPIO/miniUART.c \
		  drivers/GPIO/uart_PL011.c \
		  drivers/SD/emmc.c \
		  drivers/graphics/console.c \
		  drivers/graphics/fb_pixels.c \
		  fonts/bizcat_font.c \
		  fs/fat.c \
		  kernel/calls/calls.c \
		  kernel/framebuf.c \
		  kernel/heap.c \
		  kernel/main.c \
		  kernel/malloc.c \
		  kernel/mbox.c \
		  kernel/printf.c \
		  kernel/test/main.c \
		  kernel/test/tests_examples.c \
		  kernel/timer.c \
		  lib/io/printf.c \
		  libc/stdlib.c \
		  libc/string.c
export _SRCS_ASM
export _SRCS_C
# Header files (relative to ./include/)
# TODO: automatically track each object file's dependencies
_DEPS_H = errno.h \
		  error_types.h \
		  fonts/bizcat_font.h \
		  framebuf.h \
		  fs/block.h \
		  fs/dirent.h \
		  fs/fat.h \
		  fs/file.h \
		  fs/fs.h \
		  fs/vfs.h \
		  graphics/console.h \
		  graphics/fb_pixels.h \
		  hw/eMMC.h \
		  hw/gpio.h \
		  hw/mbox.h \
		  hw/mem.h \
		  hw/mmio.h \
		  hw/uart.h \
		  io/printf.h \
		  kernel/test.h \
		  kernel/timer.h \
		  kernel/util.h \
		  stdlib.h \
		  string.h \
		  printf.h \
		  unistd.h

# Variables used by sub-make
export TOPDIR = $(CURDIR)

SRCS = $(addprefix $(SRCDIR)/,$(_SRCS_ASM) $(_SRCS_C))
OBJ_ASM = $(patsubst %,$(OBJDIR)/%.o,$(_SRCS_ASM))
OBJ_C = $(patsubst %,$(OBJDIR)/%.o,$(_SRCS_C))
OBJ = $(OBJ_ASM) $(OBJ_C)
DEPS = $(addprefix $(INCDIR)/,$(_DEPS_H))
export DEPS_ABS = $(addprefix $(CURDIR)/,$(DEPS))

# Where make searches for named files
VPATH = $(SRCDIR) $(INCDIR) $(OBJDIR) $(BINDIR)
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.S $(SRCDIR)
vpath %.o $(OBJDIR)



all: $(KERNEL_IMG)

#dbg:
#	@echo \$$\(SRCS\): $(SRCS)
#	@echo \$$\(OBJ\):  $(OBJ)
#	@echo \$$\(OBJDIRS\): $(OBJDIRS)
#	@echo \$$\(DEPS\): $(DEPS)


# Rule for the tags file
tags:
	@echo "[TAGS]"
	@ctags -o $(TAGS) -R $(SRCDIR) $(INCDIR)

# Rule to make compile_commands.json (note: runs make)
$(CCDB): distclean
	bear --output $@ -- make -j`nproc`
	$(MAKE) distclean

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/% $(DEPS)
	@$(MAKE) \
		--makefile='$(SUB_MAKE_MAKEFILE)' \
		--no-print-directory \
		-C $(shell echo $< | grep -o '$(SRCDIR)/[^/]*/') \
		$(abspath $@)

# Rule for creating the .elf of the kernel
$(KERNEL_ELF): $(OBJ) | $(BINDIR)
	@echo "LD       $@"
	@$(LD) $(OBJ) -T $(LINK_SCRIPT) -o $(KERNEL_ELF)

# Rule for creating the kernel image
$(KERNEL_IMG): $(KERNEL_ELF)
	@echo "Objcopy  $@"
	@$(OBJCOPY) $(KERNEL_ELF) $(KERNEL_IMG)

$(SD_IMG): $(BINDIR)
	@echo "DD       $@"
	@dd if=/dev/zero of=$@ bs=1M count=1024

# Clean all intermediate files
clean:
	@rm -f  $(KERNEL_ELF)
	@rm -f  $(DEBUG_PIDFILE)
	@rm -rf $(OBJDIR)

# Clean ALL files (including the kernel image)
distclean: clean
	@rm -f  $(KERNEL_IMG)
	@rm -f  $(SD_IMG)
	@rm -df $(BINDIR)
	@rm -df $(OUTDIR)

# Reset dir tree to git repo state
reset: distclean
	@rm -f $(CCDB)
	@rm -f $(TAGS)

# Run the VM with the generated kernel
run: $(KERNEL_IMG) $(SD_IMG)
	$(VM) -kernel "$(KERNEL_IMG)" -drive file="$(SD_IMG)",if=sd,format=raw

# $(DEBUG_PIDFILE) *MUST* BE THE FIRST PREREQUISITE
debug: $(DEBUG_PIDFILE) $(KERNEL_ELF) $(GDB_CMDFILE)
	$(GDB) -command='$(GDB_CMDFILE)' '$(KERNEL_ELF)'
	@echo "KILL     VM process (PID `cat $<`)"
	-@kill `cat $<`
	@echo "RM       VM PID file '$<'"
	@rm $<

$(DEBUG_PIDFILE): $(KERNEL_IMG) $(SD_IMG)
	{ $(VM_DBG) -kernel '$(KERNEL_IMG)' -drive file='$(SD_IMG)',if=sd,format=raw & echo $$! > $@; }

.PHONY: tags clean distclean reset run debug
