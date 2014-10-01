PROGRAM_NAME := master

all : $(PROGRAM_NAME).bin

QUIET := @
#QUIET :=

###############################################################################
# Configuration options for our application
SYSTEM_CLOCK := 12000000
LINKER_SCRIPT := lpc812.ld


###############################################################################
# Toolchain setup
TOOLCHAIN_PREFIX := arm-none-eabi-
TOOLCHAIN_PATH := /opt/gcc-arm-none-eabi-4_8-2014q2/bin/
TOOL_PATH := ../../tools/

CC := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld
OBJCOPY := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy

FLASH_TOOL := lpc81x_isp.py --run --flash $<
TERMINAL_PROGRAM := miniterm.py -p /dev/ttyUSB0 -b 115200
PREPROCESSOR_SIMULATOR := $(TOOL_PATH)preprocessor-simulator.py -b 115200
MAP_SUMMARY_TOOL := $(TOOL_PATH)parse_gcc_map_file.py

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
CFLAGS += -Os -fsigned-char -fdata-sections -ffunction-sections -fno-common
CFLAGS += -D__SYSTEM_CLOCK=$(SYSTEM_CLOCK)

LDFLAGS = -T $(LINKER_SCRIPT) -nostdlib --warn-common
LDFLAGS += --gc-sections -O3
LDFLAGS += -Map=$(MAPFILE) --cref
LDFLAGS += -L$(TOOLCHAIN_PATH)../lib/gcc/arm-none-eabi/4.8.4/armv6-m

LDLIBS = -lgcc


###############################################################################
# Components of our application
OBJECTS := main.o crt0.o utils.o config.o uart0.o persistent_storage.o
OBJECTS += servo_reader.o uart_reader.o ch3_handler.o
OBJECTS += drive_mode.o indicators.o winch.o channel_reversing.o
OBJECTS += servo_output.o preprocessor_output.o lights.o


###############################################################################
# Dependencies
$(OBJECTS) : makefile globals.h uart0.h utils.h


###############################################################################
# Rules

%.o : %.c
	$(QUIET)$(CC) -c $(CFLAGS) -o $@ $<


%.axf : MAPFILE = $(@:.axf=.map)
%.axf : $(OBJECTS) $(LINKER_SCRIPT)
	$(QUIET)$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)
	$(QUIET)$(MAP_SUMMARY_TOOL) $(MAPFILE)

%.bin : %.axf
	$(QUIET)$(OBJCOPY) $< -O binary $@


# Generate list files with mixed Assembler and C code
%.lst : %.c
	$(QUIET)$(CC) -c -g -Wa,-adlhn $(CFLAGS) $< > $@


###############################################################################
# Helper tools

# Create list files that include C code as well as Assembler
list : $(OBJECTS:.o=.lst)


# Invoke the tool to program the microcontroller
program : $(PROGRAM_NAME).bin
	$(QUIET)$(FLASH_TOOL)


# Invoke a tool for UART communication
terminal :
	$(QUIET)$(TERMINAL_PROGRAM)


# Invoke the preprocessor simulation tool
preprocessor-simulator :
	$(QUIET)$(PREPROCESSOR_SIMULATOR)


clean :
	$(QUIET)rm -f $(OBJECTS)
	$(QUIET)rm -f $(OBJECTS:.o=.lst)
	$(QUIET)rm -f $(PROGRAM_NAME).bin
	$(QUIET)rm -f $(PROGRAM_NAME).axf
	$(QUIET)rm -f $(PROGRAM_NAME).map


.PHONY : all clean program terminal preprocessor-simulator list

