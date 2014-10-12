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

#include <globals.h>
#include <uart0.h>

#define PERSISTENT_DATA_VERSION 1

typedef struct {
    uint8_t version;
    bool steering_reversed;
    bool throttle_reversed;
    SERVO_ENDPOINTS_T servo_output_endpoint;
} PERSISTENT_DATA_T;

__attribute__ ((section(".persistent_data")))
const PERSISTENT_DATA_T persistent_data;



// ****************************************************************************
void load_persistent_storage(void)
{
    PERSISTENT_DATA_T defaults;
    const PERSISTENT_DATA_T *ptr;
    
    defaults.version = PERSISTENT_DATA_VERSION;
    defaults.steering_reversed = false;    
    defaults.throttle_reversed = false;    
    defaults.servo_output_endpoint.left = 1000;
    defaults.servo_output_endpoint.centre = 1500;
    defaults.servo_output_endpoint.right = 2000;

    if (defaults.version != persistent_data.version) {
        ptr = &defaults;
    }
    else {
        ptr = &persistent_data;
    }

    channel[ST].reversed = ptr->steering_reversed;
    channel[TH].reversed = ptr->throttle_reversed;
    servo_output_endpoint = ptr->servo_output_endpoint;
}


// ****************************************************************************
void write_persistent_storage(void)
{
    PERSISTENT_DATA_T new_data;
    
    new_data.version = PERSISTENT_DATA_VERSION;
    new_data.steering_reversed = false;    
    new_data.throttle_reversed = false;    
    new_data.servo_output_endpoint.left = 1000;
    new_data.servo_output_endpoint.centre = 1500;
    new_data.servo_output_endpoint.right = 2000;   
    
    if (new_data.version != persistent_data.version ||
        new_data.steering_reversed != persistent_data.steering_reversed ||
        new_data.throttle_reversed != persistent_data.throttle_reversed ||
        new_data.servo_output_endpoint.left != persistent_data.servo_output_endpoint.left ||
        new_data.servo_output_endpoint.centre != persistent_data.servo_output_endpoint.centre ||
        new_data.servo_output_endpoint.right != persistent_data.servo_output_endpoint.right) {
        
        uart0_send_cstring("Writing persistent data\n");
    }   
}
