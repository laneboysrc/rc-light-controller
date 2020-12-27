.DEFAULT_GOAL := all

###############################################################################
# Configuration options for our application
BUILD_DIR = build
TARGET := $(BUILD_DIR)/configurator.html


###############################################################################
# Toolchain setup
NPM = npm
INLINE_SOURCE = ./node_modules/.bin/inline-source
MKDIR_P = mkdir -p


###############################################################################
# Pretty-print setup
VERBOSE ?= $V
ifeq ("$(VERBOSE)", "1")
ECHO :=
else
ECHO := @
endif


###############################################################################
# Source files
SOURCES := src/index.html
SOURCES += src/config_hardware_test.js
SOURCES += src/config_preprocessor.js
SOURCES += src/configurator.css
SOURCES += src/default_firmware_image_mk4.js
SOURCES += src/gamma.js
SOURCES += src/lpc81x_isp.js
SOURCES += src/main.js
SOURCES += src/preprocessor-simulator.js
SOURCES += src/templating.js
SOURCES += src/ui.js
SOURCES += src/webusb.js


###############################################################################
# Rules
all: $(TARGET)

$(BUILD_DIR):
	@echo [MKDIR] $@
	$(ECHO) mkdir -p $@

$(INLINE_SOURCE):
	@echo [NPM]
	$(ECHO) $(NPM) install

$(TARGET): $(BUILD_DIR) $(INLINE_SOURCE) $(SOURCES)
	@echo [BUILD]
	$(ECHO) make -C ../light-program-assembler all
	$(ECHO) cd src ; ../$(INLINE_SOURCE) $(COMPRESS) index.html ../$@
	$(ECHO) sed -i -e 's:#{this.date}:'$(shell date +%Y-%m-%d)':g' $@

clean:
	@echo [RM] $(BUILD_DIR)
	$(ECHO) $(RM) -r $(BUILD_DIR)

# debug: $(TARGET)
# 	@echo [NW.JS debug]
# 	$(ECHO) ./node_modules/.bin/nwbuild -p linux64 -v 0.31.5 --flavor sdk --cacheDir ./cache/ -r .

# release: COMPRESS=--compress
release: $(TARGET)



.PHONY : all clean debug release
