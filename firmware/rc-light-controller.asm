;******************************************************************************
;
;   rc-light-controller.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller
    LIST        p=pic16f628a, r=dec
    RADIX       dec

    #include    <p16f628a.inc>

    __CONFIG _CP_OFF & _WDT_OFF & _BODEN_ON & _PWRTE_ON & _INTRC_OSC_NOCLKOUT & _MCLRE_OFF & _LVP_OFF


;******************************************************************************
;******************************************************************************
;******************************************************************************
; TODO:
;
; Test steering servo programming
; LEDs during steering servo programming
;
;******************************************************************************
;******************************************************************************
;******************************************************************************


;******************************************************************************
;   Port usage:
;   ===========
;   RB6, RB1:   IN  Servo input ST (PGC and RX for slave double-usage)
;   RB7:        IN  Servo input TH (PGD double-usage)
;   RA5:        IN  Servo input CH3 (Vpp double-usage)
;   RB2:        OUT Slave out (TX Master) / Servo out (Slave) 
;
;   RA3:        OUT CLK TLC5916
;   RA4:        OUT SDI TLC5916
;   RA2:        OUT LE TLC5916
;   RB0:        OUT OE TLC5916
;
;   RA7, RB3:   IN  Tied to +Vdd for routing convenience!
;   RB5         IN  RB5 is tied to RB2 for routing convenience!
;   RA6, RA0, RA1, RB4:     OUT NC pins, switch to output

#define PORT_CH3        PORTA, 5
#define PORT_STEERING   PORTB, 6
#define PORT_THROTTLE   PORTB, 7

; TLC5916 LED driver serial communication ports
#define PORT_CLK        PORTA, 3
#define PORT_SDI        PORTA, 4
#define PORT_LE         PORTA, 2
#define PORT_OE         PORTB, 0


#define CH3_BUTTON_TIMEOUT 6    ; Time in which we accept double-click of CH3
#define BLINK_COUNTER_VALUE 5   ; 5 * 65.536 ms = ~333 ms = ~1.5 Hz
#define BRAKE_AFTER_REVERSE_COUNTER_VALUE 20 ; 30 * 65.536 ms = ~2 s
#define BRAKE_DISARM_COUNTER_VALUE 30        ; 30 * 65.536 ms = ~2 s
#define INDICATOR_STATE_COUNTER_VALUE 15     ; 15 * 65.536 ms = ~1 s
#define INDICATOR_STATE_COUNTER_VALUE_OFF 30 ; ~2 s

; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_STAND 0          ; Stand lights
#define LIGHT_MODE_HEAD 1           ; Head lights
#define LIGHT_MODE_FOG 2            ; Fog lights
#define LIGHT_MODE_HIGH_BEAM 3      ; High beam

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2
#define DRIVE_MODE_BRAKE_ARMED 3
#define DRIVE_MODE_REVERSE_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5

#define CENTRE_THRESHOLD 10
#define STEERING_BLINK_THRESHOLD 50
#define STEERING_BLINK_OFF_THRESHOLD 30

#define EEPROM_MAGIC1 0x55
#define EEPROM_MAGIC2 0xAA

#define EEPROM_ADR_MAGIC1 0      
#define EEPROM_ADR_MAGIC2 4
#define EEPROM_ADR_SERVO_EPL 1
#define EEPROM_ADR_SERVO_CENTRE 2
#define EEPROM_ADR_SERVO_EPR 3

; Bitfields in variable setup_mode
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
    CBLOCK  0x20

    throttle
    throttle_abs
    throttle_l
    throttle_h
    throttle_centre_l
    throttle_centre_h
    throttle_epl_l
    throttle_epl_h
    throttle_epr_l
    throttle_epr_h
    throttle_reverse

    steering
    steering_abs
    steering_l
    steering_h
    steering_centre_l
    steering_centre_h
    steering_epl_l
    steering_epl_h
    steering_epr_l
    steering_epr_h
    steering_reverse
    
    ch3
    ch3_value
    ch3_ep0
    ch3_ep1

    drive_mode_counter
    drive_mode_brake_disarm_counter   
    indicator_state_counter
    blink_counter
    ch3_click_counter
    ch3_clicks

    blink_mode      
    light_mode
    drive_mode
    indicator_state
    setup_mode

    servo
    servo_epl
    servo_centre
    servo_epr
    servo_setup_epl
    servo_setup_centre
    servo_setup_epr

	d0          ; Delay and temp registers
	d1
	d2
	d3
    temp

    wl          ; Temporary parameters for 16 bit math functions
    wh
    xl
    xh
    yl 
    yh
    zl
    zh

    send_hi
    send_lo

    debug_steering_old
    debug_throttle_old
    debug_indicator_state_old

    ENDC


;******************************************************************************
;* MACROS
;******************************************************************************
swap_x_y    macro   x, y
    ; Currently X contains A; Y contains B
    movf  x, w      ; W = A
    XORWF y, w      ; W = A ^ B
    XORWF x, f      ; X = ((A^B)^A) = B
    XORWF y, f      ; Y = ((A^B)^B) = A
    ; Now X contains B. Y contains A.
            endm



;******************************************************************************
; Reset vector 
;******************************************************************************
    ORG     0x000           
    goto    Init


;******************************************************************************
; Relocatable code section
;******************************************************************************
    CODE

;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; Light configuration
;
;******************************************************************************
;******************************************************************************
;******************************************************************************
    EXTERN local_light_table
    EXTERN slave_light_table
    EXTERN slave_light_half_table

light_table
    ; d0 indicates which light table we request:
    ;   0: local
    ;   1: slave
    ;   2: slave_half
    btfsc   d0, 0               
    goto    slave_light_table
    btfsc   d0, 1
    goto    slave_light_half_table
    goto    local_light_table


;******************************************************************************
; Initialization
;******************************************************************************
Init
    BANKSEL CMCON
    movlw   0x07
    movwf   CMCON       ; Disable the comparators

    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTB


    BANKSEL OPTION_REG
    movlw   b'10000111'
            ; |||||||+ PS0  (Set pre-scaler to 1:256)
            ; ||||||+- PS1
            ; |||||+-- PS2
            ; ||||+--- PSA  (Use pre-scaler for Timer 0)
            ; |||+---- T0SE (not used when Timer 0 uses internal osc)
            ; ||+----- T0CS (Timer 0 to use internal oscillator)
            ; |+------ INTEDG (not used in this application)
            ; +------- RBPU (Disable Port B pull-ups)
    movwf   OPTION_REG

    bcf     INTCON, T0IF    ; Clear Timer 0 Interrupt Flag    


    ;-----------------------------
    ; Port direction
    movlw   b'10100000' ; Make all ports A exceot RA7 and RA5 output
    movwf   TRISA

    movlw   b'11101110' ; Make RB7, RB6, RB5, RB3, RB2 and RB1 inputs
    movwf   TRISB


    BANKSEL d0
    ;-----------------------------
    ; Clear all memory locations between 0x20 and 0x7f
    movlw   0x7f
	movwf	FSR
	movwf	0x20		; Store a non-zero value in the last RAM address we
                        ;  like to clear
clear_ram	
    decf	FSR, f		
	clrf	INDF		; Clear Indirect memory location
	movfw	0x20		; If we reached the first RAM location it will be 0 now,
    skpz                ;  so we are done!
	goto	clear_ram   


    ; Load defaults for end points for position 0 and 1 of CH3; discard lower
    ; 4 bits so our math can use bytes only
    movlw   1000 >> 4
    movwf   ch3_ep0

    movlw   2000 >> 4
    movwf   ch3_ep1


    BANKSEL TXSTA
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
    movlw   b'10000000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (disable reception for MASTER)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG



    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter

    movlw   HIGH(1500)
    movwf   throttle_centre_h
    movwf   steering_centre_h
    movlw   LOW(1500)
    movwf   throttle_centre_l
    movwf   steering_centre_l

    movlw   HIGH(1400)
    movwf   throttle_epl_h
    movwf   steering_epl_h
    movlw   LOW(1400)
    movwf   throttle_epl_l
    movwf   steering_epl_l

    movlw   HIGH(1600)
    movwf   throttle_epr_h
    movwf   steering_epr_h
    movlw   LOW(1600)
    movwf   throttle_epr_l
    movwf   steering_epr_l

    ; Steering is reversed for the Dingo (not auto-adjust yet)
    movlw   1                   
    movwf   steering_reverse

    ; Load steering servo values from the EEPROM
    call    Servo_load_values

    ;------------------------------------
    ; Initialize neutral for steering and throttle 2 seconds after power up
    ; During this time we use all local LED outputs as running lights.
	movlw	0x11
	movwf	d1
	movlw	0x5D
	movwf	d2
	movlw	0x05
	movwf	d3

    clrf    temp
    call    TLC5916_send
    clrf    xl
    setc

init_delay
	decfsz	d1, f
	goto	$ + 2
	decfsz	d2, f
	goto	$ + 6
    rrf     xl, f
    movf    xl, w
    movwf   temp
    call    TLC5916_send
	decfsz	d3, f
	goto	init_delay
    return


    ;------------------------------------
    call    Read_throttle
    movf    throttle_h, w
    movwf   throttle_centre_h
    movf    throttle_l, w
    movwf   throttle_centre_l

    call    Read_steering
    movf    steering_h, w
    movwf   steering_centre_h
    movf    steering_l, w
    movwf   steering_centre_l

;   goto    Main_loop    


;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_ch3
    call    Read_throttle
    call    Read_steering

    call    Process_ch3
    call    Process_throttle
    call    Process_steering

    call    Process_ch3_double_click
    call    Process_drive_mode
    call    Process_indicators
    call    Process_steering_servo
    call    Service_timer0

    call    Debug_output_values

    call    Output_local_lights
;    call    Output_slave

    goto    Main_loop


;******************************************************************************
; Output_local_lights
;******************************************************************************
Output_local_lights
    clrf    d0
    call    Output_get_state
    call    TLC5916_send
    return


;******************************************************************************
; Output_slave
;
;******************************************************************************
Output_slave
    ; Forward the information to the slave
    movlw   0x87            ; Magic byte for synchronization
    call    UART_send_w        

    movlw   1
    movwf   d0
    call    Output_get_state
    movf    temp, w         ; LED data for full brightness
    call    UART_send_w        

    movlw   2
    movwf   d0
    call    Output_get_state
    movf    temp, w         ; LED data for half brightness
    call    UART_send_w        

    movf    servo, w        ; Steering wheel servo data
    call    UART_send_w        

    return


;******************************************************************************
; Output_get_state
;
;******************************************************************************
Output_get_state
    clrf    temp

    ; Stand lights
    btfss   light_mode, LIGHT_MODE_STAND
    goto    output_local_get_state_head
    movlw   0
    call    light_table
    iorwf   temp, f

    ; Head lights
output_local_get_state_head
    btfss   light_mode, LIGHT_MODE_HEAD
    goto    output_local_get_state_fog
    movlw   1
    call    light_table
    iorwf   temp, f

    ; Fog lights    
output_local_get_state_fog
    btfss   light_mode, LIGHT_MODE_FOG
    goto    output_local_get_state_high_beam
    movlw   2
    call    light_table
    iorwf   temp, f

    ; High beam    
output_local_get_state_high_beam
    btfss   light_mode, LIGHT_MODE_HIGH_BEAM
    goto    output_local_get_state_brake
    movlw   3
    call    light_table
    iorwf   temp, f

    ; Brake lights    
output_local_get_state_brake
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    output_local_get_state_reverse
    movlw   4
    call    light_table
    iorwf   temp, f

    ; Reverse lights        
output_local_get_state_reverse
    btfss   drive_mode, DRIVE_MODE_REVERSE
    goto    output_local_get_state_indicator_left
    movlw   5
    call    light_table
    iorwf   temp, f

    ; Indicator left    
output_local_get_state_indicator_left
    ; Skip all indicators and hazard lights if blink flag is in off period
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_local_get_state_end

    btfss   blink_mode, BLINK_MODE_INDICATOR_LEFT
    goto    output_local_get_state_indicator_right
    movlw   6
    call    light_table
    iorwf   temp, f
    
    ; Indicator right
output_local_get_state_indicator_right
    btfss   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    goto    output_local_get_state_hazard
    movlw   7
    call    light_table
    iorwf   temp, f
   
    ; Hazard lights 
output_local_get_state_hazard
    btfss   blink_mode, BLINK_MODE_HAZARD
    goto    output_local_get_state_end
    movlw   8
    call    light_table
    iorwf   temp, f

output_local_get_state_end
    return


;******************************************************************************
; Service_timer0
;
; Soft-timer with a resolution of 65.536 ms
;******************************************************************************
Service_timer0
    btfss   INTCON, T0IF
    return

    bcf     INTCON, T0IF

    movf    ch3_click_counter, f
    skpz     
    decf    ch3_click_counter, f    

    movf    indicator_state_counter, f
    skpz     
    decf    indicator_state_counter, f    


    decfsz  drive_mode_brake_disarm_counter, f
    goto    service_timer0_drive_mode

    btfss   drive_mode, DRIVE_MODE_BRAKE_DISARM
    goto    service_timer0_drive_mode

    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED


service_timer0_drive_mode
    decfsz  drive_mode_counter, f
    goto    service_timer0_blink

    btfss   drive_mode, DRIVE_MODE_REVERSE_BRAKE
    goto    service_timer0_blink

    bcf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE


service_timer0_blink
    decfsz  blink_counter, f
    return

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    movlw   1 << BLINK_MODE_BLINKFLAG
    xorwf   blink_mode, f

    

    return


;******************************************************************************
; Synchronize_blinking
;
; This function ensures that blinking always starts with a full "on" period.
; It resets the blink counter and sets the blink flag, but only if none
; of hazard and indicator are already on (= blinking)
;******************************************************************************
Synchronize_blinking
    btfsc   blink_mode, BLINK_MODE_HAZARD
    return
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    return
    btfsc   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    return

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    bsf     blink_mode, BLINK_MODE_BLINKFLAG
    return


;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; CH3 related functions
;
;******************************************************************************
;******************************************************************************
;******************************************************************************



;******************************************************************************
; Read_ch3
; 
; Read servo channel 3 and write the result in ch3_h/ch3_l
;******************************************************************************
Read_ch3
    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H       ; Reset the timer to 0
    clrf    TMR1L
    clrf    ch3_value   ; Prime the result with "timing measurement failed"

    ; Wait until servo signal is LOW 
    ; This ensures that we do not start in the middle of a pulse
ch3_wait_for_low1
    btfsc   PORT_CH3
    goto    ch3_wait_for_low1

ch3_wait_for_high
    btfss   PORT_CH3    ; Wait until servo signal is high
    goto    ch3_wait_for_high

    bsf     T1CON, 0    ; Start timer 1

ch3_wait_for_low2
    btfsc   PORT_CH3    ; Wait until servo signal is LOW
    goto    ch3_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    call    Validate_servo_measurement
  
    ; Use the middle 12 bit as an 8 bit value since we don't need high
    ; accuracy for the CH3 
    rlf     xl, f
    rlf     xh, f
    rlf     xl, f
    rlf     xh, f
    rlf     xl, f
    rlf     xh, f
    rlf     xl, f
    rlf     xh, f

    movf    xh, w    
    movwf   ch3_value

    return


;******************************************************************************
; Process_ch3
; 
; Normalize the processed CH3 channel into ch3 value 0 or 1.
;
; Algorithm:
;
; Switch position 0 stored in ch3_ep0: 1000 us 
; Switch position 1 stored in ch3_ep1: 2000 is
;   Note: these values can be changed through the setup procedure to adjust
;   to a specific TX/RX.
;
; Center is therefore   (2000 + 1000) / 2 = 1500 us
; Hysteresis:           (2000 - 1000) / 8 = 125 us
;   Note: divide by 8 was chosen for simplicity of implementation
; If last switch position was pos 0:
;   measured timing must be larger than 1500 + 125 = 1625 us to accept as pos 1
; If last switch position was pos 1:
;   measured timing must be larger than 1500 - 125 = 1375 us to accept as pos 0
;
; Note: calculation must ensure that due to servo reversing pos 0 may
; have a larger or smaller time value than pos 1.
;******************************************************************************
#define ch3_centre d1
#define ch3_hysteresis d2   

Process_ch3
    ; Step 1: calculate the centre: (ep0 + ep1) / 2
    ; To avoid potential overflow we actually calculate (ep0 / 2) + (ep1 / 2)
    movf    ch3_ep0, w
    movwf   ch3_centre
    clrc
    rrf     ch3_centre, f

    movf    ch3_ep1, w
    movwf   temp
    clrc
    rrf     temp, w
    addwf   ch3_centre, f
    
    ; Step 2: calculate the hysteresis: (max(ep0, ep1) - min(ep0, ep1)) / 8
    movf    ch3_ep0, w
    movwf   temp
    movf    ch3_ep1, w
    call    Max
    movwf   ch3_hysteresis

    movf    ch3_ep0, w
    movwf   temp
    movf    ch3_ep1, w
    call    Min
    subwf   ch3_hysteresis, f
    clrc
    rrf     ch3_hysteresis, f
    clrc
    rrf     ch3_hysteresis, f
    clrc
    rrf     ch3_hysteresis, f

    ; Step 3: Depending on whether CH3 was previously set we have to 
    ; test for the positive or negative hysteresis around the centre. In
    ; addition we have to utilize positive or negative hysteresis depending
    ; on which end point is larger in value (to support reversed channels)
    btfss   ch3, 0
    goto    process_ch3_pos0

    ; CH3 was in pos 1; check if we need to use the positive (ch reversed) or 
    ; negative (ch normal) hysteresis
    movf    ch3_ep1, w
    subwf   ch3_ep0, w
    skpnc
    goto    process_ch3_higher
    goto    process_ch3_lower

process_ch3_pos0
    ; CH3 was in pos 0; check if we need to use the positive (ch normal) or 
    ; negative (ch reversed) hysteresis
    movf    ch3_ep1, w
    subwf   ch3_ep0, w
    skpnc
    goto    process_ch3_lower
;   goto    process_ch3_higher

process_ch3_higher
    ; Add the hysteresis to the centre. Then subtract it from the current 
    ; ch3 value. If it is smaller C will be set and we treat it to toggle
    ; channel 3.
    movf    ch3_centre, w
    addwf   ch3_hysteresis, w
    subwf   ch3_value, w
    skpc    
    return
    goto    process_ch3_toggle

process_ch3_lower
    ; Subtract the hysteresis to the centre. Then subtract it from the current 
    ; ch3 value. If it is larger C will be set and we treat it to toggle
    ; channel 3.
    movf    ch3_hysteresis, w
    subwf   ch3_centre, w
    subwf   ch3_value, w
    skpnc    
    return

process_ch3_toggle
    ; Toggle bit 0 of ch3 to change between pos 0 and pos 1
    movlw   1
    xorwf   ch3, f
    bsf     ch3, 1
    return


;******************************************************************************
; Process_ch3_double_click
;******************************************************************************
Process_ch3_double_click
    btfsc   ch3, 7
    goto    process_ch3_initialized

    ; Ignore the potential "toggle" after power on
    bsf     ch3, 7
    bcf     ch3, 1
    return

process_ch3_initialized
    btfss   ch3, 1
    goto    process_ch3_click_timeout

    bcf     ch3, 1
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter

    movlw   0x43                    ; send 'C'
    call    UART_send_w        
    return
    
process_ch3_click_timeout
    movf    ch3_clicks, f           ; Any buttons pending?
    skpnz   
    return                          ; No: done

    movf    ch3_click_counter, f    ; Double-click timer expired?
    skpz   
    return                          ; No: wait for more buttons


    movf    setup_mode, f
    bz      process_ch3_click_no_setup

    ;====================================
    ; Steering servo setup in progress:
    ; 1 click: next setup step
    ; more than 1 click: cancel setup
    decfsz  ch3_clicks, f                
    goto    process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_NEXT
    return    
    
process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_CANCEL
    return    

    ;====================================
    ; Normal operation; setup is not active
process_ch3_click_no_setup
    movlw   0x50                    ; send 'P'
    call    UART_send_w        

    decfsz  ch3_clicks, f                
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click: switch light mode up (Stand, Head, Fog, High Beam) 
    rlf     light_mode, f
    bsf     light_mode, LIGHT_MODE_STAND
    movlw   0x0f
    andwf   light_mode, f
    movlw   0x31                    ; send '1'
    call    UART_send_w        
    return

process_ch3_double_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_triple_click

    ; --------------------------
    ; Double click: switch light mode down (Stand, Head, Fog, High Beam)  
    rrf     light_mode, f
    movlw   0x0f
    andwf   light_mode, f
    movlw   0x32                    ; send '2'
    call    UART_send_w        
    return

process_ch3_triple_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_quad_click

    ; --------------------------
    ; Triple click: all lights on/off
    movlw   0x0f
    andwf   light_mode, w
    sublw   0x0f
    movlw   0x0f
    skpnz
    movlw   0x00     
    movwf   light_mode
    movlw   0x33                    ; send '3'
    call    UART_send_w        
    return

process_ch3_quad_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_8_click

    ; --------------------------
    ; Quad click: Hazard lights on/off  
    clrf    ch3_clicks
    call    Synchronize_blinking
    movlw   1 << BLINK_MODE_HAZARD
    xorwf   blink_mode, f
    movlw   0x34                    ; send '4'
    call    UART_send_w        
    return

process_ch3_8_click
    movlw   4
    subwf   ch3_clicks, w
    bnz     process_ch3_click_end

    movlw   1
    movwf   setup_mode    
    movlw   0x38                    ; send '8'
    call    UART_send_w        

process_ch3_click_end
    clrf    ch3_clicks
    return

    

;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; THROTTLE related functions
;
;******************************************************************************
;******************************************************************************
;******************************************************************************



;******************************************************************************
; Read_throttle
; 
; Read the throttle servo channel and write the result in throttle_h/throttle_l
;******************************************************************************
Read_throttle
    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H       ; Reset the timer to 0
    clrf    TMR1L
    clrf    throttle_h  ; Prime the result with "timing measurement failed"
    clrf    throttle_l

    ; Wait until servo signal is LOW 
    ; This ensures that we do not start in the middle of a pulse
th_wait_for_low1
    btfsc   PORT_THROTTLE
    goto    th_wait_for_low1

th_wait_for_high
    btfss   PORT_THROTTLE   ; Wait until servo signal is high
    goto    th_wait_for_high

    bsf     T1CON, 0    ; Start timer 1

th_wait_for_low2
    btfsc   PORT_THROTTLE   ; Wait until servo signal is LOW
    goto    th_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    call    Validate_servo_measurement
    movf    xh, w    
    movwf   throttle_h
    movf    xl, w
    movwf   throttle_l
    return


;******************************************************************************
; Process_throttle
;   If POS == CEN:          ; We found neutral
;       POS_NORMALIZED = 0
;   Else
;       If POS < CEN:   ; We need to calculate against EPL
;           POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPL)
;           if not REV:
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;       Else            ; We need to calculate against EPR
;           POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPR)
;           if REV:
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;
;******************************************************************************
Process_throttle
    movf    throttle_h, w
    movwf   xh
    movf    throttle_l, w
    movwf   xl

    ; Check for invalid throttle measurement (e.g. timeout) by testing whether
    ; throttle_h/l == 0. If yes treat it as "throttle centre"
    clrf    yh
    clrf    yl
    call    If_x_eq_y
    bnz     throttle_is_valid

    clrw
    goto    throttle_set

throttle_is_valid
    ; Throttle in centre? (note that we preloaded xh/xl just before this)
    ; If yes then set throttle output variable to '0'
    movf    throttle_centre_h, w
    movwf   yh
    movf    throttle_centre_l, w
    movwf   yl
    call    If_x_eq_y
    bnz     throttle_off_centre

    clrw
    goto    throttle_set

throttle_off_centre
    movf    throttle_h, w
    movwf   xh
    movf    throttle_l, w
    movwf   xl   
    call    If_y_lt_x
    bnc     throttle_right

throttle_left
    movf    throttle_epl_h, w
    movwf   zh
    movf    throttle_epl_l, w
    movwf   zl

    call    Min_x_z     ; Adjust endpoint if POS is less than EPL
    movf    zh, w
    movwf   throttle_epl_h
    movf    zl, w
    movwf   throttle_epl_l

    call    Calculate_normalized_servo_position
    movf    throttle_reverse, f
    skpnz   
    sublw   0
    goto    throttle_set

throttle_right
    movf    throttle_epr_h, w
    movwf   zh
    movf    throttle_epr_l, w
    movwf   zl

    call    Max_x_z     ; Adjust endpoint if POS is larger than EPR
    movf    zh, w
    movwf   throttle_epr_h
    movf    zl, w
    movwf   throttle_epr_l

    call    Calculate_normalized_servo_position
    movf    throttle_reverse, f
    skpz   
    sublw   0

throttle_set
    movwf   throttle

    ; Calculate abs(throttle) for easier math. We can use the highest bit 
    ; of throttle to get the sign later!
    movwf   throttle_abs
    btfsc   throttle_abs, 7
    decf    throttle_abs, f
    btfsc   throttle_abs, 7
    comf    throttle_abs, f
    return    


;******************************************************************************
; Process_drive_mode
;
; Simulates the state machine in the ESC and updates the variable drive_mode
; accordingly.
;
; Currently programmed for the HPI SC-15WP
;
; +/-10: forward = 0, reverse = 0
; >+10: forward = 1, brake_armed = 1
; <-10:
;   if brake_armed: brake = 1
;   if not brake_armed: reverse = 1, brake = 0
; 2 seconds in Neutral: brake_armed = 0
; Brake -> Neutral: brake = 0, brake_armed = 0
; Reverse -> Neutral: brake = 1 for 2 seconds

; Bitfields in variable drive_mode
;#define DRIVE_MODE_FORWARD 0 
;#define DRIVE_MODE_BRAKE 1 
;#define DRIVE_MODE_REVERSE 2
;#define DRIVE_MODE_BRAKE_ARMED 3
;******************************************************************************
Process_drive_mode
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_drive_mode_not_neutral

    btfsc   drive_mode, DRIVE_MODE_REVERSE_BRAKE
    return
    btfsc   drive_mode, DRIVE_MODE_BRAKE_DISARM
    return

    bcf     drive_mode, DRIVE_MODE_FORWARD
    btfss   drive_mode, DRIVE_MODE_REVERSE
    goto    process_drive_mode_not_neutral_after_reverse

    bcf     drive_mode, DRIVE_MODE_REVERSE
    bsf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bsf     drive_mode, DRIVE_MODE_BRAKE
    movlw   BRAKE_AFTER_REVERSE_COUNTER_VALUE
    movwf   drive_mode_counter   
    return

process_drive_mode_not_neutral_after_reverse
    bsf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    movlw   BRAKE_DISARM_COUNTER_VALUE
    movwf   drive_mode_brake_disarm_counter   

    btfsc   drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED
    bcf     drive_mode, DRIVE_MODE_BRAKE
    return

process_drive_mode_not_neutral
    bcf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM

    btfsc   throttle, 7
    goto    process_drive_mode_brake_or_reverse

    bsf     drive_mode, DRIVE_MODE_FORWARD
    bsf     drive_mode, DRIVE_MODE_BRAKE_ARMED
    bcf     drive_mode, DRIVE_MODE_REVERSE
    bcf     drive_mode, DRIVE_MODE_BRAKE
    return

process_drive_mode_brake_or_reverse
    btfsc   drive_mode, DRIVE_MODE_BRAKE_ARMED
    goto    process_drive_mode_brake

    bsf     drive_mode, DRIVE_MODE_REVERSE
    bcf     drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_FORWARD
    return
    
process_drive_mode_brake
    bsf     drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_FORWARD
    bcf     drive_mode, DRIVE_MODE_REVERSE
    return



;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; STEERING related functions
;
;******************************************************************************
;******************************************************************************
;******************************************************************************



;******************************************************************************
; Read_steering
; 
; Read the steering servo channel and write the result in steering_h/steering_l
;******************************************************************************
Read_steering
    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H       ; Reset the timer to 0
    clrf    TMR1L
    clrf    steering_h  ; Prime the result with "timing measurement failed"
    clrf    steering_l

    ; Wait until servo signal is LOW 
    ; This ensures that we do not start in the middle of a pulse
st_wait_for_low1
    btfsc   PORT_STEERING
    goto    st_wait_for_low1

st_wait_for_high
    btfss   PORT_STEERING   ; Wait until servo signal is high
    goto    st_wait_for_high

    bsf     T1CON, 0    ; Start timer 1

st_wait_for_low2
    btfsc   PORT_STEERING   ; Wait until servo signal is LOW
    goto    st_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    call    Validate_servo_measurement
    movf    xh, w    
    movwf   steering_h
    movf    xl, w
    movwf   steering_l
    return


;******************************************************************************
; Process_steering
;   If POS == CEN:          ; We found dead centre
;       POS_NORMALIZED = 0
;   Else
;       If POS < CEN:   ; We need to calculate against EPL
;           POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPL)
;           If not REV
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;       Else            ; We need to calculate against EPR
;           POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPR)
;           If REV
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;
;******************************************************************************
Process_steering
    movf    steering_h, w
    movwf   xh
    movf    steering_l, w
    movwf   xl

    ; Check for invalid throttle measurement (e.g. timeout) by testing whether
    ; throttle_h/l == 0. If yes treat it as "throttle centre"
    clrf    yh
    clrf    yl
    call    If_x_eq_y
    bnz     steering_is_valid

    clrw
    goto    steering_set

steering_is_valid
    ; Steering in centre? (note that we preloaded xh/xl just before this)
    ; If yes then set steering output variable to '0'
    movf    steering_centre_h, w
    movwf   yh
    movf    steering_centre_l, w
    movwf   yl
    call    If_x_eq_y
    bnz     steering_off_centre

    clrw
    goto    steering_set

steering_off_centre
    movf    steering_h, w
    movwf   xh
    movf    steering_l, w
    movwf   xl   
    call    If_y_lt_x
    bnc      steering_right

steering_left
    movf    steering_epl_h, w
    movwf   zh
    movf    steering_epl_l, w
    movwf   zl

    call    Min_x_z     ; Adjust endpoint if POS is smaller than EPR
    movf    zh, w
    movwf   steering_epl_h
    movf    zl, w
    movwf   steering_epl_l

    call    Calculate_normalized_servo_position
    movf    steering_reverse, f
    skpnz   
    sublw   0
    goto    steering_set

steering_right
    movf    steering_epr_h, w
    movwf   zh
    movf    steering_epr_l, w
    movwf   zl

    call    Max_x_z     ; Adjust endpoint if POS is larger than EPR
    movf    zh, w
    movwf   steering_epr_h
    movf    zl, w
    movwf   steering_epr_l

    call    Calculate_normalized_servo_position
    movf    steering_reverse, f
    skpz   
    sublw   0

steering_set
    movwf   steering

    ; Calculate abs(steering) for easier math. We can use the highest bit 
    ; of throttle to get the sign later!
    movwf   steering_abs
    btfsc   steering_abs, 7
    decf    steering_abs, f
    btfsc   steering_abs, 7
    comf    steering_abs, f
    return   


;******************************************************************************
; Process_indicators
; 
; Implements a sensible indicator algorithm.
;
; To turn on the indicators, throtte and steering must be centered for 2 s,
; then steering must be either left or right >50% for more than 2 s.
;
; Indicators are turned off when: 
;   - opposite steering is >30%
;   - steering neutral or opposite for >2s
;******************************************************************************
#define STATE_INDICATOR_NOT_NEUTRAL 0
#define STATE_INDICATOR_NEUTRAL_WAIT 1
#define STATE_INDICATOR_BLINK_ARMED 2
#define STATE_INDICATOR_BLINK_ARMED_LEFT 3
#define STATE_INDICATOR_BLINK_ARMED_RIGHT 4
#define STATE_INDICATOR_BLINK_LEFT 5
#define STATE_INDICATOR_BLINK_LEFT_WAIT 6
#define STATE_INDICATOR_BLINK_RIGHT 7
#define STATE_INDICATOR_BLINK_RIGHT_WAIT 8

Process_indicators
    movf    indicator_state, w
    addwf   PCL,F

Process_indicators_table
    goto    process_indicators_not_neutral
    goto    process_indicators_neutral_wait
    goto    process_indicators_blink_armed
    goto    process_indicators_blink_armed_left
    goto    process_indicators_blink_armed_right
    goto    process_indicators_blink_left
    goto    process_indicators_blink_left_wait
    goto    process_indicators_blink_right
    goto    process_indicators_blink_right_wait
    IF ((HIGH ($)) != (HIGH (Process_indicators_table)))
        ERROR "Process_indicators_table CROSSES PAGE BOUNDARY!"
    ENDIF

    IF 0
    movf    indicator_state, w
    movwf   temp
    skpnz   
    goto    process_indicators_not_neutral
    decf    temp, f
    skpnz   
    goto    process_indicators_neutral_wait
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed_left
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed_right
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_left
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_left_wait
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_right
    goto    process_indicators_blink_right_wait
    ENDIF

process_indicators_not_neutral
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    skpnc
    return

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter
    movlw   STATE_INDICATOR_NEUTRAL_WAIT
    movwf   indicator_state
    return

process_indicators_neutral_wait
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_armed
    movlw   STATE_INDICATOR_BLINK_ARMED
    movwf   indicator_state
    return

process_indicators_set_not_neutral
    movlw   STATE_INDICATOR_NOT_NEUTRAL
    movwf   indicator_state
    bcf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    bcf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    skpc
    return    

    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter    
    movlw   STATE_INDICATOR_BLINK_ARMED_LEFT
    btfss   steering, 7 
    movlw   STATE_INDICATOR_BLINK_ARMED_RIGHT
    movwf   indicator_state
    return
  
process_indicators_blink_armed_left  
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfss   steering, 7 
    goto    process_indicators_set_blink_armed

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_left  
    movlw   STATE_INDICATOR_BLINK_LEFT
    movwf   indicator_state
    call    Synchronize_blinking
    bsf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed_right  
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfsc   steering, 7 
    goto    process_indicators_set_blink_armed

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_right
    movlw   STATE_INDICATOR_BLINK_RIGHT
    movwf   indicator_state
    call    Synchronize_blinking
    bsf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    return

process_indicators_blink_left
    btfsc   steering, 7 
    goto    process_indicators_blink_left_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_LEFT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_left_wait
    btfsc   steering, 7 
    goto    process_indicators_blink_left_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_wait_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_left

    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral

process_indicators_blink_right
    btfss   steering, 7 
    goto    process_indicators_blink_right_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_not_neutral

process_indicators_blink_right_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_RIGHT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_right_wait
    btfss   steering, 7 
    goto    process_indicators_blink_right_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_right_wait_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_right

    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral



;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; STEERING SERVO related functions
;
;******************************************************************************
;******************************************************************************
;******************************************************************************


;******************************************************************************
; Process_steering_servo
;
; This function calculates:
;
;       right - centre * 100
;       -------------------- + centre
;           abs(steering)
;
; To ease calculation we first do right - centre, then calculate its absolute
; value but store the sign. After multiplication and division using the
; absolute value we re-apply the sign, then add centre.
;******************************************************************************
#define SIGN_FLAG wl

Process_steering_servo
    movf    setup_mode, f
    bz      process_steering_servo_no_setup

    btfsc   setup_mode, SETUP_MODE_CANCEL
    goto    process_steering_servo_setup_cancel
    btfsc   setup_mode, 3
    goto    process_steering_servo_setup_right
    btfsc   setup_mode, 2
    goto    process_steering_servo_setup_left
    btfsc   setup_mode, 1
    goto    process_steering_servo_setup_centre

process_steering_servo_setup_init
    movlw   -120
    movwf   servo_epl
    clrf    servo_centre
    movlw   120
    movwf   servo_epr
    bsf     setup_mode, 1
    goto    process_steering_servo_no_setup

process_steering_servo_setup_centre
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    bcf     setup_mode, SETUP_MODE_NEXT
    call    process_steering_servo_no_setup
    movf    servo, w
    movwf   servo_setup_centre         
    bsf     setup_mode, 2
    return

process_steering_servo_setup_left
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    bcf     setup_mode, SETUP_MODE_NEXT
    call    process_steering_servo_no_setup
    movf    servo, w
    movwf   servo_setup_epl         
    bsf     setup_mode, 3
    return

process_steering_servo_setup_right
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    movf    servo, w
    movwf   servo_epr         
    movf    servo_setup_epl, w         
    movwf   servo_epl         
    movf    servo_setup_epr, w         
    movwf   servo_epr
    call    Servo_store_values
    clrf    setup_mode
    return

process_steering_servo_setup_cancel
    clrf    setup_mode
    call    Servo_load_values
    return

process_steering_servo_no_setup
    movf    servo_epr, w
    btfss   steering, 7
    movf    servo_epl, w
    movwf   temp

    movf    servo_centre, w
    subwf   temp, f

    clrf    SIGN_FLAG
    btfsc   temp, 7
    incf    SIGN_FLAG, f
        
    btfsc   temp, 7
    decf    temp, f
    btfsc   temp, 7
    comf    temp, f

    ; temp contains now     abs(right - centre)
    movf    temp, w
    movwf   xl
    clrf    xh
    call    Mul_x_by_100
    movf    steering_abs, w
    movwf   yl
    clrf    yh
    call    Div_x_by_y

    movf    SIGN_FLAG, f
    bz      process_servo_not_negative

    ; Re-apply the sign bit
    movf    xl, w
    clrf    xl
    subwf   xl, f   

process_servo_not_negative
    movf    servo_centre, w
    addwf   xl, w
    movwf   servo
    return


;******************************************************************************
; Servo_load_values
; 
;******************************************************************************
Servo_load_values
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   114                 ; 'r'   
    call    UART_send_w
    movlw   100                 ; 'd'   
    call    UART_send_w
    movlw   0x20                ; Space   

    ; First check if the magic variables are intact. If not, assume the 
    ; EEPROM has not been initialized yet or is corrupted, so write default
    ; values back.
    movlw   EEPROM_ADR_MAGIC1
    call    EEPROM_read_byte
    sublw   EEPROM_MAGIC1
    bnz     Servo_load_defaults

    movlw   EEPROM_ADR_MAGIC2
    call    EEPROM_read_byte
    sublw   EEPROM_MAGIC2
    bnz     Servo_load_defaults

    movlw   EEPROM_ADR_SERVO_EPL
    call    EEPROM_read_byte
    movwf   servo_epl

    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_read_byte
    movwf   servo_centre

    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_read_byte
    movwf   servo_epr

    call    UART_send_w
    movf    servo_epl, w
    call    UART_send_signed_char
    movf    servo_centre, w
    call    UART_send_signed_char
    movf    servo_epr, w
    call    UART_send_signed_char
    movlw   0x0d                ; CR   
    call    UART_send_w

    return


;******************************************************************************
; Servo_store_values
; 
;******************************************************************************
Servo_store_values
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   115                 ; 's'   
    call    UART_send_w
    movlw   116                 ; 't'   
    call    UART_send_w
    movlw   111                 ; 'o'   
    call    UART_send_w
    movlw   0x20                ; Space   
    call    UART_send_w
    movf    servo_epl, w
    call    UART_send_signed_char
    movf    servo_centre, w
    call    UART_send_signed_char
    movf    servo_epr, w
    call    UART_send_signed_char
    movlw   0x0d                ; CR   
    call    UART_send_w

    movf    servo_epl, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPL
    call    EEPROM_write_byte

    movf    servo_centre, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_write_byte

    movf    servo_epr, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_write_byte
    return


;******************************************************************************
; Servo_load_defaults
;
; Load default values of -100..0..100 for the steering servo, write them 
; back to the EEPROM and write the 2 magic variables. 
;******************************************************************************
Servo_load_defaults
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   100                 ; 'd'   
    call    UART_send_w
    movlw   101                 ; 'e'   
    call    UART_send_w
    movlw   102                 ; 'f'   
    call    UART_send_w
    movlw   0x0d                ; CR   
    call    UART_send_w

    movlw   -100
    movwf   servo_epl
    clrf    servo_centre
    movlw   100
    movwf   servo_epr

    call    Servo_store_values

    movlw   EEPROM_MAGIC1
    movwf   temp
    movlw   EEPROM_ADR_MAGIC1
    call    EEPROM_write_byte

    movlw   EEPROM_MAGIC2
    movwf   temp
    movlw   EEPROM_ADR_MAGIC2
    call    EEPROM_write_byte
    return

    
;******************************************************************************
;******************************************************************************
;******************************************************************************
;
; UTILITY FUNCTIONS
;
;******************************************************************************
;******************************************************************************
;******************************************************************************

;******************************************************************************
; Validate_servo_measurement
;
; TMR1H/TMR1L: measured servo pulse width in us
;
; This function ensures that the measured servo pulse is in the range of
; 600 ... 2500 us. If not, "0" is returned to indicate failure.
; If the servo pulse is less than 800 us it is clamped to 800 us.
; If the servo pulse is more than 2300 us it is clamped to 2300 us.
;
; The resulting servo pulse width (clamped; or 0 if out of range) is returned
; in xh/xl
;******************************************************************************
Validate_servo_measurement
    movf    TMR1H, w
    movwf   xh
    movf    TMR1L, w
    movwf   xl

    movlw   HIGH(600)
    movwf   yh
    movlw   LOW(600)
    movwf   yl
    call    If_y_lt_x
    bnc     Validate_servo_above_min
    
Validate_servo_out_of_range
    clrf    xh
    clrf    xl
    return

Validate_servo_above_min
    movlw   HIGH(2500)
    movwf   yh
    movlw   LOW(2500)
    movwf   yl    
    call    If_y_lt_x
    bnc     Validate_servo_out_of_range

    movlw   HIGH(800)
    movwf   yh
    movlw   LOW(800)
    movwf   yl    
    call    If_y_lt_x
    bnc     Validate_servo_above_clamp_min

Validate_servo_clamp
    movf    yh, w
    movwf   xh
    movf    yl, w
    movwf   xl
    return

Validate_servo_above_clamp_min
    movlw   HIGH(2300)
    movwf   yh
    movlw   LOW(2300)
    movwf   yl    
    call    If_y_lt_x
    bnc     Validate_servo_clamp
    return


;******************************************************************************
; Calculate_normalized_servo_position
;
; xh/xl: POS servo measured pulse width
; yh/yl: CEN centre pulse width
; zh/zl: EP  end point pulse width
;
;       If EP < CEN:
;           If POS < EP     ; Clamp invald input
;               return 100
;           return (CEN - POS) * 100 / (CEN - EP)
;       Else:               ; EP >= CEN
;           If EP < POS     ; Clamp invald input
;               return ((POS - CEN) * 100 / (EP - CEN))
;           return 100
;
; Result in W: 0..100
;******************************************************************************
Calculate_normalized_servo_position
    ; x = POS, y = CEN, z = EP

    swap_x_y    xh, yh
    swap_x_y    xl, yl
    swap_x_y    yh, zh
    swap_x_y    yl, zl

    ; x = CEN, y = EP, z = POS

    call    If_y_lt_x
    bc      calculate_ep_gt_cen
        
    movfw   zl
    subwf   yl, w
    movfw   zh
    skpc                
    incfsz  zh, w       
    subwf   yh, w
    skpnc   
    retlw   100

calculate_normalized_left
    ; (CEN - POS) * 100 / (CEN - EP)
    ; Worst case we are dealing with CEN = 2300 and POS = 800 (we clamp 
    ; measured values into that range!)
    ; To keep within 16 bits we have to scale down:
    ;
    ;   ((CEN - POS) / 4) * 100 / ((CEN - EP) / 4)
    

    movf    xh, w           ; Save CEN in wh/wl as xh/xl gets result of 
    movwf   wh              ;  sub_x_from_y
    movf    xl, w
    movwf   wl

    swap_x_y    yh, zh
    swap_x_y    yl, zl

    ; w = CEN, x = CEN, y = POS, z = EP

    call    Sub_y_from_x    ; xh/hl =  CEN - POS
    call    Div_x_by_4      ; xh/hl =  (CEN - POS) / 4
    call    Mul_x_by_100    ; xh/hl =  ((CEN - POS) / 4) * 100

    swap_x_y    wh, xh
    swap_x_y    wl, xl
    swap_x_y    yh, zh
    swap_x_y    yl, zl

    ; w = ((CEN - POS) / 4) * 100, x = CEN, y = EP, z = POS

    call    Sub_y_from_x    ; xh/hl =  CEN - EP
    call    Div_x_by_4      ; xh/hl =  (CEN - EP) / 4

    swap_x_y    xh, yh
    swap_x_y    xl, yl
    swap_x_y    wh, xh
    swap_x_y    wl, xl

    ; x = ((CEN - POS) / 4) * 100, y = ((CEN - EP) / 4)

    call    Div_x_by_y
    movf    xl, w
    return    

calculate_ep_gt_cen
    movfw   zl
    subwf   yl, w
    movfw   zh
    skpc                
    incfsz  zh, w       
    subwf   yh, w
    skpc    
    retlw   100

calculate_normalized_right
    ; ((POS - CEN) * 100 / (EP - CEN))
    ; Worst case we are dealing with CEN = 800 and POS = 2300 (we clamp 
    ; measured values into that range!)
    ; To keep within 16 bits we have to scale down:
    ;
    ;   ((POS - CEN) / 4) * 100 / ((EP - CEN) / 4)
    
    ; x = CEN, y = EP, z = POS

    swap_x_y    yh, zh
    swap_x_y    yl, zl
    swap_x_y    xh, yh
    swap_x_y    xl, yl

    ; x = POS, y = CEN, z = EP

    call    Sub_y_from_x    ; xh/hl =  POS - CEN
    call    Div_x_by_4      ; xh/hl =  (POS - CEN) / 4
    call    Mul_x_by_100    ; xh/hl =  ((POS - CEN) / 4) * 100

    swap_x_y    xh, wh
    swap_x_y    xl, wl
    swap_x_y    xh, zh
    swap_x_y    xl, zl

    ; w = ((POS - CEN) / 4) * 100, x = EP, y = CEN

    call    Sub_y_from_x    ; xh/hl =  EP - CEN
    call    Div_x_by_4      ; xh/hl =  (EP - CEN) / 4

    swap_x_y    xh, yh
    swap_x_y    xl, yl
    swap_x_y    wh, xh
    swap_x_y    wl, xl

    ; x = ((POS - CE) / 4) * 100, y = ((EP - CEN) / 4)

    call    Div_x_by_y
    movf    xl, w
    return  


;******************************************************************************
; Max
;  
; Given two 8-bit values in temp and w, returns the larger one in both temp
; and w
;******************************************************************************
Max
    subwf   temp, w
    skpc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min
;  
; Given two 8-bit values in temp and w, returns the smaller one in both temp
; and w
;******************************************************************************
Min
    subwf   temp, w
    skpnc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the smaller one in zl/zh.
;******************************************************************************
Min_x_z
    movf    xl, w
    subwf	zl, w	
    movf	xh, w
    skpc
    addlw   1
    subwf	zh, w
    andlw	b'10000000'	
    skpz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Max_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the larger one in zl/zh.
;******************************************************************************
Max_x_z
    movf    xl, w
    subwf   zl, w		
    movf    xh, w
    skpc
    addlw   1
    subwf   zh, w		
    andlw   b'10000000'  
    skpnz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Div_x_by_y
;
; xh/xl = xh/xl / yh/yl; Remainder in zh/zl
;
; Based on "32 by 16 Divison" by Nikolai Golovchenko
; http://www.piclist.com/techref/microchip/math/div/div16or32by16to16.htm
;******************************************************************************
#define counter d0
Div_x_by_y
    clrf    zl      ; Clear remainder
    clrf    zh
    clrf    temp    ; Clear remainder extension
    movlw   16
    movwf   counter
    setc            ; First iteration will be subtraction

div16by16loop
    ; Shift in next result bit and shift out next dividend bit to remainder
    rlf     xl, f   ; Shift LSB
    rlf     xh, f   ; Shift MSB
    rlf     zl, f
    rlf     zh, f
    rlf     temp, f

    movf    yl, w
    btfss   xl, 0
    goto    div16by16add

    ; Subtract divisor from remainder
    subwf   zl, f
    movf    yh, w
    skpc
    incfsz  yh, w
    subwf   zh, f
    movlw   1
    skpc
    subwf   temp, f
    goto    div16by16next

div16by16add
    ; Add divisor to remainder
    addwf   zl, f
    movf    yh, w
    skpnc
    incfsz  yh, w
    addwf   zh, f
    movlw   1
    skpnc
    addwf   temp, f

div16by16next
    ; Carry is next result bit
    decfsz  counter, f
    goto    div16by16loop

; Shift in last bit
    rlf     xl, f
    rlf     xh, f
    return
#undefine counter


;******************************************************************************
; Mul_x_by_100
;
; Calculates xh/xl = xh/xl * 100
; Only valid for xh/xl <= 655 as the output is only 16 bits
;******************************************************************************
Mul_x_by_100
    ; Shift accumulator left 2 times: xh/xl = xh/xl * 4
	clrc
	rlf	    xl, f
	rlf	    xh, f
	rlf	    xl, f
	rlf	    xh, f

    ; Copy accumulator to temporary location
	movf	xh, w
	movwf	d1
	movf	xl, w
	movwf	d0

    ; Shift temporary value left 3 times: d1/d0 = xh/xl * 4 * 8   = xh/xl * 32
	clrc
	rlf	    d0, f
	rlf	    d1, f
	rlf	    d0, f
	rlf	    d1, f
	rlf	    d0, f
	rlf	    d1, f

    ; xh/xl = xh/xl * 32  +  xh/xl * 4   = xh/xl * 36
	movf	d0, w
	addwf	xl, f
	movf	d1, w
	skpnc
	incfsz	d1, w
	addwf	xh, f

    ; Shift temporary value left by 1: d1/d0 = xh/xl * 32 * 2   = xh/xl * 64
	clrc
	rlf	    d0, f
	rlf	    d1, f

    ; xh/xl = xh/xl * 36  +  xh/xl * 64   = xh/xl * 100 
	movf	d0, w
	addwf	xl, f
	movf	d1, w
	skpnc
	incfsz	d1, w
	addwf	xh, f
    return


;******************************************************************************
; Div_x_by_4
;
; Calculates xh/xl = xh/xl / 4
;******************************************************************************
Div_x_by_4
	clrc
	rrf     xh, f
	rrf	    xl, f
	clrc
	rrf     xh, f
	rrf	    xl, f
    return


;******************************************************************************
; Sub_y_from_x
;
; This function calculates xh/xl = xh/xl - yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
Sub_y_from_x
    movf    yl, w
    subwf   xl, f
    movf    yh, w
    skpc
    incfsz  yh, W
    subwf   xh, f
    return         


;******************************************************************************
; If_y_lt_x
;
; This function compares the 16 bit unsigned values in yh/yl with xh/xl.
; If y < x then C flag is cleared on exit
; If y >= x then C flag is set on exit
;
; x and y stay unchanged.
;******************************************************************************
If_y_lt_x
    movfw   xl
    subwf   yl, w
    movfw   xh
    skpc                
    incfsz  xh, w       
    subwf   yh, w
    return


;******************************************************************************
; If_x_eq_y
;
; This function compares the 16 bit unsigned values in yh/yl with xh/xl.
; If x == y then Z flag is set on exit
; If y != x then Z flag is cleared on exit
;
; x and y stay unchanged.
;******************************************************************************
If_x_eq_y
    movfw   xl
    subwf   yl, w
    skpz
    return
    movfw   xh
    subwf   yh, w
    return


;******************************************************************************
; TLC5916_send
;
; Sends the value in the temp register to the TLC5916 LED driver.
;******************************************************************************
TLC5916_send
    movlw   8
    movwf   d0

tlc5916_send_loop
    rlf     temp, f
    skpc    
    bcf     PORT_SDI
    skpnc    
    bsf     PORT_SDI
    bsf     PORT_CLK
    nop
    bcf     PORT_CLK
    decfsz  d0, f
    goto    tlc5916_send_loop

    bsf     PORT_LE
    nop
    bcf     PORT_LE
    bcf     PORT_OE
    return


;******************************************************************************
; EEPROM_write_byte
;
; Writes the value stored in 'temp' into the address given in W
;******************************************************************************
EEPROM_write_byte
	BANKSEL EEADR
	movwf   EEADR
	BANKSEL temp
	movf    temp, w
	BANKSEL EEDATA
	movwf   EEDATA		    ; Setup byte to write
	bsf	    EECON1, WREN    ; Enable writes
	
	movlw   H'55'           ; Required sequence!
	movwf   EECON2
    movlw   H'AA'
	movwf   EECON2
	bsf     EECON1, WR      ; Begin write procedure
	bcf     EECON1, WREN	; Disable writes 
                            ;  Note: does not affect current write cycle
	
	; Wait for the write to complete before we return
	BANKSEL PIR1
    btfss   PIR1, EEIF
	goto    $-1		 
	bcf     PIR1, EEIF      ; Clear EEPROM Write Operation IRQ flag
    return


;******************************************************************************
; EEPROM_read_byte
;
; Reads the value stored at address W. The read value is returned in W.
;******************************************************************************
EEPROM_read_byte
	BANKSEL EEADR
	movwf   EEADR
	bsf     EECON1, RD      
	movf    EEDATA, w
	BANKSEL PIR1
	return


;**********************************************************************
Debug_output_values

#define setup_mode_old debug_indicator_state_old
#define servo_old debug_throttle_old

debug_output_setup
    movf    setup_mode, w
    subwf   setup_mode_old, w
    bnz     debug_output_servo
    movf    servo, w
    subwf   servo_old, w
    bz      debug_output_indicator

debug_output_servo
    movlw   73                  ; 'S'   
    call    UART_send_w
    movlw   101                 ; 'e'   
    call    UART_send_w
    movlw   114                 ; 't'   
    call    UART_send_w
    movlw   117                 ; 'u'   
    call    UART_send_w
    movlw   112                 ; 'p'   
    call    UART_send_w
    movf    setup_mode, w
    movwf   setup_mode_old
    call    UART_send_signed_char
    movlw   0x20                ; Space
    call    UART_send_w
    movlw   73                  ; 'S'   
    call    UART_send_w
    movlw   101                 ; 'e'   
    call    UART_send_w
    movlw   114                 ; 'r'   
    call    UART_send_w
    movlw   118                 ; 'v'   
    call    UART_send_w
    movlw   111                 ; 'o'   
    call    UART_send_w
    movf    servo, w
    movwf   servo_old
    call    UART_send_signed_char
    movlw   h'0d'               ; CR
    call    UART_send_w
    

debug_output_indicator
    IF 0
    movf    indicator_state, w
    subwf   debug_indicator_state_old, w
    bz      debug_output_steering

    movlw   73                  ; 'I'   
    call    UART_send_w
    movf    indicator_state, w
    movwf   debug_indicator_state_old
    call    UART_send_signed_char
    movlw   h'0d'               ; CR
    call    UART_send_w
    ENDIF

debug_output_steering
    IF 0
    movf    steering, w
    subwf   debug_steering_old, w
    bz      debug_output_throttle

    movlw   83                  ; 'S'   
    call    UART_send_w
    movlw   84                  ; 'T'   
    call    UART_send_w
    movf    steering, w
    movwf   debug_steering_old
    call    UART_send_signed_char
    movlw   h'0d'               ; CR
    call    UART_send_w
    ENDIF

debug_output_throttle
    IF 0
    movf    throttle, w
    subwf   debug_throttle_old, w
    bz      debug_output_end

    movlw   84                  ; 'T'   
    call    UART_send_w
    movlw   72                  ; 'H'   
    call    UART_send_w
    movf    throttle, w
    movwf   debug_throttle_old
    call    UART_send_signed_char
    movlw   h'0d'               ; CR
    call    UART_send_w

    movf    drive_mode, w
    call    UART_send_signed_char
    movlw   h'0d'               ; CR
    call    UART_send_w
    ENDIF

debug_output_end
    return



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


;******************************************************************************
; Send W, which is treated as signed char, as human readable number via the
; UART.
;******************************************************************************
UART_send_signed_char
    clrf    send_hi
    movwf   send_lo
    btfss   send_lo, 7  ; Highest bit indicates negative values
    goto    UART_send_signed_char_pos

    movlw   45          ; Send leading minus
    call    UART_send_w

    decf    send_lo, f  ; Absolute value of the number to send
    comf    send_lo, f

UART_send_signed_char_pos
    goto    UART_send_16bit


;******************************************************************************
; Send W out via the UART
;******************************************************************************
UART_send_w
    btfss   PIR1, TXIF
    goto    UART_send_w ; Wait for transmitter interrupt flag

    movwf   TXREG	    ; Send data stored in W
    return      


;******************************************************************************
; Send a 16 bit value stored in send_hi and send_lo as a 5 digit decimal 
; number over the UART
;******************************************************************************
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


;******************************************************************************
; Send 'Hello world\n' via the UART
;******************************************************************************
Send_Hello_world
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
    return


    END     ; Directive 'end of program'


;******************************************************************************
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
;       - Off
;       - Stand light
;       - + Head light
;       - + Fog lights
;       - + High beam
;   - Drive mode:
;       - Neutral
;       - Forward
;       - Braking - Brake lights
;       - Reverse - Reverse light
;   - Indicators
;   - Steering servo pass-through
;   - Hazard lights
;   - "Lichthupe"?
;
;
;   Other functions:
;   - Failsafe mode: all lights blink when no signal (DOES NOT WORK FOR HK-310)
;   - Program CH3 (NOT NEEDED)
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
;       - triple press: all lights off
;       - quadruple press: hazard lights
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
;   3 Bytes: SYNC, Lights, Lights-half, ST
;       SYNC:       Always 0x80..0x87, which does not appear in the other values
;                   If a slave receives 0x87 the data is processed.
;                   If the value is 0x86..0x80 then it increments the value 
;                   by 1 and sends all 3 received bytes at its output. 
;                   This provides us with a simple way of daisy-chaining 
;                   several slave modules!
;                   NOTE: this means that the steering servo can only be
;                   connected to the last slave module!
;       Lights:     Each bit indicates a different light channel (0..6)
;       Lights-half:Each bit indicates a different light channel (0..6) for
;                   outputs that shall be on in half brightness. Note that if
;                   the same bit is set in "Lights" then the output will be
;                   fully on (i.e. full brightness takes precedence over half
;                   brightness).
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
;       CH3: 1013 = AUX, 2120 = OFF; 
;
;   TODO: find out normals for left/right fwd/bwd
;
;
;   Servo processing:
;   =================
;   Given the TX/RX findings above, we will design the light controller
;   to expect a servo range of 800 .. 1500 .. 2300 us (1500 +/-700 us).
;
;       Everything below 600 will be considered invalid.
;       Everything between 600 and 800 will be clamped to 800.
;       Everything between 2300 and 2500 will be clamped to 2300
;       Everything above 2500 will be considered invalid.
;       Defaults are 1000 .. 1500 .. 2000 us
;   
;   Hex values for those numbers:
;       600     0x258    
;       800     0x320
;       1000    0x3E8 
;       1500    0x5dc    
;       2000    0x7d0
;       2300    0x8FC
;       2500    0x9c4
;   
;
;   Timeout for measuring high pulse: Use TMR1H bit 4: If set, more than 
;   4096 ms have expired!
;   Timeout for waiting for pulse to go high: Use TMR1H bit 7: If set, more 
;   than 32768 ms have expired! 
;   These tests allow us to use cheap bit test instructions.
;
;   ###########################################################################
;   NOTE: SINCE WE ARE TARGETING THE HK-310 ONLY WHICH ALWAYS SENDS A SERVO
;         PULSE WE DO NOT IMPLEMENT TIMEOUTS FOR NOW!
;   ###########################################################################
;  
;   End points and Centre can be configured (default to the above values).
;   Assuming CH3 is a switch, only the endpoints can be configured.
;
;   CH3 processing:
;   Implement a Schmitt-Trigger around the center between the endpoints.
;   Example:
;       Switch pos 0: 1000 us
;       Switch pos 1: 2000 us
;       Center is therefore   (2000 + 1000) / 2 = 1500 us
;       Hysteresis:           (2000 - 1000) / 8 = 125 us
;       If last switch position was pos 0:
;           measured timing must be larger than 1500 + 125 = 1625 us to accept 
;           as pos 1
;       If last switch position was pos 1:
;           measured timing must be larger than 1500 - 125 = 1375 us to accept
;           as pos 0
;   For accuracy is not important as we are dealing with a switch we can
;   only use bits 11..4 (16 us resolution), so we can deal with byte 
;   calculations.
;   Note: calculation must ensure that due to servo reversing pos 0 may
;   have a larger or smaller time value than pos 1.
;
;   Steering and Throttle processing:
;   We have:
;       EPL (end point left)
;       EPR (end point right)
;       REV (flag that indicates servo reversing)
;       CEN (neutral position)
;           Margin for neutral position: +/- 5%
;           Some speed controlled can configure this from 4..8%
;       POS (measured servo pulse length)
;   To make processing easier we ensure that
;       EPL > CEN > EPR must be true
;   This can be achieved with the REV flag that indicates when a channel is
;   reversed.
;   By default we assume that 
;       Throttle forward is EPR, backward EPL
;       Steering left is EPL, Steering right is EPR
;
;   We need to convert POS into a range of 
;       -100 .. 0 .. +100   (left .. centre .. right)
;
;   If POS == CEN:          ; We found dead centre
;       POS_NORMALIZED = 0
;   Else
;       If POS > CEN:   ; We are dealing with a right turn
;           POS_NORMALIZED = calculate(POS, EPR, CEN)
;           If REV
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;       Else            ; We are dealing with a left turn
;           POS_NORMALIZED = calculate(POS, EPL, CEN)
;           If not REV
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;
;   caluclate       ; inputs: POS, EP(L or R), CEN
;       If EP > CEN:
;           If POS > EP     ; Clamp invald input
;               return 100
;           POS_NORMALIZED = ((POS - CEN) * 100 / (EP - CEN))
;       Else:               ; EP < CEN
;           If POS < EP     ; Clamp invald input
;               return 100
;           POS_NORMALIZED = (CEN - POS) * 100 / (CEN - EP)
;       
;
;   Timer and PWM:
;   ==============
;   We need a way to measure time, e.g. for double click detection of ch3 and
;   to derive the blink frequency. We will use TIMER0 for generating a low,
;   steady frequency. TIMER0 will be set in such a way that within a worst-case
;   mainloop it can only overflow once. This means we will be able to 
;   accurately measure longer periods of time.
;
;   To do so we select a pre-scaler of 1:256. This gives us a timer clock of
;   256 us. This means that the timer overflows every 65.536 ms.
;   We will use T0IF to detect overflow.
;   The blink frequency of 1.5 Hz can be easily derived: a single period is
;   5 timer overflows (333 ms / 65.536 ms).
;   For ease of implementation we can have several 8-bit variables that are
;   incremented every 64.536 ms. E.g. we can have one for blinking, that is
;   reset after it reaches "5", which toggles the blink flag.
;   We can have another one that we reset when we receive a CH3 pulse and
;   want to determine multiple clicks.
;
;   For combined tail/brake lights we have to control PWM on one output.
;   We could assume that the servo pulses are periodic every 20 ms and use
;   that to time our PWM -- at least a simple 50% PWM. That may flicker though.
;   The PIC has a 10 bit PWM on output RB3. TIMER2 is used for the frequency.
;    
;
;   Port usage:
;   ===========
;   Ports special assignment:
;   RA5:    Servo input (Vpp double-usage)
;   RB7:    Servo input (PGD double-usage)
;   RB6:    Servo input (PGC double-usage)
;   RB1:    Connected to RB6 (RX double-usage for slave!
;   RB2:    Slave out (TX Master) / Servo out (Slave)
;   RB3:    LED output with PWM (CCP1) [Out6/Out7]
;   
;   Ports with no special assignment:
;   RA0 [Out0?]
;   RA1 [Out1?]
;   RA2 [Out2?]
;   RA3 [Out3?]
;   RA4 [Out4?]
;   RA6 [Out5?]
;   RA7
;   RB0
;   RB4
;   RB5 [Button?]
;
;
;   Outputs:
;   ========
;   Master and slave have identical light outputs:
;   5 hardware outputs = 5 bits: Out0..Out5. 
;   1 hardware output with brightness control = 2 bits: Out6=dim, Out7=bright
;     If both Out6 and Out7 are set then the output is "bright"
;
;   This is 7 bits, so we can use it in a table to program easily:
;
;                       Out7 Out6 Out5 Out4 Out3 Out2 Out1 Out0    
;    Stand light
;    Head light
;    Fog lights
;    High beam
;    Brake
;    Reverse
;    Indicator left
;    Indicator right
;    Hazard lights   
;
;    This table allows us to assign multiple functions to a single output,
;    and to assign more than one output to a single function.
;
;
;   Auto configuration:
;   ===================
;   In order to avoid having to setup the light controller manually every time
;   something is changed on the vehicle RC system, automatic configuration
;   shall be employed.
;
;   The assumption is that both throttle and steering are in Neutral on power
;   on. So at startup we measure the servo signals and treat them as neutral.
;   To ensure sensible values we only accept 1500 us +/-25% (Note: translate
;   this to a senisble hex value for easy comparison!)
;
;   To adjust direction and reverse settings, we require the user to first
;   do full throttle forward, then full throttle back. And steering left, then
;   steering right. 
;   Worst case the user has to switch the vehicle on/off again to re-initialize.
;
;   The end-points are set to 30% initially and "grow" when servo signals are
;   received that are larger than the current value (up to the maximum of 800
;   and 2300 us respectively where we clamp servo signals).
;
;
;   Steering wheel servo:
;   =====================
;   To allow easy reconfiguration of the steering wheel servo the user has to
;   press the CH3 button 8 times. The steering channel will then directly drive
;   the steering wheel servo output, allowing the user to set the center 
;   position. Toggling CH3 once switches to "left end point". The user can
;   use the steering channel to set the left end point of the steering wheel
;   servo. Note that this may mean that the user has to turn the steering
;   channel to the right in case of servo reversing is needed! The user
;   confirms with toggling CH3 again, switching to "left end point" mode.
;   Once this is set, toggling CH3 stores all values persistently and switches
;   back to normal operation.
;
;   The slave controller accepts inputs of -120..0..+120, which it translates
;   to servo pulses 780..1500..2220 us. (This scales easily by multiplying
;   the servo value by 6, offsetting by 1500!)
;   The scaling will be done in the master.
;   The master stores 3 values for the steering wheel servo: left, centre and
;   right. Each of those may contain a value in the range of -120 to +120.
;   If the "steering" variable is 0 then it sends the centre value.
;   If the "steering" variable has a positive value it sends the interpolated
;   value:
;
;       right - centre * 100
;       -------------------- + centre
;           abs(steering)
;
;   Note this needs to be a signed operation! If "steering" is negative then
;   'right' is replaced with 'left' in the formula above.
;
;******************************************************************************

