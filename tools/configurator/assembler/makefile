.DEFAULT_GOAL := test


###############################################################################
# Configuration options for our application
TARGET := light_program_assembler.js
SOURCE_DIRS := .
BUILD_DIR = build
Y_SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.y))


###############################################################################
# Toolchain setup
YACC := jison
MKDIR_P = mkdir -p
TEST_RUNNER = test/run-tests.sh
NPM = npm
DISASSEMBLER = ../disassembler/test/test-disassembler.js


###############################################################################
# Pretty-print setup
V ?= $(VERBOSE)
ifneq ($(V), 1)
QUIET := @
ECHO := @echo
else
QUIET :=
ECHO := @true
endif


###############################################################################
# Compiler and linker flags
YFLAGS :=


###############################################################################
# Plumbing for rules
vpath %.c $(SOURCE_DIRS)

$(shell $(MKDIR_P) $(BUILD_DIR))   # Always create the build directory

define compile-objects



$1/%.js: %.y scanner.l
	$(ECHO) [YACC] $$<
	$(QUIET) $(YACC) $(YFLAGS) -o $$@ $$< scanner.l

endef

$(foreach bdir, $(BUILD_DIR), $(eval $(call compile-objects,$(bdir))))


###############################################################################
# Target and object file setup
TARGET_BIN := $(TARGET)

OBJECTS += $(patsubst %.y, $(BUILD_DIR)/%.js, $(Y_SOURCES))


###############################################################################
# Rules
all : $(TARGET_BIN)

$(TARGET_BIN): $(OBJECTS)
	$(QUIET) $(NPM) install

run: $(TARGET_BIN)
	$(ECHO) [RUN] $<
	$(QUIET) ./$< $(RUN_OPTIONS)

test: $(TARGET_BIN)
	$(ECHO) [TEST] $<
	$(QUIET) $(TEST_RUNNER) ./$<

test-reassembly: $(TARGET_BIN)
	$(ECHO) [TEST ASM-DASM-ASM] $<
	$(QUIET) test/run-test-reassembly.sh ./$< $(DISASSEMBLER)


# Clean all generated files
clean:
	$(ECHO) [RM] $(BUILD_DIR)
	$(QUIET) $(RM) -rf $(BUILD_DIR)/*


.PHONY : all clean run test
