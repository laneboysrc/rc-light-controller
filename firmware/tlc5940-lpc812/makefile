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
GCC_ARM := /opt/gcc-arm-none-eabi-4_8-2014q2
GCC_ARM_BIN := $(GCC_ARM)/bin
GCC_ARM_LIB := $(GCC_ARM)/lib/gcc/arm-none-eabi/4.8.4/armv6-m

CC := $(GCC_ARM_BIN)/arm-none-eabi-gcc
LD := $(GCC_ARM_BIN)/arm-none-eabi-ld
OBJCOPY := $(GCC_ARM_BIN)/arm-none-eabi-objcopy

FLASH_TOOL := lpc81x_isp.py --run --flash $<
TERMINAL_PROGRAM := miniterm.py -p /dev/ttyUSB0 -b 115200
PREPROCESSOR_SIMULATOR := ../../tools/preprocessor-simulator.py -b 115200
MAP_SUMMARY_TOOL := ../../tools/parse_gcc_map_file.py

###############################################################################
# Compiler and linker flags
CFLAGS = -mthumb -mcpu=cortex-m0plus
CFLAGS += -std=c99
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wstrict-prototypes -Wshadow -Wwrite-strings
CFLAGS += -Wdeclaration-after-statement -Waddress -Wlogical-op
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wmissing-field-initializers -Wdouble-promotion -Wfloat-equal
CFLAGS += -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas
CFLAGS += -Wundef
CFLAGS += -I. -isystem./LPC8xx
CFLAGS += -Os -fdata-sections -ffunction-sections -fno-common
CFLAGS += -D__SYSTEM_CLOCK=$(SYSTEM_CLOCK)

LDFLAGS = -nostdlib -T $(LINKER_SCRIPT) -Map=$(MAPFILE) --gc-sections -L$(GCC_ARM_LIB)

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

