;**********************************************************************
;
;   rc-light-controller.asm
;
;
;**********************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;**********************************************************************
    TITLE       RC Light Controller
    LIST        p=pic16f628a, r=dec
    RADIX       dec

    #include    <p16f628a.inc>

    __CONFIG _CP_OFF & _WDT_OFF & _BODEN_ON & _PWRTE_ON & _INTRC_OSC_NOCLKOUT & _MCLRE_OFF & _LVP_OFF



;**********************************************************************
;   The following outputs are needed:
;   - Stand light, tail light
;   - Head light
;   - High beam
;   - Fog light, fog rear light
;   - Indicators L + R
;   - Brake light
;   - Reversing light
;   - Servo output for steering
;
;
;   Inputs:
;   - Steering servo
;   - Throttle servo
;   - CH3 momentary
;
;
;   Modes that the SW must handle:
;   - Light mode:
;       - Stand light
;       - + Head light
;       - + Fog lights
;       - + High beam
;   - Drive mode:
;       - Neutral
;       - Forward
;       - Braking
;       - Reverse
;   - Indicators
;   - Steering servo pass-through
;   - Hazard lights
;
;
;   Other functions:
;   - Failsafe mode: all lights blink when no signal
;   - Program CH3
;   - Program TH neutral, full fwd, full rev
;   - Program servo output for steering servo: direction, neutral and end points
;
;
;   Slave unit is controlled via PPM (6 lights + 1 steering)
;
;
;   Operation:
;   - CH3
;       - short press if hazard lights: hazard lights off
;       - short press: cycle through light modes up
;       - double press: cycle through light modes down
;       - triple press: hazard lights
;       - long press: all lights off --> NOT POSSIBLE WITH GT-3B!
;
;
;   PPM:
;   Each pulse is 300us,
;   Data: 1000-2000 us full range, 1500 us = center (includes pulse width!)
;       Allow for +/- 10%: 900-2100 us
;   Repeated every 20ms
;   => 8 channels worst case: 8 * 2100 us =  16800 us
;   => space between transmissions minimum: -20000 = 3200 us
;   => 9 channels don't fit!
;
;
;   CH3 behaviour:
;   - Hard switch: on/off positions (i.e. HK 310)
;   - Toggle button: press=on, press again=off (i.e. GT-3B)
;   - Monentary button: press=on, release=off (in actual use ?)
;
;
;   Timing architecture:
;   ====================
;   We are dealing with 3 RC channels. A channel has a signal of max 2.1 ms
;   that is repeated every 20 ms. Hoever, there is no specification in which
;   sequence and with which timing the receiver outputs the individual channels.
;   Worst case there is only 6.85 ms between the channels. As such we can
;   not send the PPM signal synchronously between reading the channels.
;
;   Using interrupts may be critical too, as we need precision. Lets assume we
;   want to divide the servo range of 1 ms to 2 ms in 200 steps (100% in each
;   direction as e.g. EPA in transmitters works). This requires a resolution
;   of 5 us -- which is just 5 instruction cycles. No way to finish
;   processing interrupts this way.
;
;   So instead of PPM we could use UART where the PIC has HW support.
;   Or we could do one 20 ms measure the 3 channels, then in the next 20 ms
;   period send PPM.
;
;   UART at 57600 BAUD should be fast enough, even though we have increased
;   data rate (since we are sending data values). On the plus side this
;   allows for "digital" accuracy as there is no jitter in timing generation
;   (both sending and receiving) as there is with PPM).
;   UART can also use parity for added security against bit defects.
;
;   ****************************************
;   * CONCLUSION: let's use UART, not PPM! *
;   ****************************************
;
;
;   Measuring the 3 servo channels:
;   ===============================
;   The "Arduino OpenSourceLights" measure all 3 channels in turn, with a
;   21 ms timeout. This means worst one needs to wait 3 "rounds" each 20ms
;   until all 3 channels have been measured. That's 60 ms, which is still very
;   low (usually tact switches are de-bounced with 40 ms).
;
;   So the pseudo code should look like:
;
;       main
;           wait for CH3 = Low
;           wait for CH3 = High
;           start TMR1
;           wait for CH3 = Low
;           stop TMR1
;           save CH1 timing value
;
;           (repeat for CH2, CH1 (if present))
;
;           process channels
;           switch lights according to new mode
;           send lights and steering to slave via UART (3 bytes)
;
;           goto main
;
;
;   Robustness matters:
;   ===================
;   A servo signal should come every 20 ms. OpenSourceLights says in their
;   comment that they meassured "~20-22ms" between pulses. So for the safe
;   side let's assume that worst case we wait 25 ms for a servo pulse.
;
;   How to detect "failsafe"?
;   Since at minimum CH3 must be present, we use it to detect fail safe. If
;   no CH3 is received within 25 ms then we assume failure mode.
;
;   At startup we shall detect whether channels are present.
;   CH3 is always required, so we first wait for that to ensure the TX/RC are
;   on. Then we wait for TH and ST. If they don't appear we assume they are
;   not present.
;
;
;   UART protocol:
;   ==============
;   3 Bytes: SYNC, Lights, ST
;       SYNC:       Always 0x8x, which does not appear in the other values
;                   If a slave receives 0x8F the data is processed.
;                   If the value is 0x8x then it increments the value by 1 and
;                   sends all 3 received bytes at its output. This provides
;                   us with a simple way of daisy-chaining several slave
;                   modules!
;       Lights:     Each bit indicates a different light channel (0..6)
;       ST:         Steering servo data: -110 - 0 - +110
;
;
;
;
;   Flashing speed:
;   ===============
;   1.5 Hz = 333 ms per half-period
;
;
;**********************************************************************



;***** VARIABLE DEFINITIONS
    CBLOCK  0x20

	d1          ; Delay registers
	d2
	d3

    ENDC



;**********************************************************************
    ORG     0x000           ; Processor reset vector
    goto    Main            




;**********************************************************************
Main
    movlw   0x07
    movwf   CMCON       ; Disable the comparators

    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTB

    BANKSEL TRISA
    bcf     OPTION_REG, T0CS

    movlw   0x00        ; Make all ports A output
    movwf   TRISA
    movlw   b'00100110' ; Make all ports B output, except RB5, RB2 (UART!) and RB1 (UART!)
    movwf   TRISB


    ;-----------------------------
    ; UART specific initialization
OSC = d'4000000'        ; Osc frequency in Hz
BAUDRATE = d'19200'     ; Desired baudrate
BRGH_VALUE = 1          ; Either 0 or 1
SPBRG_VALUE = (((d'10'*OSC/((d'16'+d'48'*BRGH_VALUE)*BAUDRATE))+d'5')/d'10')-1

    movlw   b'00100000'
            ; |||||||+ TX9D (not used)
            ; ||||||+- TRMT (read only)
            ; |||||+-- BRGH (high baud rate generator)
            ; ||||+---      (not implemented)
            ; |||+---- SYNC (cleared to select async mode)
            ; ||+----- TXEN (set to enable transmit function)
            ; |+------ TX9  (cleared to use 8 bit mode = no parity)
            ; +------- CSRC (not used in async mode)
    movwf   TXSTA

    IF (BRGH_VALUE == 1)
        bsf TXSTA, BRGH
    ELSE
        bcf TXSTA, BRGH
    ENDIF
    movlw	SPBRG_VALUE
    movwf	SPBRG

    BANKSEL RCSTA
    movlw   b'10010000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (set to enable constant reception)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG


    ;**********************************************************************
main_loop
    call    Delay_2s
    
                            ; Send 'Hello world\n'
    movlw   0x48
    call    UART_send_w
    movlw   0x65
    call    UART_send_w
    movlw   0x6C
    call    UART_send_w
    movlw   0x6C
    call    UART_send_w
    movlw   0x6F
    call    UART_send_w
    movlw   0x20
    call    UART_send_w
    movlw   0x57
    call    UART_send_w
    movlw   0x6F
    call    UART_send_w
    movlw   0x72
    call    UART_send_w
    movlw   0x6C
    call    UART_send_w
    movlw   0x64
    call    UART_send_w
    movlw   0x0a
    call    UART_send_w


    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal oscillator
    clrf    TMR1H
    clrf    TMR1L

    btfss   PORTB, 5    ; Wait until servo signal is high
    goto    main_loop


    movlw   0x01        ; Start timer 1
    movwf   T1CON

wait_for_low
    btfsc   PORTB, 5    ; Wait until servo signal is LOW
    goto    wait_for_low


    clrf    T1CON       ; Stop timer 1

    movf    TMR1H, 0    ; 1500ms = 0x5DC
    sublw   0x05
    btfss   STATUS, 0   ; Carry/!Borrow bit
    goto    is_larger

    movf    TMR1L, 0
    sublw   0xdc
    btfss   STATUS, 0   ; Carry/!Borrow bit
    goto    is_larger

    bsf     PORTA, 0
    goto    main_loop

is_larger
    bcf     PORTA, 0
    goto    main_loop



;**********************************************************************
Delay_2.1ms
    movlw   D'3'
    movwf   d2
    movlw   D'185'
    movwf   d1
    goto    delay_loop

Delay_0.9ms
    movlw   D'2'
    movwf   d2
    movlw   D'40'
    movwf   d1
delay_loop
    decfsz  d1, f
    goto    delay_loop
    decfsz  d2, f
    goto    delay_loop
    return


;**********************************************************************
Delay_2s
	movlw	0x11
	movwf	d1
	movlw	0x5D
	movwf	d2
	movlw	0x05
	movwf	d3
delay_0
	decfsz	d1, f
	goto	$ + 2
	decfsz	d2, f
	goto	$ + 2
	decfsz	d3, f
	goto	delay_0
    return


;**********************************************************************
; Send W out via the UART
;**********************************************************************
UART_send_w
    btfss   PIR1, TXIF
    goto    UART_send_w ; Wait for transmitter interrupt flag

    movwf   TXREG	    ; Send data stored in W
    return      



    END     ; Directive 'end of program'
