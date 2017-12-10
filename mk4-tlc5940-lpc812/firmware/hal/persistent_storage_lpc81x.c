#include <stdint.h>
#include <stdio.h>

#include <LPC8xx.h>
#include <LPC8xx_ROM_API.h>

#include <hal.h>

__attribute__ ((section(".persistent_data")))
static volatile const uint32_t persistent_data[HAL_NUMBER_OF_PERSISTENT_ELEMENTS];

volatile const uint32_t *HAL_persistent_storage_read(void)
{
    return persistent_data;
}

const char *HAL_persistent_storage_write(const uint32_t *new_data)
{
    unsigned int param[5];

    param[0] = 50;
    param[1] = ((unsigned int)persistent_data) >> 10;
    param[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: prepare sector failed";
    }

    param[0] = 59;  // Erase page command
    param[1] = ((unsigned int)persistent_data) >> 6;
    param[2] = ((unsigned int)persistent_data) >> 6;
    param[3] = __SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: erase page failed";
    }

    param[0] = 50;
    param[1] = ((unsigned int)persistent_data) >> 10;
    param[2] = ((unsigned int)persistent_data) >> 10;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: prepare sector failed";
    }

    param[0] = 51;  // Copy RAM to Flash command
    param[1] = (unsigned int)persistent_data;
    param[2] = (unsigned int)new_data;
    param[3] = 64;
    param[4] = __SYSTEM_CLOCK / 1000;
    __disable_irq();
    iap_entry(param, param);
    __enable_irq();
    if (param[0] != 0) {
        return "ERROR: copy RAM to flash failed";
    }

    return NULL;
}