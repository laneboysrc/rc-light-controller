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

	    cblock	h'20'	;bank 0  h'20' to h'7f'. 96 locations

	    ;these allow us to save the context during interrupts.
savew1	;SAVEW1 *MUST* be at location h'20'!
savestatus	
savepclath
savefsr

CounterA
CounterB
CounterC

rx_data		;the received byte from the serial UART
tx_data		;byte to be transmitted via UART

	    endc



;**********************************************************************
    org     0x000           ; processor reset vector

    goto    Main            ; go to beginning of program





;**********************************************************************
Main
    movlw	0x07
    movwf	CMCON		; disable the comparators

    clrf	PORTA		; set all (output) ports to GND
    clrf	PORTB	

    banksel	TRISB
    bcf		OPTION_REG, T0CS	

    movlw	0x00		; make all ports A output
    movwf	TRISA
    movlw	0x26		; make all ports B output, except RB5, RB2 (UART!) and RB1 (UART!)
    movwf	TRISB


    ; UART specific initialization
    bcf	TXSTA, CSRC	; <7> (0) don't care in asynch mode
    bcf	TXSTA, TX9	; <6>  0  select 8 bit mode
    bsf	TXSTA, TXEN	; <5>  1  enable transmit function 
			    ;      *MUST* be 1 for transmit to work!!!
    bcf	TXSTA, SYNC	; <4>  0 asynchronous mode. 
			    ;      *MUST* be 0 !!!
			    ;      If NOT 0 the async mode is NOT selected!
			    ; <3>  (0) not implemented
    bcf	TXSTA, BRGH	; <2>  0 disable high baud rate generator !!!
			    ; <1>    (0) trmt is read only.
    bcf	TXSTA, TX9D	; <0>  (0)  tx9d data cleared to 0.


xtal_freq	=   d'4000000'	;crystal frequency in Hertz.
baudrate	=	d'2400'	;desired baudrate.
;spbrg_value =	(((d'10'*xtal_freq/(d'16'*baudrate))+d'5')/d'10')-1
			    ;this is based on txsta,brgh=1.
spbrg_value	=	(((d'10'*xtal_freq/(d'64'*baudrate))+d'5')/d'10')-1
			    ;this is based on txsta,brgh=0.

    movlw	spbrg_value	
    movwf	SPBRG


    banksel	RCSTA

;more uart specific initialization

			    ;rcsta=ReCeive STAtus and control register
			    ;assume nothing.

    bsf	RCSTA, SPEN	; 7 spen 1=rx/tx set for serial uart mode
			    ;   !!! very important to set spen=1
    bcf	RCSTA, RX9	; 6 rc8/9 0=8 bit mode
    bcf	RCSTA, SREN	; 5 sren 0=don't care in uart mode
    bsf	RCSTA, CREN	; 4 cren 1=enable constant reception
			    ;!!! (and low clears errors)
			    ; 3 not used / 0 / don't care
    bcf	RCSTA, FERR	; 2 ferr input framing error bit. 1=error
			    ; 1 oerr input overrun error bit. 1=error
			    ;!!! (reset oerr by neg pulse clearing cren)
			    ;you can't clear this bit by using bcf.
			    ;It is only cleared when you pulse cren low. 
    bcf	RCSTA, RX9D	; 0 rx9d input (9th data bit). ignore.



    movf	RCREG,w		;clear uart receiver
    movf	RCREG,w		; including fifo
    movf	RCREG,w		; which is three deep.


    movlw	0		;any character will do.
    movwf	TXREG		;send out dummy character
			    ; to get transmit flag valid!



    banksel	PORTA
    bcf		PORTA, 0


    ; main loop 
main_loop
    ; Send 'Hello world\n'
    movlw   0x48          
    call	transmitw	;send W to the UART transmitter
    movlw   0x65
    call	transmitw	
    movlw   0x6C
    call	transmitw	
    movlw   0x6C
    call	transmitw	
    movlw   0x6F
    call	transmitw	
    movlw   0x20
    call	transmitw	
    movlw   0x57
    call	transmitw	
    movlw   0x6F
    call	transmitw	
    movlw   0x72
    call	transmitw	
    movlw   0x6C
    call	transmitw	
    movlw   0x64
    call	transmitw	
    movlw   0x0a
    call	transmitw	


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

    movf    TMR1H, 0   ; 1500ms = 0x5DC
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
delay_2.1ms
    movlw	D'3'
    movwf	CounterB
    movlw	D'185'
    movwf	CounterA
    goto	delay_loop

delay_0.9ms
    movlw	D'2'
    movwf	CounterB
    movlw	D'40'
    movwf	CounterA
delay_loop
    decfsz	CounterA,1
    goto	delay_loop 
    decfsz	CounterB,1
    goto	delay_loop 
    return


;**********************************************************************
transmitw
			    ;transmitw is most common entry point.
			    ;(output what is in W)
    btfss	PIR1, TXIF
    goto	transmitw	;wait for transmitter interrupt flag
gietx	
;    bcf	INTCON, GIE	;disable interrupts
;	btfsc	INTCON, GIE	;making SURE they are disabled!
;	goto	gietx
    movwf	TXREG	;load data to be sent...
;	bsf	INTCON, GIE	;re-enable interrupts
    return			;tx_data unchanged. 
			    ;transmitted data is in W



    END			; directive 'end of program'
