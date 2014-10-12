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

__attribute__ ((section(".persistent_data")))
volatile uint32_t persistent_data[16];

//uint32_t *persistent_data = 0x2680;


// ****************************************************************************
void load_persistent_storage(void)
{
#if 0
    PERSISTENT_DATA_T defaults;
    const PERSISTENT_DATA_T *ptr;
    
    defaults.version = PERSISTENT_DATA_VERSION;
    defaults.steering_reversed = false;    
    defaults.throttle_reversed = false;    
    defaults.servo_output_endpoint.left = 1000;
    defaults.servo_output_endpoint.centre = 1500;
    defaults.servo_output_endpoint.right = 2000;

    uart0_send_cstring("read_persistent_storage\n");

    if (defaults.version != persistent_data.version) {
        uart0_send_cstring("Using defaults!\n");
        ptr = &defaults;
    }
    else {
        ptr = &persistent_data;
    }

    channel[ST].reversed = ptr->steering_reversed;
    channel[TH].reversed = ptr->throttle_reversed;
    servo_output_endpoint = ptr->servo_output_endpoint;
    #endif
}


// ****************************************************************************
void write_persistent_storage(void)
{
    uint32_t new_data[6];
    int i;
    
    new_data[0] = PERSISTENT_DATA_VERSION;
    new_data[1] = false;    
    new_data[2] = false;    
    new_data[3] = 1000;
    new_data[4] = 1500;
    new_data[5] = 2000;   

    uart0_send_cstring("persistent_data is at 0x");
    uart0_send_uint32_hex(persistent_data);
    uart0_send_linefeed();



    
    if (new_data[0] != persistent_data[0] ||
        new_data[1] != persistent_data[1] ||
        new_data[2] != persistent_data[2] ||
        new_data[3] != persistent_data[3] ||
        new_data[4] != persistent_data[4] ||
        new_data[5] != persistent_data[5]) {
        
        unsigned int param[5];
        uart0_send_cstring("write_persistent_storage ");
        uart0_send_uint32(((uint32_t)persistent_data)>>6);
        uart0_send_linefeed();
        
        param[0] = 50;
        param[1] = ((unsigned int)persistent_data) >> 10;
        param[2] = ((unsigned int)persistent_data) >> 10;
        __disable_irq();
        iap_entry(param, param);
        __enable_irq();
        uart0_send_cstring("Prepare sector: ");
        uart0_send_uint32(param[0]);
        uart0_send_linefeed();

        param[0] = 59;  // Erase page command
        param[1] = ((unsigned int)persistent_data) >> 6; 
        param[2] = ((unsigned int)persistent_data) >> 6;
        param[3] = __SYSTEM_CLOCK / 1000;
        __disable_irq();
        iap_entry(param, param);
        __enable_irq();
        uart0_send_cstring("Erase page: ");
        uart0_send_uint32(param[0]);
        uart0_send_linefeed();

        param[0] = 50;
        param[1] = ((unsigned int)persistent_data) >> 10;
        param[2] = ((unsigned int)persistent_data) >> 10;
        __disable_irq();
        iap_entry(param, param);
        __enable_irq();
        uart0_send_cstring("Prepare sector: ");
        uart0_send_uint32(param[0]);
        uart0_send_linefeed();

        param[0] = 51;  // Copy RAM to Flash command
        param[1] = (unsigned int)persistent_data; 
        param[2] = (unsigned int)new_data;
        param[3] = 64;
        param[4] = __SYSTEM_CLOCK / 1000;
        __disable_irq();
        iap_entry(param, param);
        __enable_irq();
        uart0_send_cstring("Copy: ");
        uart0_send_uint32(param[0]);
        uart0_send_linefeed();
    }   

    for (i = 0; i < 6; i++) {
        uart0_send_uint32_hex(persistent_data[i]);
        uart0_send_cstring(" -> ");
        uart0_send_uint32_hex(new_data[i]);
        uart0_send_linefeed();
    }
}
