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

__attribute__ ((section(".persistent_data")))
uint8_t persistent_data[64] = { 0 };

// ****************************************************************************
void load_persistent_storage(void)
{
    ;
}


// ****************************************************************************
void write_persistent_storage(void)
{
    ;
}
