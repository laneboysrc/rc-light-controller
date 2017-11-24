#include <LPC8xx.h>
#include <hal.h>

void hal_servo_output_init(void)
{
    LPC_SCT->CONFIG |= (1 << 18);           // Auto-limit on counter H
    LPC_SCT->CTRL_H |= (1 << 3) |           // Clear the counter H
                       (11 << 5);           // PRE_H[12:5] = 12-1 (SCTimer H clock 1 MHz)
    LPC_SCT->MATCHREL[0].H = 20000 - 1;     // 20 ms per overflow (50 Hz)
    LPC_SCT->MATCHREL[4].H = 1500;          // Servo pulse 1.5 ms intially

    LPC_SCT->EVENT[0].STATE = 0xFFFF;       // Event 0 happens in all states
    LPC_SCT->EVENT[0].CTRL = (0 << 0) |     // Match register 0
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    LPC_SCT->EVENT[4].STATE = 0xFFFF;       // Event 4 happens in all states
    LPC_SCT->EVENT[4].CTRL = (4 << 0) |     // Match register 4
                             (1 << 4) |     // Select H counter
                             (0x1 << 12);   // Match condition only

    // We've chosen CTOUT_1 because CTOUT_0 resides in PINASSIGN6, which
    // changing may affect CTIN_1..3 that we need.
    // CTOUT_1 is in PINASSIGN7, where no other function is needed for our
    // application.
    LPC_SCT->OUT[1].SET = (1 << 0);        // Event 0 will set CTOUT_1
    LPC_SCT->OUT[1].CLR = (1 << 4);        // Event 4 will clear CTOUT_1

    // CTOUT_1 = PIO0_12
    LPC_SWM->PINASSIGN7 = 0xffffff0c;

    LPC_SCT->CTRL_H &= ~(1 << 2);          // Start the SCTimer H
}

// Put the servo pulse duration in milliseconds into the match register
// to output the pulse of the given duration.
void hal_servo_output_set_pulse(uint16_t servo_pulse)
{
    LPC_SCT->MATCHREL[4].H = servo_pulse;
}

void hal_servo_output_enable(void)
{
    // Re-enable event 0 to set CTOUT_1
    LPC_SCT->OUT[1].SET = (1 << 0);
}

void hal_servo_output_disable(void)
{
    // Turn off the setting of CTOUT_1, so no pulse will be generated. However,
    // clearing of CTOUT_1 is still active through event 0, so if a pulse is
    // currently active it will be nicely terminated, and from the next period
    // onwards the pulses will cease.
    LPC_SCT->OUT[1].SET = 0;
}