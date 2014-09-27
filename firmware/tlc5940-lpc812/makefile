PROGRAM_NAME := master

all : $(PROGRAM_NAME).bin


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

FLASH_TOOL = lpc81x_isp.py --run --flash $<
TERMINAL_PROGRAM := miniterm.py -p /dev/ttyUSB0 -b 115200


###############################################################################
# Compiler and linker flags
CFLAGS := -mthumb -mcpu=cortex-m0plus
CFLAGS += -Wall -Wextra
CFLAGS += -I. -I./LPC8xx
CFLAGS += -Os -fdata-sections -ffunction-sections
CFLAGS += -D__SYSTEM_CLOCK=$(SYSTEM_CLOCK) -DUART0_BAUDRATE=38400

LDFLAGS := -nostdlib -T $(LINKER_SCRIPT) -Map=MAPFILE --gc-sections -L$(GCC_ARM_LIB)

LDLIBS := -lgcc


###############################################################################
# Components of our application
OBJECTS := main.o crt0.o utils.o config.o uart0.o
OBJECTS += servo_reader.o uart_reader.o ch3_handler.o servo_output.o
OBJECTS += drive_mode.o indicators.o winch.o channel_reversing.o
OBJECTS += output_preprocessor.o


###############################################################################
# Dependencies
$(OBJECTS) : makefile globals.h uart0.h utils.h


###############################################################################
# Rules
%.axf : $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(subst MAPFILE,$(@:.axf=.map),$(LDFLAGS)) -o $@ $(OBJECTS) $(LDLIBS)


%.bin : %.axf
	$(OBJCOPY) $< -O binary $@


# Generate list files with mixed Assembler and C code
%.lst : %.c
	$(CC) -c -g -Wa,-adlhn $(CFLAGS) $< > $@


###############################################################################
# Helper tools

# Create list files that include C code as well as Assembler
list : $(OBJECTS:.o=.lst)


# Invoke the tool to program the microcontroller
program : $(PROGRAM_NAME).bin
	$(FLASH_TOOL)


# Invoke a tool for UART communication
terminal :
	$(TERMINAL_PROGRAM)


clean :
	rm -f $(OBJECTS)
	rm -f $(OBJECTS:.o=.lst)
	rm -f $(PROGRAM_NAME).bin
	rm -f $(PROGRAM_NAME).axf
	rm -f $(PROGRAM_NAME).map


.PHONY : all clean program terminal list

