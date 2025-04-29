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

#include <hal.h>
#include <globals.h>
#include <printf.h>

#define PERSISTENT_DATA_VERSION 1

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
    const volatile uint32_t *persistent_data = HAL_persistent_storage_read();

    defaults[OFFSET_VERSION] = PERSISTENT_DATA_VERSION;
    defaults[OFFSET_STEERING_REVERSED] = config.flags.reverse_st;
    defaults[OFFSET_THROTTLE_REVERSED] = config.flags.reverse_th;
    defaults[OFFSET_SERVO_LEFT] = config.servo_out_pulse_left;
    defaults[OFFSET_SERVO_CENTRE] = config.servo_out_pulse_centre;
    defaults[OFFSET_SERVO_RIGHT] = config.servo_out_pulse_right;

    if (defaults[0] != persistent_data[0]) {
        ptr = defaults;
    }
    else {
        ptr = persistent_data;
    }

    channel[ST].reversed = ptr[OFFSET_STEERING_REVERSED];
    channel[TH].reversed = ptr[OFFSET_THROTTLE_REVERSED];
    channel[AUX].reversed = config.flags.reverse_aux;
    channel[AUX2].reversed = config.flags.reverse_aux2;
    channel[AUX3].reversed = config.flags.reverse_aux3;
    servo_output_endpoint.left = ptr[OFFSET_SERVO_LEFT];
    servo_output_endpoint.centre = ptr[OFFSET_SERVO_CENTRE];
    servo_output_endpoint.right = ptr[OFFSET_SERVO_RIGHT];
}


// ****************************************************************************
void write_persistent_storage(void)
{
    uint32_t new_data[6];
    const char *error_message;

    new_data[OFFSET_VERSION] = PERSISTENT_DATA_VERSION;
    new_data[OFFSET_STEERING_REVERSED] = channel[ST].reversed;
    new_data[OFFSET_THROTTLE_REVERSED] = channel[TH].reversed;
    new_data[OFFSET_SERVO_LEFT] = servo_output_endpoint.left;
    new_data[OFFSET_SERVO_CENTRE] = servo_output_endpoint.centre;
    new_data[OFFSET_SERVO_RIGHT] = servo_output_endpoint.right;

    error_message = HAL_persistent_storage_write(new_data);

    if (error_message) {
        printf("ERROR: %s failed\n", error_message);
    }
}
