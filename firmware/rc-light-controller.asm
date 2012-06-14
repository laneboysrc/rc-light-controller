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
;   - "Lichthupe"?
;
;
;   Other functions:
;   - Failsafe mode: all lights blink when no signal
;   - Program CH3
;   - Program TH neutral, full fwd, full rev
;   - Program steering servo: neutral and end points
;   - Program servo output for steering servo: direction, neutral and end points
;
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
;       Allow for: 770-2350 us
;   Repeated every 20ms
;   => 8 channels worst case: 8 * 2100 us =  16800 us
;   => space between transmissions minimum: -20000 = 3200 us
;   => 9 channels don't fit!
;
;
;   CH3 behaviour:
;   - Hard switch: on/off positions (i.e. HK 310)
;   - Toggle button: press=on, press again=off (i.e. GT-3B)
;   - Momentary button: press=on, release=off (in actual use ?)
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
;       SYNC:       Always 0x80..0x87, which does not appear in the other values
;                   If a slave receives 0x87 the data is processed.
;                   If the value is 0x86..0x80 then it increments the value 
;                   by 1 and sends all 3 received bytes at its output. 
;                   This provides us with a simple way of daisy-chaining 
;                   several slave modules!
;                   NOTE: this means that the steering servo can only be
;                   connected to the last slave module!
;       Lights:     Each bit indicates a different light channel (0..6)
;       ST:         Steering servo data: -120 - 0 - +120
;
;
;
;   Flashing speed:
;   ===============
;   1.5 Hz = 333 ms per half-period
;
;
;   Steering servo:
;   ===============
;   We need to be concerned with quite some things:
;   - Neutral/Trim of both servos
;   - Direction of the steering servo
;   - End point adjustment of both steering servos
;   
;   The best way will be to approach to first calibrate the original steering
;   servo trim and end points. 
;   Then do a 3 step calibration for the salve steering servo neutral and
;   end points.
;
;   Ideally there should be a simple way to re-calibrate the original steering
;   servo. 
;       - Neutral may be automatically done each time the system is powered on
;           Or each time the light mode is changed; assuming that the user
;           does not operate both controls at the same time. Maybe to ensure
;           that add a safety that only +/- 10% are automatically adjustable?
;       - End points can be automatically adjusted if a value is received
;         that is larger than the current stored end point. To reduce we need
;         to check over a long period of time.
;   Conclusion: maybe for the first version this is overkill, just allow
;   for manual calibration.
;
;
;   TX/RX system findings:
;   ======================
;   In general the jitter is 3 us, which is expected given that it takes about
;   3 instructions to detect a port change and start/stop the timer.
;
;   GT3B: 
;       EPA can be +/- 120 %
;       Normal range (trim zero, 100% EPA): 986 .. 1568 .. 2120
;       Trim range: 1490 .. 1568 .. 1649  (L30 .. N00 .. R30)
;       Worst case with full EPA and trim: 870 .. 2300 (!)
;       Failsafe: Servo signal holds for about 500ms, then stops
;       CH3: 1058, 2114
;
;   HK-310:
;       EPA can be +/- 120 %
;       Normal range (sub-trim and trim zero, 100% EPA): 1073 .. 1568 .. 2117
;       Sub-Trim range: 1232 .. 1565 .. 1901  (-100 .. 0 .. 100)
;       Trim range: 1388 .. 1568 .. 1745
;       Worst case with full EPA and sub-tirm and trim: 779 .. 2327 (!)
;       Failsafe: Continously sends ST centre, TH off, CH3 holds last value
;       CH3: 1013 = AUX, 2120 = OFF; fai
;
;**********************************************************************



;***** VARIABLE DEFINITIONS
    CBLOCK  0x20

	d1          ; Delay registers
	d2
	d3

    temp
    send_hi
    send_lo

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
BAUDRATE = d'38400'     ; Desired baudrate
BRGH_VALUE = 1          ; Either 0 or 1
SPBRG_VALUE = (((d'10'*OSC/((d'64'-(d'48'*BRGH_VALUE))*BAUDRATE))+d'5')/d'10')-1

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
    IF 0
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

    ENDIF

    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal oscillator
    clrf    TMR1H
    clrf    TMR1L


    ; Wait until servo signal is LOW 
    ; This ensures that we do not start in the middle
    ;  of a pulse
ch3_wait_for_low1
    btfsc   PORTB, 5    
    goto    ch3_wait_for_low1

ch3_wait_for_high
    btfss   PORTB, 5    ; Wait until servo signal is high
    goto    ch3_wait_for_high

    bsf     T1CON, 0    ; Start timer 1

ch3_wait_for_low2
    btfsc   PORTB, 5    ; Wait until servo signal is LOW
    goto    ch3_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    movf    TMR1H, w    ; 1500ms = 0x5DC
    movwf   send_hi
    movf    TMR1L, w
    movwf   send_lo
    call UART_send_16bit
    goto    main_loop

    addlw   0x30
    call    UART_send_w
    goto    main_loop



    sublw   0x05
    btfss   STATUS, 0   ; Carry/!Borrow bit
    goto    is_larger

    movf    TMR1L, w
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


;**********************************************************************
; Send a 16 bit value stored in hi and lo as a number over the UART
;**********************************************************************
UART_send_16bit
        clrf temp
sub30k
        movlw 3
        addwf temp, f
        movlw low(30000)
        subwf send_lo, f

        movlw high(30000)
        skpc
        movlw high(30000) + 1
        subwf send_hi, f
        skpnc
        goto sub30k
add10k
        decf temp, f
        movlw low(10000)
        addwf send_lo, f

        movlw high(10000)
        skpnc
        movlw high(10000) + 1
        addwf send_hi, f
        skpc
        goto add10k
        movf    temp, w
        addlw   0x30
        call    UART_send_w

        clrf temp
sub3k
        movlw 3
        addwf temp, f
        movlw low(3000)
        subwf send_lo, f
        movlw high(3000)
        skpc
        movlw high(3000) + 1
        subwf send_hi, f
        skpnc
        goto sub3k
add1k
        decf temp, f
        movlw low(1000)
        addwf send_lo, f

        movlw high(1000)
        skpnc
        movlw high(1000) + 1
        addwf send_hi, f
        skpc
        goto add1k
        movf    temp, w
        addlw   0x30
        call    UART_send_w


        clrf temp
sub300
        movlw 3
        addwf temp, f
        movlw low(300)
        subwf send_lo, f
        movlw high(300)
        skpc
        movlw high(300) + 1
        subwf send_hi, f
        skpnc
        goto sub300
        movlw 100
add100
        decf temp, f
        addwf send_lo, f
        skpc
        goto add100
        incf send_hi, f
        btfsc send_hi, 7
        goto add100
        movf    temp, w
        addlw   0x30
        call    UART_send_w

        clrf temp
        movlw 30
sub30
        incf temp, f
        subwf send_lo, f
        skpnc
        goto sub30
        movfw temp
        rlf temp, f
        addwf temp, f
        movlw 10
add10
    decf temp, f
    addwf send_lo, f
    skpc
    goto add10
    movf    temp, w
    addlw   0x30
    call    UART_send_w

    movf    send_lo, w
    addlw   0x30
    call    UART_send_w
    movlw   0x0a
    call    UART_send_w

    return

    END     ; Directive 'end of program'
