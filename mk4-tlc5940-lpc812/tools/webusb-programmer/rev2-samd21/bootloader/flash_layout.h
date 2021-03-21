#pragma once

#include <samd21.h>

// The bootloader is at the bottom of the flash memory. Its size is 8 kBytes.
#define FLASH_BOOTLOADER_START (FLASH_ADDR)
#define FLASH_BOOTLOADER_SIZE (8*1024)

// The firmware resides after the bootloader, consuming the rest of the flash
#define FLASH_FIRMWARE_START (FLASH_BOOTLOADER_START + FLASH_BOOTLOADER_SIZE)
#define FLASH_FIRMWARE_SIZE (FLASH_SIZE - FLASH_BOOTLOADER_SIZE)