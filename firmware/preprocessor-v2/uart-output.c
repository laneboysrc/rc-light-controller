#include "processor.h"

#define SPBRG_VALUE 104     // 38400 @ 16 MHz

extern unsigned char data;
struct {
    unsigned locked : 1;
    unsigned dataChanged : 1;
} flags;

void UART_send(void);
void UART_send_uint(void);
void UART_send_uchar(void);
unsigned char tx_value;
unsigned int tx_uint;


/*****************************************************************************
 Init_output()

 Called once during start-up of the firmware. 
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

	TXREG = '\n';
}


/*****************************************************************************
 Output_result()

 FIXME: change to pre-processor output; Output always 4 bytes?!
 

 Outputs the timer 1 value as decimal number, including leading zeros
 ****************************************************************************/
void Output_result(void) {
    if (flags.locked) {
        tx_uint = (TMR1H << 8) + TMR1L;
        UART_send_uint();
    }
}


/*****************************************************************************
 Send tx_uint as decimal number with leading zeros out via the UART
 ****************************************************************************/
void UART_send_uint(void) {
    tx_value = 0;
    while (tx_uint >= 9999) {
        tx_uint -= 10000;
        ++tx_value;
    }
    tx_value += '0';
    UART_send();

    tx_value = 0;
    while (tx_uint >= 1000) {
        tx_uint -= 1000;
        ++tx_value;
    }
    tx_value += '0';
    UART_send();
    
    tx_value = 0;
    while (tx_uint >= 100) {
        tx_uint -= 100;
        ++tx_value;
    }
    tx_value += '0';
    UART_send();

    tx_value = 0;
    while (tx_uint >= 10) {
        tx_uint -= 10;
        ++tx_value;
    }
    tx_value += '0';
    UART_send();

    tx_value = tx_uint + '0';
    UART_send();

    tx_value = '\n';
    UART_send();
}


/*****************************************************************************
 Send tx_value out via the UART
 ****************************************************************************/
void UART_send(void) {
    // Wait for TSR register being empty, then send the character in tx_value
    while (!TRMT);
    TXREG = tx_value;  
}

