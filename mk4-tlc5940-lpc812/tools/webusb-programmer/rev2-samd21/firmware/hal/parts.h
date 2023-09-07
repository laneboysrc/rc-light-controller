// This file is required to compile the 3rd party USB stack

#pragma once

#include <samd21.h>

// The USB stack seems to be made for an older revision of the Atmel header
// files, so we need to map old names to new names.
#define USB_DEVICE_CTRLB_SPDCONF_0_Val USB_DEVICE_CTRLB_SPDCONF_FS_Val
#define USB_DEVICE_CTRLB_SPDCONF_1_Val USB_DEVICE_CTRLB_SPDCONF_LS_Val
