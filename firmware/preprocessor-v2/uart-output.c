#include "processor.h"
#include <stdint.h>

#define SPBRG_VALUE 104     // 38400 @ 16 MHz

#define SLAVE_MAGIC_BYTE 0x87

extern uint8_t channel_data[4];
extern struct {
    unsigned locked : 1;
    unsigned new_data : 1;
} flags;

uint8_t next_tx_index;
uint8_t tx_data[4];


/*****************************************************************************
 Init_output()

 Called once during start-up of the firmware. Initialize the UART hardware.
 ****************************************************************************/
void Init_output(void) {
	BRG16 = 1;
	BRGH = 1;
	SPBRGH = SPBRG_VALUE / 256;
	SPBRGL = SPBRG_VALUE % 256;

	SYNC=0;			// Disable Synchronous/Enable Asynchronous
	SPEN=1;			// Enable serial port
	TXEN=1;			// Enable transmission mode
	CREN=0;			// Disable reception mode

    // Send a dummy value to get a valid transmit flag
	TXREG = SLAVE_MAGIC_BYTE;
	
	// Initialize an out-of-bound index into tx_data to signal that no 
	// transmission is in progress.
	next_tx_index = 4;
}


/*****************************************************************************
 Output_result()

 This non-blocking function sends the received and processed servo pulse
 values out via the UART. 
 
 It has its own transmit buffer so that new pulse measurements can begin
 while transmission is still in progress (even though that is not really
 needed). 
 ****************************************************************************/
void Output_result(void) {
    // Return immediately if UART transmit hardware is busy
    if (!TRMT) {
        return;
    }

    // Ongoing transmit? Yes: send the next value out.
    if (next_tx_index < 4) {
        TXREG = tx_data[next_tx_index++];
        return;
    }    

    // If new servo data was measured we copy it to our local buffer and send
    // out the first byte, which is the magic value 0x87 that signals "start
    // of frame".
    if (flags.new_data) {
        uint8_t i;

        flags.new_data = 0;

        for (i = 0; i < 4 ; i++) {
            tx_data[i] = channel_data[i];
        }
        
        next_tx_index = 0;
        TXREG = SLAVE_MAGIC_BYTE;  
        return;
    }
}

