.DEFAULT_GOAL := all

###############################################################################
# Configuration options for our application
TARGET := firmware
BUILD_DIR = build
DEFAULT_LIGHT_PROGRAM := ../../mk4-tlc5940-lpc812/firmware/light_programs/generic.light_program


###############################################################################
# Pretty-print setup
VERBOSE ?= $V
ifeq ("$(VERBOSE)", "1")
ECHO :=
else
ECHO := @
endif


###############################################################################
# Toolchain setup
CC := arm-none-eabi-gcc
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
GDB := arm-none-eabi-gdb

TOOL_PATH := ../../mk4-tlc5940-lpc812/tools/
GENERIC_TOOL_PATH := ../../tools/
BOOTLOADER_BIN := ../bootloader/build/bootloader.bin

MKDIR_P := mkdir -p
FLASH_TOOL := openocd -l build/openocd.log -f board/atmel_samr21_xplained_pro.cfg
DFU_UTIL := dfu-util
TERMINAL_PROGRAM := miniterm.py -b 115200 /dev/ttyACM0
PREPROCESSOR_SIMULATOR := $(GENERIC_TOOL_PATH)preprocessor-simulator.py -b 115200 /dev/ttyACM1
CONFIGURATION_VALIDATION_TOOL := $(TOOL_PATH)validate_image_file.py
LIGHT_PROGRAM_ASSEMBLER_PATH := $(TOOL_PATH)configurator/assembler
DEFAULT_FIRMWARE_IMAGE_JS := $(TOOL_PATH)configurator/src/default_firmware_image.js
TEXT2JS := $(TOOL_PATH)text2js.py

###############################################################################
# Source files    Note: all source files must be relative paths!
SOURCE_PATH := ../../mk4-tlc5940-lpc812/firmware

SOURCES += $(SOURCE_PATH)/main.c
SOURCES += $(SOURCE_PATH)/ch3_handler.c
SOURCES += $(SOURCE_PATH)/channel_reversing.c
SOURCES += $(SOURCE_PATH)/config.c
SOURCES += ./config_light_programs.c
SOURCES += $(SOURCE_PATH)/config_lights.c
SOURCES += $(SOURCE_PATH)/drive_mode.c
SOURCES += $(SOURCE_PATH)/indicators.c
SOURCES += $(SOURCE_PATH)/light_programs.c
SOURCES += $(SOURCE_PATH)/lights.c
SOURCES += $(SOURCE_PATH)/persistent_storage.c
SOURCES += $(SOURCE_PATH)/preprocessor_output.c
SOURCES += $(SOURCE_PATH)/servo_output.c
SOURCES += $(SOURCE_PATH)/servo_reader.c
SOURCES += $(SOURCE_PATH)/uart_reader.c
SOURCES += $(SOURCE_PATH)/utils.c
SOURCES += $(SOURCE_PATH)/winch.c
SOURCES += $(SOURCE_PATH)/libs/printf/printf.c
SOURCES += hal/hal_samd21.c
SOURCES += hal/startup_samd21.c
SOURCES += hal/usb.c
SOURCES += libs/usb/usb_requests.c
SOURCES += libs/usb/samd/usb_samd.c

INCLUDES += -I.
INCLUDES += -I$(SOURCE_PATH)
INCLUDES += -I./hal
INCLUDES += -isystem./hal/samd21
INCLUDES += -isystem./libs/usb
INCLUDES += -isystem./libs/usb/samd
INCLUDES += -isystem$(SOURCE_PATH)/libs/printf

DEFINES += -D__SAMD21E15A__
DEFINES += -DDONT_USE_CMSIS_INIT
DEFINES += -DPRINTF_SUPPORT_LONG
DEFINES += -DNODEBUG

LIBS +=

DEPENDENCIES += makefile


###############################################################################
# Compiler and linker flags
CPU_FLAGS = -mthumb -mcpu=cortex-m0plus

CFLAGS += $(CPU_FLAGS)
CFLAGS += -std=gnu99
CFLAGS += -Os
CFLAGS += -g
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wstrict-prototypes -Wshadow -Wwrite-strings
CFLAGS += -Wdeclaration-after-statement -Waddress -Wlogical-op
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wmissing-field-initializers -Wdouble-promotion -Wfloat-equal
CFLAGS += -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas
CFLAGS += -Wundef
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -fpack-struct=4
CFLAGS += -MMD -MP -MT $(BUILD_DIR)/$(*F).o -MF $(BUILD_DIR)/$(@F).d
CFLAGS += $(INCLUDES) $(DEFINES)

LDFLAGS += $(CPU_FLAGS)
LDFLAGS += -Wl,-T,$(LINKER_SCRIPT)
LDFLAGS += -Wl,-nostdlib
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Os
LDFLAGS += -Wl,-Map=$(TARGET_MAP) -Wl,--cref

LINKER_SCRIPT := hal/samd21e15.ld


###############################################################################
# TARGET and object file setup
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir %/$(subst .c,.o, $(SOURCES))))

TARGET_ELF := $(addprefix $(BUILD_DIR)/, $(TARGET).elf)
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET).bin)
TARGET_HEX := $(addprefix $(BUILD_DIR)/, $(TARGET).hex)
TARGET_MAP := $(addprefix $(BUILD_DIR)/, $(TARGET).map)
TARGET_ELF_WITH_BOOTLOADER := $(addprefix $(BUILD_DIR)/, $(TARGET)_with_bootloader.elf)

$(OBJECTS): $(DEPENDENCIES)
$(TARGET_MAP): $(TARGET_ELF)


# Remove certain warnings from the USB library
$(BUILD_DIR)/usb_requests.o: CFLAGS += -Wno-pedantic -Wno-missing-prototypes -Wno-switch-default -Wno-missing-declarations
# $(BUILD_DIR)/dfu.o: CFLAGS += -Wno-pedantic -Wno-declaration-after-statement -Wno-switch-default
$(BUILD_DIR)/usb_samd.o: CFLAGS += -Wno-strict-prototypes -Wno-missing-prototypes


###############################################################################
# Rules
all : $(TARGET_BIN) $(TARGET_HEX) echosize

$(BUILD_DIR):
	@echo [MKDIR] $@
	@mkdir -p $@

$(TARGET_ELF): $(BUILD_DIR) $(OBJECTS) $(LINKER_SCRIPT)
	@echo [LD] $@
	$(ECHO) $(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(TARGET_BIN): $(TARGET_ELF)
	@echo [CP] $@
	$(ECHO) $(OBJCOPY) $< -O binary $@

$(TARGET_HEX): $(TARGET_ELF)
	@echo [CP] $@
	$(ECHO) $(OBJCOPY) $< -O ihex $@

# For debugging and flashing the application via openocd we create a special
# ELF file that includes the bootloader.
# We transform the binary bootloader image into a .o file, which we link
# at address 0 of the flash. This is done in the linker script.
#
# Source: https://visualgdb.com/tutorials/arm/bootloader/
$(TARGET_ELF_WITH_BOOTLOADER): $(BUILD_DIR) $(OBJECTS) $(LINKER_SCRIPT) $(BUILD_DIR)/bootloader.o
	@echo [LD bootloader] $@
	$(ECHO) $(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) $(BUILD_DIR)/bootloader.o -o $@

$(BOOTLOADER_BIN) :
	$(MAKE) -C ../bootloader

$(BUILD_DIR)/bootloader.o: $(BOOTLOADER_BIN)
	@echo [BOOTLOADER] $@
	$(ECHO) $(LD) -r -b binary $< -o $@
	$(ECHO) $(OBJCOPY) --rename-section .data=.bootloader $@

%.o:
	@echo [CC] $@
	$(ECHO) $(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(@F)), $(addprefix ./,$(SOURCES))) -c -o $@

%.lst:
	@echo [LIST] $@
	$(ECHO) $(CC) -c -g -Wa,-adlhn $(CFLAGS) $(filter %/$(subst .lst,.c,$(@F)), $(addprefix ./,$(SOURCES))) -o /dev/null > $@

echosize: $(TARGET_ELF)
	-@echo ''
	$(ECHO) $(SIZE) $(TARGET_ELF)
	-@echo ''

config_light_programs.c: $(DEFAULT_LIGHT_PROGRAM)
	@echo [ASM] $@
	$(ECHO) cd $(LIGHT_PROGRAM_ASSEMBLER_PATH) && $(MAKE) run RUN_OPTIONS="--include-name -o $(abspath config_light_programs.c) $(abspath $(DEFAULT_LIGHT_PROGRAM))"

default_firmware_image: $(TARGET_HEX)
	@echo [TEXT2JS] $<
	$(ECHO) $(TEXT2JS) $< default_firmware_image >$(DEFAULT_FIRMWARE_IMAGE_JS)
	@echo [TEXT2JS] $(DEFAULT_LIGHT_PROGRAM)
	$(ECHO) $(TEXT2JS) $(DEFAULT_LIGHT_PROGRAM) default_light_program >>$(DEFAULT_FIRMWARE_IMAGE_JS)

# Create list files that include C code as well as Assembler
list: $(BUILD_DIR) $(OBJECTS:.o=.lst)

# Invoke the tool to program the microcontroller
program: $(TARGET_ELF_WITH_BOOTLOADER)
	@echo [PROGRAM] $<
	$(ECHO)$(GDB) -nx -quiet -batch -command gdb.flash.script
#	$(ECHO) $(FLASH_TOOL) -c "init; reset halt ; at91samd bootloader 0; reset halt ; flash write_image $< ; at91samd bootloader 8192; reset ; exit"

flash: program

# Load the program using the USB DFU (Device Firmware Upgrade) tool
# This will only work when the bootloader is already present in the device
dfu-util: $(TARGET_BIN)
	$(ECHO) $(DFU_UTIL) -d 0x6666:0xcab1,:0xcab0 -E 1 -D $<

# Set/clear the fuses that protect the bootloader from being accidentally
# overwritten
lock-bootloader:
	$(ECHO) $(FLASH_TOOL) -c "init ; reset halt ; at91samd bootloader 8192; reset ; exit"
	$(ECHO) sleep 1

unlock-bootloader:
	$(ECHO) $(FLASH_TOOL) -c "init ; reset halt ; at91samd bootloader 0; reset ; exit"
	$(ECHO) sleep 1

# Invoke a tool for UART communication
terminal:
	$(ECHO) $(TERMINAL_PROGRAM)

# Invoke the preprocessor simulation tool
preprocessor-simulator:
	$(ECHO) $(PREPROCESSOR_SIMULATOR)

# Clean all generated files
clean:
	@echo [RM] $(BUILD_DIR)
	$(ECHO) $(RM) -rf $(BUILD_DIR)

# debug: $(TARGET_ELF_WITH_BOOTLOADER) unlock-bootloader
	# $(ECHO) $(GDB) -q -ex "target extended-remote | $(FLASH_TOOL) -c \"gdb_port pipe\"" -ex "load" --tui $<
debug: $(TARGET_ELF_WITH_BOOTLOADER)
	$(ECHO) $(GDB) -nx -quiet -command gdb.debug.script



.PHONY : all clean debug echosize list program flash dfu-util lock-bootloader unlock-bootloader terminal default_firmware_image preprocessor-simulator

-include $(wildcard $(BUILD_DIR)/*.d)
