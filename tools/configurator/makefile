
###############################################################################
# Configuration options for our application
TARGET := test
SOURCE_DIRS := .
BUILD_DIR = build
SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.c))
Y_SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.y))
L_SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.l))


###############################################################################
# Toolchain setup
BISON := bison
FLEX := flex
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
# Target and object file setup
SOURCES += $(patsubst %.y, $(BUILD_DIR)/%.tab.c, $(Y_SOURCES))
SOURCES += $(patsubst %.l, $(BUILD_DIR)/%.yy.c, $(L_SOURCES))
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET))

###############################################################################
# Compiler and linker flags
BISONFLAGS := -Werror -Wall -t -v --feature=caret

FLEXFLAGS :=

CFLAGS += -std=c99
CFLAGS += $(foreach sdir, $(SOURCE_DIRS), -I$(sdir)) -I$(BUILD_DIR)
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wstrict-prototypes -Wshadow -Wwrite-strings
CFLAGS += -Wdeclaration-after-statement -Waddress -Wlogical-op
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wmissing-field-initializers -Wdouble-promotion -Wfloat-equal
CFLAGS += -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas
CFLAGS += -Wundef
CFLAGS += -Os


###############################################################################
# Plumbing for rules
vpath %.c $(SOURCE_DIRS)

$(shell $(MKDIR_P) $(BUILD_DIR))   # Always create the build directory

define compile-objects
$1/%.tab.c: %.y
	$(ECHO) [BISON] $$<
	$(QUIET) $(BISON) $(BISONFLAGS) -o $$@ $$<

$1/%.yy.c: %.l
	$(ECHO) [FLEX] $$<
	$(QUIET) $(FLEX) $(FLEXFLAGS) -o $$@ $$<
endef

$(foreach bdir, $(BUILD_DIR), $(eval $(call compile-objects,$(bdir))))


###############################################################################
# Rules
all : $(TARGET_BIN)

$(TARGET_BIN): $(SOURCES)
	$(ECHO) [CC] $^
	$(QUIET) $(CC) $(CFLAGS) $^ -o $@

run: $(TARGET_BIN)
	$(ECHO) [RUN] $<
	$(QUIET) $<

test: $(TARGET_BIN)
	$(ECHO) [TEST] $<
	$(QUIET) test/run-tests.sh $<

# Clean all generated files
clean:
	$(ECHO) [RM] $(BUILD_DIR)
	$(QUIET) $(RM) -rf $(BUILD_DIR)


.PHONY : all clean run test
