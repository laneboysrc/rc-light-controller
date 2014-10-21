.DEFAULT_GOAL := all


###############################################################################
# Configuration options for our application
TARGET := light_program_assembler
SOURCE_DIRS := .
BUILD_DIR = build
SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.c))
Y_SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.y))
L_SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.l))


###############################################################################
# Toolchain setup
CC = gcc
LD = gcc
YACC := bison
LEX := flex
MKDIR_P = mkdir -p
TEST_RUNNER = test/run-tests.sh


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
YFLAGS := -Werror -Wall --debug --verbose --feature=caret

CFLAGS += -std=c99
CFLAGS += $(foreach sdir, $(SOURCE_DIRS), -I$(sdir)) -I$(BUILD_DIR)
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wstrict-prototypes -Wshadow -Wwrite-strings
CFLAGS += -Wdeclaration-after-statement -Waddress -Wlogical-op
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wmissing-field-initializers -Wdouble-promotion -Wfloat-equal
CFLAGS += -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas
CFLAGS += -Wundef
CFLAGS += -O2

# For the flex/bison output we use less different CFLAGS as otherwise we get
# a ton of unresolvable warnings
TOOL_CFLAGS = -std=c99 -D_POSIX_SOURCE
TOOL_CFLAGS += $(foreach sdir, $(SOURCE_DIRS), -I$(sdir)) -I$(BUILD_DIR)
TOOL_CFLAGS += -O2

LDFLAGS :=
LDLIBS :=

###############################################################################
# Plumbing for rules
vpath %.c $(SOURCE_DIRS)

$(shell $(MKDIR_P) $(BUILD_DIR))   # Always create the build directory

define compile-objects

$1/%.o: $1/%.c
	$(ECHO) [CC] $$<
	$(QUIET) $(CC) -c $(TOOL_CFLAGS) $$< -o $$@

$1/%.o: %.c
	$(ECHO) [CC] $$<
	$(QUIET) $(CC) -c $(CFLAGS)  $$< -o $$@


$1/%.c: %.y
	$(ECHO) [YACC] $$<
	$(QUIET) $(YACC) $(YFLAGS) -o $$@ $$<

$1/%.c: %.l
	$(ECHO) [LEX] $$<
	$(QUIET) $(LEX) --header-file=$$(@:.c=.h) -o $$@  $$<
endef

$(foreach bdir, $(BUILD_DIR), $(eval $(call compile-objects,$(bdir))))


###############################################################################
# Target and object file setup
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET))

OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS += $(patsubst %.l, $(BUILD_DIR)/%.o, $(L_SOURCES))
OBJECTS += $(patsubst %.y, $(BUILD_DIR)/%.o, $(Y_SOURCES))

$(OBJECTS): $(patsubst %.y, $(BUILD_DIR)/%.c, $(Y_SOURCES))
$(OBJECTS): $(patsubst %.l, $(BUILD_DIR)/%.c, $(L_SOURCES))


###############################################################################
# Rules
all : $(TARGET_BIN)

$(TARGET_BIN): $(OBJECTS)
	$(ECHO) [LD] $@
	$(QUIET) $(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

run: $(TARGET_BIN)
	$(ECHO) [RUN] $<
	$(QUIET) $<

test: $(TARGET_BIN)
	$(ECHO) [TEST] $<
	$(QUIET) test/run-tests.sh $<

# Clean all generated files
clean:
	$(ECHO) [RM] $(BUILD_DIR)
	$(QUIET) $(RM) -rf $(BUILD_DIR)/*


.PHONY : all clean run test
