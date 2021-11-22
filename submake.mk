# The current directory relative to the project root
CURDIR_REL = $(subst $(TOPDIR)/,,$(CURDIR))
CURDIR_SRC_REL = $(subst $(SRCDIR)/,,$(CURDIR_REL))

# Get the objdir this submake will use
OBJDIR_ABS = $(addprefix $(TOPDIR)/,$(addsuffix $(subst $(SRCDIR),,$(CURDIR_REL)),$(OBJDIR)))

# Source files
SRCS_ASM = $(subst $(CURDIR_SRC_REL)/,,$(filter $(CURDIR_SRC_REL)/%,$(_SRCS_ASM)))
SRCS_C = $(subst $(CURDIR_SRC_REL)/,,$(filter $(CURDIR_SRC_REL)/%,$(_SRCS_C)))
SRCS = $(SRCS_ASM) $(SRCS_C)
# TODO: which header files do we actually depend on?

# Object files we will generate
OBJ_ASM = $(patsubst %,$(OBJDIR_ABS)/%.o,$(SRCS_ASM))
OBJ_C = $(patsubst %,$(OBJDIR_ABS)/%.o,$(SRCS_C))
OBJ = $(OBJ_ASM) $(OBJ_C)

OBJDIRS = $(sort $(dir $(OBJ)))

all: $(OBJ)

# Rule to make build directories if they don't exist
$(OUTDIR_ABS) $(OBJDIRS) $(BINDIR_ABS):
	@mkdir -p $@

setup: $(OUTDIR_ABS) $(OBJDIRS) $(BINDIR_ABS)

# Rule for turning ASM source files into object files
$(OBJ_ASM): $(OBJDIR_ABS)/%.o: ./% $(DEPS_ABS) | setup
	@echo "CC [ASM] $(subst $(TOPDIR)/$(OBJDIR)/,,$@)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C source files into object files
$(OBJ_C): $(OBJDIR_ABS)/%.o: ./% $(DEPS_ABS) | setup
	@echo "CC [ C ] $(subst $(TOPDIR)/$(OBJDIR)/,,$@)"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all setup
