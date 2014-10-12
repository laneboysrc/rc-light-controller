/******************************************************************************

	Use IAP to program the flash
	A single page of 64 bytes should be sufficient
	Top 32 bytes of RAM needed
	RAM buffer with data needs to be on word boundary
	Uses 148 bytes of stack space
	Use compare function to only write changes
	Interrupts must be disabled during erase and write operations
	Q: How long does erase and write take?

******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <LPC8xx.h>
#include <LPC8xx_ROM_API.h>
#include <globals.h>
#include <uart0.h>

#define PERSISTENT_DATA_VERSION 1
#define NUMBER_OF_PERSISTENT_ELEMENTS 16

__attribute__ ((section(".persistent_data")))
volatile const uint32_t persistent_data[NUMBER_OF_PERSISTENT_ELEMENTS];

#define OFFSET_VERSION 0
#define OFFSET_STEERING_REVERSED 1
#define OFFSET_THROTTLE_REVERSED 2
#define OFFSET_SERVO_LEFT 3
#define OFFSET_SERVO_CENTRE 4
#define OFFSET_SERVO_RIGHT 5


// ****************************************************************************
void load_persistent_storage(void)
{
    uint32_t defaults[6];
    const volatile uint32_t *ptr;

    defaults[OFFSET_VERSION] = PERSISTENT_DATA_VERSION;
    defaults[OFFSET_STEERING_REVERSED] = false;
    defaults[OFFSET_THROTTLE_REVERSED] = false;
    defaults[OFFSET_SERVO_LEFT] = 1000;
    defaults[OFFSET_SERVO_CENTRE] = 1500;
    defaults[OFFSET_SERVO_RIGHT] = 2000;

    if (defaults[0] != persistent_data[0]) {
        ptr = defaults;
    }
    else {
        ptr = persistent_data;
    }

    channel[ST].reversed = ptr[OFFSET_STEERING_REVERSED];
    channel[TH].reversed = ptr[OFFSET_THROTTLE_REVERSED];
    servo_output_endpoint.left = ptr[OFFSET_SERVO_LEFT];
    servo_output_endpoint.centre = ptr[OFFSET_SERVO_CENTRE];
    servo_output_endpoint.right = ptr[OFFSET_SERVO_RIGHT];
}


// ****************************************************************************
void write_persistent_storage(void)
{
    uint32_t new_data[6];
    unsigned int param[5];
    int i;

    new_data[OFFSET_VERSION] = PERSISTENT_DATA_VERSION;
    new_data[OFFSET_STEERING_REVERSED] = channel[ST].reversed;
    new_data[OFFSET_THROTTLE_REVERSED] = channel[TH].reversed;
    new_data[OFFSET_SERVO_LEFT] = servo_output_endpoint.left;
    new_data[OFFSET_SERVO_CENTRE] = servo_output_endpoint.centre;
    new_data[OFFSET_SERVO_RIGHT] = servo_output_endpoint.right;

    for (i = 0; i < 6; i++) {
        if (new_data[0] != persistent_data[0]) {

            param[0] = 50;
            param[1] = ((unsigned int)persistent_data) >> 10;
            param[2] = ((unsigned int)persistent_data) >> 10;
            __disable_irq();
            iap_entry(param, param);
            __enable_irq();
            if (param[0] != 0) {
                uart0_send_cstring("ERROR: prepare sector failed\n");
                break;
            }

            param[0] = 59;  // Erase page command
            param[1] = ((unsigned int)persistent_data) >> 6;
            param[2] = ((unsigned int)persistent_data) >> 6;
            param[3] = __SYSTEM_CLOCK / 1000;
            __disable_irq();
            iap_entry(param, param);
            __enable_irq();
            if (param[0] != 0) {
                uart0_send_cstring("ERROR: erase page failed\n");
                break;
            }

            param[0] = 50;
            param[1] = ((unsigned int)persistent_data) >> 10;
            param[2] = ((unsigned int)persistent_data) >> 10;
            __disable_irq();
            iap_entry(param, param);
            __enable_irq();
            if (param[0] != 0) {
                uart0_send_cstring("ERROR: prepare sector failed\n");
                break;
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
                uart0_send_cstring("ERROR: copy RAM to flash failed\n");
                break;
            }

            break;
        }
    }
}
