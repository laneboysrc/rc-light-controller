.DEFAULT_GOAL := all

###############################################################################
# Configuration options for our application
TARGET := bootloader
BUILD_DIR = build


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
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
GDB := arm-none-eabi-gdb

MKDIR_P := mkdir -p
FLASH_TOOL := openocd


###############################################################################
# Source files    Note: all source files must be relative paths!
COMMON_SOURCE_PATH := ../firmware/
PRINTF_SOURCE_PATH := ../../mk4-tlc5940-lpc812/firmware/libs/printf/

SOURCES += ./main.c
SOURCES += ./usb.c
SOURCES += $(COMMON_SOURCE_PATH)hal/startup_samd21.c
SOURCES += $(COMMON_SOURCE_PATH)libs/usb/usb_requests.c
SOURCES += $(COMMON_SOURCE_PATH)libs/usb/class/dfu/dfu.c
SOURCES += $(COMMON_SOURCE_PATH)libs/usb/samd/usb_samd.c
SOURCES += $(PRINTF_SOURCE_PATH)printf.c

INCLUDES += -I.
INCLUDES += -I$(COMMON_SOURCE_PATH)hal
INCLUDES += -isystem$(COMMON_SOURCE_PATH)hal/samd21
INCLUDES += -isystem$(COMMON_SOURCE_PATH)libs/usb
INCLUDES += -isystem$(COMMON_SOURCE_PATH)libs/usb/samd
INCLUDES += -isystem$(PRINTF_SOURCE_PATH)

DEFINES += -D__SAMD21E15A__
DEFINES += -DDONT_USE_CMSIS_INIT
DEFINES += -DPRINTF_SUPPORT_LONG
DEFINES += -DNO_STACK_CANARIES
# DEFINES += -DNODEBUG

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

LINKER_SCRIPT := samd21e15-bootloader.ld


###############################################################################
# TARGET and object file setup
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir %/$(subst .c,.o, $(SOURCES))))

TARGET_ELF := $(addprefix $(BUILD_DIR)/, $(TARGET).elf)
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET).bin)
TARGET_HEX := $(addprefix $(BUILD_DIR)/, $(TARGET).hex)
TARGET_MAP := $(addprefix $(BUILD_DIR)/, $(TARGET).map)

$(OBJECTS): $(DEPENDENCIES)
$(TARGET_MAP): $(TARGET_ELF)


# Remove certain warnings from the USB library
$(BUILD_DIR)/usb_requests.o: CFLAGS += -Wno-pedantic -Wno-missing-prototypes -Wno-switch-default -Wno-missing-declarations
$(BUILD_DIR)/dfu.o: CFLAGS += -Wno-pedantic -Wno-declaration-after-statement -Wno-switch-default
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

# Create list files that include C code as well as Assembler
list: $(BUILD_DIR) $(OBJECTS:.o=.lst)

# Invoke the tool to program the microcontroller
program: $(TARGET_ELF)
	$(QUIET) $(FLASH_TOOL) -f board/atmel_samr21_xplained_pro.cfg -c "init; reset halt ; flash write_image $< ; reset ; shutdown"

flash: program

# Clean all generated files
clean:
	@echo [RM] $(BUILD_DIR)
	$(ECHO) $(RM) -rf $(BUILD_DIR)

debug: $(TARGET_ELF)
	$(ECHO) $(GDB) -q -ex "target extended-remote | $(FLASH_TOOL) -f board/atmel_samr21_xplained_pro.cfg -c \"gdb_port pipe\"" -ex "load" --tui $<



.PHONY : all clean debug echosize list program flash

-include $(wildcard $(BUILD_DIR)/*.d)
