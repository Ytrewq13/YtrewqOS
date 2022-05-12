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
export TOOLSDIR = tools
LDDIR = $(TOOLSDIR)/link

CCDB = compile_commands.json
CFLAGS_FILE = compile_flags.txt
TAGS = tags

KERNEL_ELF = $(BINDIR)/kernel8.elf
KERNEL_IMG = $(BINDIR)/kernel8.img
SD_IMG = $(BINDIR)/sd.img
SKEL_DIR = $(TOOLSDIR)/fat/skel
FORMAT_FS = $(TOOLSDIR)/fat/setup-fs.sh
TEMPLATE_IMG = $(BINDIR)/template.img
LINK_SCRIPT = $(LDDIR)/linker.ld
GDB_CMDFILE = $(TOOLSDIR)/debug/commands.gdb
DEBUG_PIDFILE = $(BINDIR)/vm.pid

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
	CF_DBG = -g -DDEBUG
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
			calls/calls.S \
			calls/exceptions.S
_SRCS_C = calls/calls.c \
		  drivers/GPIO/miniUART.c \
		  drivers/GPIO/uart_PL011.c \
		  drivers/SD/emmc.c \
		  drivers/graphics/console.c \
		  drivers/graphics/fb_pixels.c \
		  drivers/power/power.c \
		  drivers/mbox.c \
		  drivers/mmio.c \
		  fonts/bizcat_font.c \
		  fs/fat.c \
		  fs/util.c \
		  kernel/framebuf.c \
		  kernel/main.c \
		  kernel/printf.c \
		  kernel/shell.c \
		  kernel/sys.c \
		  lib/io/printf.c \
		  libc/stdlib.c \
		  libc/string.c \
		  mm/heap.c \
		  mm/malloc.c \
		  test/main.c \
		  test/tests_emmc.c \
		  test/tests_examples.c \
		  test/tests_malloc.c \
		  kernel/timer.c
export _SRCS_ASM
export _SRCS_C
# Header files (relative to ./include/)
# TODO: automatically track each object file's dependencies
_DEPS_H = asm/errno.h \
		  asm/syscall.h \
		  asm/sysregs.h \
		  config.def.h \
		  errno.h \
		  error_types.h \
		  fonts/bizcat_font.h \
		  drivers/framebuf.h \
		  drivers/graphics/console.h \
		  drivers/graphics/fb_pixels.h \
		  drivers/hw/eMMC.h \
		  drivers/hw/gpio.h \
		  drivers/hw/mbox.h \
		  drivers/hw/mem.h \
		  drivers/hw/mmio.h \
		  drivers/hw/power.h \
		  drivers/hw/uart.h \
		  fs/block.h \
		  fs/dirent.h \
		  fs/fat.h \
		  fs/file.h \
		  fs/fs.h \
		  fs/util.h \
		  fs/vfs.h \
		  io/printf.h \
		  kernel/shell.h \
		  kernel/test.h \
		  kernel/timer.h \
		  kernel/util.h \
		  printf.h \
		  process/env.h \
		  ptrace.h \
		  stdlib.h \
		  string.h \
		  syscall.h \
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


ifndef KERNEL_ELF
	$(error KERNEL_ELF is undefined)
endif
ifndef DEBUG_PIDFILE
	$(error DEBUG_PIDFILE is undefined)
endif
ifndef OBJDIR
	$(error OBJDIR is undefined)
endif
ifndef KERNEL_IMG
	$(error KERNEL_IMG is undefined)
endif
ifndef SD_IMG
	$(error SD_IMG is undefined)
endif
ifndef BINDIR
	$(error BINDIR is undefined)
endif
ifndef OUTDIR
	$(error OUTDIR is undefined)
endif
ifndef CCDB
	$(error CCDB is undefined)
endif
ifndef TAGS
	$(error TAGS is undefined)
endif


all: $(KERNEL_IMG)

#dbg:
#	@echo \$$\(SRCS\): $(SRCS)
#	@echo \$$\(OBJ\):  $(OBJ)
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

# Create the SD card image and format it to exFAT
$(SD_IMG): $(TEMPLATE_IMG)
	@echo "CP       $@"
	@cp "$(TEMPLATE_IMG)" "$(SD_IMG)"

$(TEMPLATE_IMG): $(BINDIR)
	@echo "IMG      $@"
	@qemu-img create -f raw -q "$@" 1024M
	@mkfs.exfat "$@" &> /dev/null
	@exfatlabel "$@" "mysdcard"
	@echo "CP       $@"
	@sh "$(FORMAT_FS)" "$(SKEL_DIR)" "$(TEMPLATE_IMG)"

# Clean all intermediate files
clean:
	@rm -f  $(KERNEL_ELF)
	@rm -f  $(DEBUG_PIDFILE)
	@rm -rf $(OBJDIR)

# Clean ALL files (including the kernel image)
distclean: clean
	@rm -f  $(KERNEL_IMG)
	@rm -f  $(SD_IMG)
	@rm -f  $(TEMPLATE_IMG)
	@rm -df $(BINDIR)
	@rm -df $(OUTDIR)

# Reset dir tree to git repo state
reset: distclean
	@rm -f $(CCDB)
	@rm -f $(TAGS)

sd: $(BINDIR) $(TEMPLATE_IMG)
	@echo "CP       $(SD_IMG)"
	@rm -f $(SD_IMG)
	@cp $(TEMPLATE_IMG) $(SD_IMG)

$(OUTDIR) $(BINDIR) $(OBJDIR):
	mkdir -p $@

# Run the VM with the generated kernel
run: $(KERNEL_IMG) $(SD_IMG)
	$(VM) -kernel "$(KERNEL_IMG)" -drive file="$(SD_IMG)",if=sd,format=raw

# $(DEBUG_PIDFILE) *MUST* BE THE FIRST PREREQUISITE
debug: $(DEBUG_PIDFILE) $(KERNEL_ELF) $(GDB_CMDFILE)
	$(GDB) -command='$(GDB_CMDFILE)' '$(KERNEL_ELF)'
	@echo "KILL     VM process (PID `cat $<`)"
	-@kill `cat $<`

$(DEBUG_PIDFILE): $(KERNEL_IMG) $(SD_IMG)
	{ $(VM_DBG) -kernel '$(KERNEL_IMG)' -drive file='$(SD_IMG)',if=sd,format=raw -pidfile '$@' & }

.PHONY: tags clean distclean reset sd run debug
