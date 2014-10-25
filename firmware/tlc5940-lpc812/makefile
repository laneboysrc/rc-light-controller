.DEFAULT_GOAL := all

###############################################################################
# Configuration options for our application
TARGET := firmware
SOURCE_DIRS := .
BUILD_DIR = build

SYSTEM_CLOCK := 12000000

SOURCES := $(foreach sdir, $(SOURCE_DIRS), $(wildcard $(sdir)/*.c))
DEPENDENCIES := makefile globals.h uart0.h utils.h
LIBS = gcc
LINKER_SCRIPT := light_controller.ld


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
# Toolchain setup
TOOLCHAIN_PREFIX := arm-none-eabi-
TOOLCHAIN_PATH := /opt/gcc-arm-none-eabi-4_8-2014q2/bin/
TOOL_PATH := ../../tools/

CC := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld
OBJCOPY := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy

MKDIR_P = mkdir -p
FLASH_TOOL := lpc81x_isp.py --run --flash
TERMINAL_PROGRAM := miniterm.py -p /dev/ttyUSB0 -b 115200
PREPROCESSOR_SIMULATOR := $(TOOL_PATH)preprocessor-simulator.py -b 115200
MAP_SUMMARY_TOOL := $(TOOL_PATH)parse_gcc_map_file.py
CONFIGURATION_VALIDATION_TOOL := $(TOOL_PATH)validate_image_file.py


###############################################################################
# Target and object file setup
OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET_ELF := $(addprefix $(BUILD_DIR)/, $(TARGET).elf)
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET).bin)
TARGET_HEX := $(addprefix $(BUILD_DIR)/, $(TARGET).hex)
TARGET_MAP := $(addprefix $(BUILD_DIR)/, $(TARGET).map)

$(OBJECTS): $(DEPENDENCIES)
$(TARGET_MAP): $(TARGET_ELF)


###############################################################################
# Compiler and linker flags
CFLAGS = -mthumb -mcpu=cortex-m0plus -mlittle-endian
CFLAGS += -std=c99
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wstrict-prototypes -Wshadow -Wwrite-strings
CFLAGS += -Wdeclaration-after-statement -Waddress -Wlogical-op
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wmissing-field-initializers -Wdouble-promotion -Wfloat-equal
CFLAGS += -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas
CFLAGS += -Wundef
CFLAGS += -I. -isystem./LPC8xx
CFLAGS += -fsigned-char -fdata-sections -ffunction-sections -fno-common
CFLAGS += -fpack-struct=4
CFLAGS += -Os
CFLAGS += -D__SYSTEM_CLOCK=$(SYSTEM_CLOCK)

LDFLAGS = -T $(LINKER_SCRIPT) -nostdlib --warn-common
LDFLAGS += --gc-sections -O3
LDFLAGS += -Map=$(TARGET_MAP) --cref
LDFLAGS += -L$(TOOLCHAIN_PATH)../lib/gcc/arm-none-eabi/4.8.4/armv6-m

LDLIBS = $(addprefix -l,$(LIBS))


###############################################################################
# Plumbing for rules
vpath %.c $(SOURCE_DIRS)

$(shell $(MKDIR_P) $(BUILD_DIR))   # Always create the build directory

define compile-objects
$1/%.o: %.c
	$(ECHO) [CC] $$<
	$(QUIET) $(CC) $(CFLAGS) -c $$< -o $$@

$1/%.lst: %.c
	$(ECHO) [LIST] $$<
	$(QUIET) $(CC) -c -g -Wa,-adlhn $(CFLAGS) $$< -o /dev/null > $$@
endef

$(foreach bdir, $(BUILD_DIR), $(eval $(call compile-objects,$(bdir))))


###############################################################################
# Rules
all : $(TARGET_BIN) $(TARGET_HEX)

$(TARGET_ELF): $(OBJECTS) $(LINKER_SCRIPT)
	$(ECHO) [LD] $@
	$(QUIET) $(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(TARGET_BIN): $(TARGET_ELF)
	$(ECHO) [CP] $@
	$(QUIET) $(OBJCOPY) $< -O binary $@
	$(QUIET) $(CONFIGURATION_VALIDATION_TOOL) $@

$(TARGET_HEX): $(TARGET_ELF)
	$(ECHO) [CP] $@
	$(QUIET) $(OBJCOPY) $< -O ihex $@

# Create list files that include C code as well as Assembler
list: $(OBJECTS:.o=.lst)

# Print a memory usage summary
summary: $(TARGET_MAP)
	$(QUIET) $(MAP_SUMMARY_TOOL) $<

# Invoke the tool to program the microcontroller
program: $(TARGET_BIN)
	$(QUIET )$(FLASH_TOOL) $<

# Invoke a tool for UART communication
terminal:
	$(QUIET) $(TERMINAL_PROGRAM)

# Invoke the preprocessor simulation tool
preprocessor-simulator:
	$(QUIET) $(PREPROCESSOR_SIMULATOR)

# Clean all generated files
clean:
	$(ECHO) [RM] $(BUILD_DIR)
	$(QUIET) $(RM) -rf $(BUILD_DIR)/*


.PHONY : all clean program terminal preprocessor-simulator list summary
