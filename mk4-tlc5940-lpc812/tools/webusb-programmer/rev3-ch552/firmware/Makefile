.DEFAULT_GOAL := all

###############################################################################
# Configuration options for our application
TARGET := webusb-programmer

BUILD_DIR = build

XRAM_SIZE ?= 0x0400
XRAM_LOC ?= 0x0000
CODE_LOC ?= 0x0000
CODE_SIZE ?= 0x3800


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
CC = sdcc
OBJCOPY = objcopy
PACK_HEX = packihx
DETACH_TOOL ?= ../tools/dfu-detach/dfu-detach
ISP_TOOL ?= ../tools/chprog/chprog


###############################################################################
# Source files
SOURCES := main.c
SOURCES += ring_buffer.c
SOURCES += usb.c
SOURCES += usb_descriptors.c

INCLUDES := -I..


###############################################################################
# Compiler and Linker flags
CPU_FLAGS := -mmcs51 --model-small

CFLAGS := $(CPU_FLAGS)
CFLAGS += --std-c99
# Since we use absolute addressing for the endpoint DMA buffers, we place
# them at the end of the 1K __xdata space so that they have less chance
# colliding with other __xdata variables.
#
# Note that SDCC does not check absolute positioned __xdata addresses so you need
# to manually take care of allocation!
#
# Note that According to the datasheet if the endpoint size is <64 we have to
# reserve two additional bytes!?!
#
# IMPORTANT: the serial number implementation in usb.c requires EP0_SIZE to
# be at least 22!
CFLAGS += -DEP0_ADDR=832 -DEP0_SIZE=64
CFLAGS += -DEP1_ADDR=896 -DEP1_SIZE=64
CFLAGS += -DEP2_ADDR=960 -DEP2_SIZE=64

LDFLAGS := $(CPU_FLAGS)
LDFLAGS += --xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC)
LDFLAGS += --code-size $(CODE_SIZE)
LDFLAGS += --code-loc $(CODE_LOC)


###############################################################################
# Objects and target files
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir %/$(SOURCES:.c=.rel)))

TARGET_IHX := $(addprefix $(BUILD_DIR)/, $(TARGET).ihx)
TARGET_BIN := $(addprefix $(BUILD_DIR)/, $(TARGET).bin)
TARGET_HEX := $(addprefix $(BUILD_DIR)/, $(TARGET).hex)


###############################################################################
# Rules

all: $(TARGET_BIN) $(TARGET_HEX)

$(BUILD_DIR):
	@echo [MKDIR] $@
	@mkdir -p $@

$(TARGET_IHX): $(BUILD_DIR) $(OBJECTS)
	@echo [LD] $@
	$(ECHO) $(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(TARGET_HEX): $(TARGET_IHX)
	@echo [HEX] $@
	$(ECHO) $(PACK_HEX) $< > $@

$(TARGET_BIN): $(TARGET_IHX)
	@echo [BIN] $@
	$(ECHO) $(OBJCOPY) -I ihex -O binary $< $@

# Some trickery for .d file creation to prepend the BUILD_DIR to the .rel file
%.rel:
	@echo [CC] $(@F:.rel=.c)
	$(ECHO) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $(@F:.rel=.c)
	@echo -n "$(BUILD_DIR)/"  > $(@:.rel=.d)
	$(ECHO) $(CC) -M $(INCLUDES) $(@F:.rel=.c) >> $(@:.rel=.d)

program: $(TARGET_BIN)
	$(ECHO) $(DETACH_TOOL)
	sleep 1
	$(ECHO) $(ISP_TOOL) $<

flash: program

clean:
	@echo [CLEAN]
	$(ECHO) rm -rf $(BUILD_DIR)

.PHONY : all clean program flash

-include $(wildcard $(BUILD_DIR)/*.d)
