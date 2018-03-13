all: deb rpm

# This makefile uses FPM to build .deb and .rpm packages for installation
# of udev rules, so that ordinary users are able to access the
# RC Light Controller via USB.
#
# https://github.com/jordansissel/fpm
#
#
# Install FPM on Ubuntu:
#
# 	sudo apt-get update
# 	sudo apt-get install ruby-dev build-essential
#
# 	sudo gem install fpm
#

BUILD_DIR = build
VERSION = 1
NAME = laneboysrc-light-controller


###############################################################################
# Pretty-print setup
VERBOSE ?= $V
ifeq ("$(VERBOSE)", "1")
ECHO :=
else
ECHO := @
endif


###############################################################################
RULE_PATH := $(BUILD_DIR)/lib/udev/rules.d/52-$(NAME).rules
ARCHIVE := $(BUILD_DIR)/$(NAME)-$(VERSION).tar.bz2

FPMFLAGS = -s tar
FPMFLAGS += -t $(TYPE)
FPMFLAGS += -a all
FPMFLAGS += -n $(NAME)
FPMFLAGS += -v $(VERSION)
FPMFLAGS += --vendor "LANE Boys RC"
FPMFLAGS += --description "udev rules for the LANE Boys RC Light Controller MK5"
FPMFLAGS += --maintainer "Werner Lane <laneboysrc@gmail.com>"
FPMFLAGS += --url "https://github.com/laneboysrc/rc-light-controller"
FPMFLAGS += --license "MIT"
FPMFLAGS += --force
FPMFLAGS += --log error

###############################################################################
rpm: TYPE = rpm
rpm: $(ARCHIVE)
	$(info [RPM])
	$(ECHO) fpm $(FPMFLAGS) $(ARCHIVE)

deb: TYPE = deb
deb: $(ARCHIVE)
	$(info [DEB])
	$(ECHO) fpm $(FPMFLAGS) $(ARCHIVE)

$(ARCHIVE): $(RULE_PATH)
	$(info [ARCHIVING] $^)
	$(ECHO) (cd $(BUILD_DIR) && tar cfj $(notdir $(ARCHIVE)) ./lib)

$(RULE_PATH): light-controller.rules
	$(info [CP] $< -> $@)
	$(ECHO) install -D -m 0644 $< $@

clean:
	$(info [RM] $(BUILD_DIR))
	$(ECHO) $(RM) -rf $(BUILD_DIR)

.phony: deb rpm clean
