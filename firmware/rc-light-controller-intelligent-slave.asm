;******************************************************************************
;
;   rc-light-controller-intelligent-slave.asm
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

; Enable debug functions like human readable UART output to read incoming
; servo values.
;#define DEBUG


    #include    <p16f628a.inc>
    #include    io_master.tmp

    __CONFIG _CP_OFF & _DATA_CP_OFF & _LVP_OFF & _BOREN_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT


    EXTERN local_light_table
    EXTERN local_light_half_table
    EXTERN slave_light_table
    EXTERN slave_light_half_table
    EXTERN local_setup_light_table
    EXTERN slave_setup_light_table

#define SLAVE_MAGIC_BYTE    0x87


#define CH3_BUTTON_TIMEOUT 6    ; Time in which we accept double-click of CH3
#define BLINK_COUNTER_VALUE 5   ; 5 * 65.536 ms = ~333 ms = ~1.5 Hz
#define BRAKE_AFTER_REVERSE_COUNTER_VALUE 15 ; 15 * 65.536 ms = ~1 s
#define BRAKE_DISARM_COUNTER_VALUE 15        ; 15 * 65.536 ms = ~1 s
#define INDICATOR_STATE_COUNTER_VALUE 8      ; 8 * 65.536 ms = ~0.5 s
#define INDICATOR_STATE_COUNTER_VALUE_OFF 30 ; ~2 s

; Bitfields in variable ch3_flags
#define CH3_FLAG_LAST_STATE 0
#define CH3_FLAG_TOGGLED 1
#define CH3_FLAG_INITIALIZED 7

; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_PARKING 0        ; Parking lights
#define LIGHT_MODE_LOW_BEAM 1       ; Low beam
#define LIGHT_MODE_FOG 2            ; Fog lamps
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

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_READY 0        ; Normal operation of the light controller
#define STARTUP_MODE_NEUTRAL 0x10   ; Waiting before reading ST/TH neutral
#define STARTUP_MODE_REVERSING 0x20 ; Waiting for Forward/Left to obtain direction

#define LIGHT_TABLE_LOCAL 0
#define LIGHT_TABLE_LOCAL_HALF 1
#define LIGHT_TABLE_SLAVE 2
#define LIGHT_TABLE_SLAVE_HALF 3
#define LIGHT_TABLE_LOCAL_SETUP 4
#define LIGHT_TABLE_SLAVE_SETUP 5

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
    CBLOCK  0x20

    steering
    steering_abs
    throttle
    throttle_abs
    ch3
    

    drive_mode_counter
    drive_mode_brake_disarm_counter   
    indicator_state_counter
    blink_counter

    ch3_flags
    ch3_click_counter
    ch3_clicks

    blink_mode      
    light_mode
    drive_mode
    indicator_state
    setup_mode
    startup_mode
    reversing_mode

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
    temp: 2     ; Reserve an extra byte labeled temp+1 

    wl          ; Temporary parameters for 16 bit math functions
    wh
    xl
    xh
    yl 
    yh
    zl
    zh

    port_dummy  ; Register to fake IO ports
    ENDC

    IFDEF DEBUG
    CBLOCK
    send_hi
    send_lo

    debug_steering_old
    debug_throttle_old
    debug_indicator_state_old

    ENDC
    ENDIF

;******************************************************************************
;* MACROS
;******************************************************************************
swap_x_y    macro   x, y
    ; Currently X contains A; Y contains B
    movf    x, w    ; W = A
    xorwf   y, w    ; W = A ^ B
    xorwf   x, f    ; X = ((A^B)^A) = B
    xorwf   y, f    ; Y = ((A^B)^B) = A
    ; Now X contains B. Y contains A.
            endm



;******************************************************************************
; Reset vector 
;******************************************************************************
.code_reset CODE    0x000           
    goto    Init


;******************************************************************************
; Relocatable code section
;******************************************************************************
    CODE

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
    ; Port direction (macro included from io_master.tmp)
    IO_INIT_SLAVE


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
    movlw   b'10010000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (enable reception for SLAVE)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG


    movlw   b'00001010'
            ; |||||||+ CCPM0 (Compare mode, generate software interrupt on 
            ; ||||||+- CCPM1  match (CCP1IF bit is set, CCP1 pin is unaffected)
            ; |||||+-- CCPM2 
            ; ||||+--- CCPM3 
            ; |||+---- CCP1Y (not used)
            ; ||+----- CCP1X (not used)
            ; |+------ 
            ; +------- 
    movwf   CCP1CON
    

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter


    ; Load steering servo values from the EEPROM
    call    Servo_load_values

    clrf    temp
    clrf    temp+1
    comf    temp+1, f
    call    TLC5916_send

;   goto    Main_loop    


;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_UART
    
    call    Process_ch3_double_click
    call    Process_drive_mode
    call    Process_indicators
    call    Process_steering_servo
    call    Service_timer0

    call    Output_local_lights
    call    Make_servo_pulse

    IFNDEF  DEBUG
    ;call    Output_slave
    ENDIF

    goto    Main_loop


;******************************************************************************
; Read_UART
;
; This function returns after having successfully received a complete
; protocol frame via the UART.
;******************************************************************************
Read_UART
    IFDEF   DEBUG

    ; TODO: add some useful debug code here

    return
    ENDIF


    call    read_UART_byte
    sublw   SLAVE_MAGIC_BYTE        ; First byte the magic byte?
    bnz     Read_UART               ; No: wait for 0x8f to appear

read_UART_byte_2
    call    read_UART_byte
    movwf   steering                ; Store 2nd byte
    sublw   SLAVE_MAGIC_BYTE        ; Is it the magic byte?
    bz      read_UART_byte_2        ; Yes: we must be out of sync...

read_UART_byte_3
    call    read_UART_byte
    movwf   throttle
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2

read_UART_byte_4
    call    read_UART_byte
    movwf   ch3                     ; The 3rd byte contains information for
    movwf   startup_mode            ;  CH3 as well as startup_mode
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2
    
    movlw   0x01                    ; Remove startup_mode bits from CH3
    andwf   ch3, f   
    movlw   0x30                    ; Remove CH3 bits from startup_mode
    andwf   startup_mode, f   

    ; Calculate abs(throttle) and abs(steering) for easier math.
    movfw   throttle
    movwf   throttle_abs
    btfsc   throttle_abs, 7
    decf    throttle_abs, f
    btfsc   throttle_abs, 7
    comf    throttle_abs, f

    movfw   steering
    movwf   steering_abs
    btfsc   steering_abs, 7
    decf    steering_abs, f
    btfsc   steering_abs, 7
    comf    steering_abs, f
    
    return


;******************************************************************************
; read_UART_byte
;
; Recieve one byte from the UART in W.
;
; To enable reception of a byte, CREN must be 1. 
;
; On any error, recover by pulsing CREN low then back to high. 
;
; When a byte has been received the RCIF flag will be set. RCIF is 
; automatically cleared when RCREG is read and empty. RCREG is double buffered, 
; so it is a two byte deep FIFO. If a third byte comes in, then OERR is set. 
; You can still recover the two bytes in the FIFO, but the third (newest) is 
; lost. CREN must be pulsed negative to clear the OERR flag. 
;
; On a framing error FERR is set. FERR is automatically reset when RCREG is 
; read, so errors must be tested for *before* RCREG is read. It is *NOT* 
; recommended that you ignore the error flags. Eventually an error will cause 
; the receiver to hang up if you don't clear the error condition.
;******************************************************************************
read_UART_byte
	btfsc   RCSTA, OERR
	goto    overerror       ; if overflow error...
	btfsc   RCSTA, FERR
	goto	frameerror      ; if framing error...
uart_ready
	btfss	PIR1, RCIF
	goto	read_UART_byte  ; if not ready, wait...	

uart_gotit
	bcf     INTCON, GIE     ; Turn GIE off. This is IMPORTANT!
	btfsc	INTCON, GIE     ; MicroChip recommends this check!
	goto 	uart_gotit      ; !!! GOTCHA !!! without this check
                            ;   you are not sure gie is cleared!
	movf	RCREG, w        ; Read UART data
	bsf     INTCON, GIE     ; Re-enable interrupts
	return

overerror	   		
    ; Over-run errors are usually caused by the incoming data building up in 
    ; the fifo. This is often the case when the program has not read the UART
    ; in a while. Flushing the FIFO will allow normal input to resume.
    ; Note that flushing the FIFO also automatically clears the FERR flag.
    ; Pulsing CREN resets the OERR flag.

	bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto 	overerror

	bcf     RCSTA, CREN     ; Pulse CREN off...
	movf	RCREG, w        ; Flush the FIFO, all 3 elements
	movf	RCREG, w		
	movf	RCREG, w
	bsf     RCSTA, CREN     ; Turn CREN back on. This pulsing clears OERR
	bsf     INTCON, GIE
	goto	read_UART_byte  ; Try again...

frameerror			
    ; Framing errors are usually due to wrong baud rate coming in.

	bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto 	frameerror

	movf	RCREG,w		;reading rcreg clears ferr flag.
	bsf     INTCON, GIE
	goto	read_UART_byte  ; Try again...


;******************************************************************************
; Output_local_lights
;******************************************************************************
Output_local_lights
    movf    startup_mode, f
    bnz     output_local_startup

    movf    setup_mode, f
    bnz     output_local_lights_setup

    movlw   1 << LIGHT_TABLE_LOCAL
    movwf   d0
    call    Output_get_state

    IFDEF   DUAL_TLC5916
    movf    temp, w         
    movwf   temp+1

    movlw   1 << LIGHT_TABLE_LOCAL_HALF
    movwf   d0
    call    Output_get_state
    ENDIF

    call    TLC5916_send
    return

output_local_lights_setup
    movlw   1 << LIGHT_TABLE_LOCAL_SETUP
    movwf   d0
    call    Output_get_setup_state
    IFDEF   DUAL_TLC5916
    movfw   temp
    movwf   temp+1
    clrf    temp
    ENDIF
    call    TLC5916_send
    return    

output_local_startup
    swapf   startup_mode, w     ; Move bits 4..7 to 0..3
    andlw   0x07                ; Mask out bits 0..2
    movwf   temp+1
    clrf    temp
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

    movf    setup_mode, f
    bnz     output_slave_setup

    movlw   1 << LIGHT_TABLE_SLAVE
    movwf   d0
    call    Output_get_state
    movf    temp, w         ; LED data for full brightness
    call    UART_send_w        

    movlw   1 << LIGHT_TABLE_SLAVE_HALF
    movwf   d0
    call    Output_get_state
    movf    temp, w         ; LED data for half brightness
    call    UART_send_w        

output_slave_servo
    movf    servo, w        ; Steering wheel servo data
    call    UART_send_w        
    return

output_slave_setup
    movlw   1 << LIGHT_TABLE_SLAVE_SETUP
    movwf   d0
    call    Output_get_setup_state
    movf    temp, w         ; LED data for full brightness
    call    UART_send_w        

    clrf    temp            ; LED data for half brightness: N/A for setup
    call    UART_send_w        
    goto    output_slave_servo

    
;******************************************************************************
; Output_get_state
;
; d0 contains the light table index to process.
; Resulting lights are stored in temp.
;******************************************************************************
Output_get_state
    clrf    temp

    ; Parking lights
    btfss   light_mode, LIGHT_MODE_PARKING
    goto    output_local_get_state_low_beam
    movlw   0
    call    light_table
    iorwf   temp, f

    ; Low beam
output_local_get_state_low_beam
    btfss   light_mode, LIGHT_MODE_LOW_BEAM
    goto    output_local_get_state_fog
    movlw   1
    call    light_table
    iorwf   temp, f

    ; Fog lamps    
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
; Output_get_setup_state
;
; d0 contains the light table index to process.
; Resulting lights are stored in temp.
;******************************************************************************
Output_get_setup_state
    movlw   0    
    btfsc   setup_mode, 2
    addlw   1
    btfsc   setup_mode, 3
    addlw   1
    call    light_table
    movwf   temp
    return


;******************************************************************************
; light_table
;
; Retrieve a line from the light table.
; w: the line we request
; d0 indicates which light table we request:
;   0:  local
;   1:  local_half
;   2:  slave
;   4:  slave_half
;   8:  local_setup
;   16: slave_setup
;
; Resulting light pattern is in w
;******************************************************************************
light_table
    IFNDEF  PREPROCESSING_MASTER        ; {
    btfsc   d0, LIGHT_TABLE_LOCAL
    goto    local_light_table
    btfsc   d0, LIGHT_TABLE_LOCAL_HALF
    goto    local_light_half_table
    btfsc   d0, LIGHT_TABLE_SLAVE
    goto    slave_light_table
    btfsc   d0, LIGHT_TABLE_SLAVE_HALF
    goto    slave_light_half_table
    btfsc   d0, LIGHT_TABLE_LOCAL_SETUP              
    goto    local_setup_light_table
    btfsc   d0, LIGHT_TABLE_SLAVE_SETUP               
    goto    slave_setup_light_table
    ENDIF                               ; }
    return



    
;******************************************************************************
Make_servo_pulse    
    movf    servo, w
    addlw   120
    movwf   xl
    clrf    xh
    call    Mul_x_by_6
    call    Add_x_and_780

    clrf    T1CON           ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H           ; Reset the timer to 0
    clrf    TMR1L
    movf    xl, w           ; Load Timer1 compare register with the servo time
    movwf   CCPR1L
    movf    xh, w
    movwf   CCPR1H
    bcf     PIR1, CCP1IF    ; Clear Timer1 compare interrupt flag
  
    bsf     T1CON, 0        ; Start timer 1
    bsf     PORT_SERVO      ; Set servo port to high pulse

    btfss   PIR1, CCP1IF    ; Wait for compare value reached
    goto    $ - 1

    bcf     PORT_SERVO      ; Turn off servo pulse
    clrf    T1CON           ; Stop timer 1
    bcf     PIR1, CCP1IF

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
; Process_ch3_double_click
;******************************************************************************
Process_ch3_double_click
    movf    startup_mode, f
    bz      process_ch3_no_startup
    return

process_ch3_no_startup
    btfsc   ch3_flags, CH3_FLAG_INITIALIZED
    goto    process_ch3_initialized

    ; Ignore the potential "toggle" after power on
    bsf     ch3_flags, CH3_FLAG_INITIALIZED
    bcf     ch3_flags, CH3_FLAG_LAST_STATE
    btfsc   ch3, CH3_FLAG_LAST_STATE
    bsf     ch3_flags, CH3_FLAG_LAST_STATE
    return

process_ch3_initialized
    movfw   ch3
    xorwf   ch3_flags, w
    movwf   temp
    btfss   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_click_timeout

    bcf     ch3_flags, CH3_FLAG_LAST_STATE
    btfsc   ch3, CH3_FLAG_LAST_STATE
    bsf     ch3_flags, CH3_FLAG_LAST_STATE
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter

    IFDEF   DEBUG
    movlw   0x43                    ; send 'C'
    call    UART_send_w        
    ENDIF
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
    IFDEF   DEBUG
    movlw   0x50                    ; send 'P'
    call    UART_send_w        
    ENDIF

    decfsz  ch3_clicks, f                
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click: switch light mode up (Parking, Low Beam, Fog, High Beam) 
    rlf     light_mode, f
    bsf     light_mode, LIGHT_MODE_PARKING
    movlw   0x03
    andwf   light_mode, f
    IFDEF   DEBUG
    movlw   0x31                    ; send '1'
    call    UART_send_w        
    ENDIF
    return

process_ch3_double_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_triple_click

    ; --------------------------
    ; Double click: switch light mode down (Parking, Low Beam, Fog, High Beam)  
    rrf     light_mode, f
    movlw   0x03
    andwf   light_mode, f
    IFDEF   DEBUG
    movlw   0x32                    ; send '2'
    call    UART_send_w        
    ENDIF
    return

process_ch3_triple_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_quad_click

    ; --------------------------
    ; Triple click: all lights on/off
    movlw   0x03
    andwf   light_mode, w
    sublw   0x03
    movlw   0x03
    skpnz
    movlw   0x00     
    movwf   light_mode
    IFDEF   DEBUG
    movlw   0x33                    ; send '3'
    call    UART_send_w        
    ENDIF
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
    IFDEF   DEBUG
    movlw   0x34                    ; send '4'
    call    UART_send_w        
    ENDIF
    return

process_ch3_8_click
    movlw   4
    subwf   ch3_clicks, w
    bnz     process_ch3_click_end

    movlw   1
    movwf   setup_mode    
    IFDEF   DEBUG
    movlw   0x38                    ; send '8'
    call    UART_send_w        
    ENDIF

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
    IF 0
    movf    indicator_state, w
    addwf   PCL, f

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
    ENDIF

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
;       (right - centre) * abs(steering)
;       -------------------------------- + centre
;                 100
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
    movf    servo_setup_centre, w         
    movwf   servo_centre
    call    Servo_store_values
    clrf    setup_mode
    return

process_steering_servo_setup_cancel
    clrf    setup_mode
    call    Servo_load_values
    return

process_steering_servo_no_setup
    movf    steering_abs, f
    bnz     process_steering_servo_not_centre
    movf    servo_centre, w
    movwf   servo    
    return

process_steering_servo_not_centre
    movf    servo_epr, w
    btfsc   steering, 7
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
    movf    steering_abs, w
    call    Mul_xl_by_w
    movlw   100
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
    IFDEF   DEBUG
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   69                  ; 'E'   
    call    UART_send_w
    movlw   114                 ; 'r'   
    call    UART_send_w
    movlw   100                 ; 'd'   
    call    UART_send_w
    movlw   0x20                ; Space   
    ENDIF

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

    IFDEF   DEBUG
    call    UART_send_w
    movf    servo_epl, w
    call    UART_send_signed_char
    movf    servo_centre, w
    call    UART_send_signed_char
    movf    servo_epr, w
    call    UART_send_signed_char
    movlw   0x0a                ; LF  
    call    UART_send_w
    ENDIF

    return


;******************************************************************************
; Servo_store_values
; 
;******************************************************************************
Servo_store_values
    IFDEF   DEBUG
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
    movlw   0x0a                ; LF   
    call    UART_send_w
    ENDIF

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
    IFDEF   DEBUG
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
    movlw   0x0a                ; LF   
    call    UART_send_w
    ENDIF

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
; Mul_xl_by_w
;
; Calculates xh/xl = xl * w
;******************************************************************************
#define count d0
Mul_xl_by_w
    clrf    xh
	clrf    count
    bsf     count, 3
    rrf     xl, f

mul_xl_by_w_loop
	skpnc
	addwf   xh, f
    rrf     xh, f
    rrf     xl, f
	decfsz  count, f
    goto    mul_xl_by_w_loop
    return


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
; Add_y_to_x
;
; This function calculates xh/xl = xh/xl + yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
Add_y_to_x
    movf    yl, w
    addwf   xl, f
    movf    yh, w
    skpnc
    incf    yh, W
    addwf   xh, f
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
; Mul_x_by_6
;
; Calculates xh/xl = xl * 6
;
; Generated by www.piclist.com/cgi-bin/constdivmul.exe (1-May-2002 version)
;******************************************************************************
Mul_x_by_6
    ; Shift accumulator left 1 times: xh/xl = xl * 2
	clrc
	rlf	    xl, f
	clrf	xh
	rlf	    xh, f

    ; Copy accumulator to temporary location
	movf	xh, w
	movwf	yh
	movf	xl, w
	movwf	yl

    ; Shift temporary value left 1 times: yh/yl = xl * 4
	clrc
	rlf	    yl, f
	rlf	    yh, f

    ; xh/xl  =  xh/xl + yh/yl  =  xl * 6
	movf	yl, w
	addwf	xl, f
	movf	yh, w
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


;******************************************************************************
; Add_x_and_780
;
; Calculates xh/xl = xh/xl + 780
;******************************************************************************
Add_x_and_780
	movlw	LOW(780)
	addwf	xl, f
	movlw	HIGH(780)
    movwf   yh
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


;******************************************************************************
; TLC5916_send
;
; Sends the value in the temp register to the TLC5916 LED driver.
; In case DUAL_TLC5916 is defined then 16 bits temp, temp+1 are sent. This 
; is used if two TLC5916 are wired up in series for 16 output channels.
;******************************************************************************
TLC5916_send
    IFDEF DUAL_TLC5916      ; {
    movlw   16
    ELSE                    ; } {
    movlw   8
    ENDIF                   ; } DUAL_TLC5916
    movwf   d0

tlc5916_send_loop
    IFDEF DUAL_TLC5916
    rlf     temp+1, f
    ENDIF
    rlf     temp, f
    skpc    
    bcf     PORT_SDI
    skpnc    
    bsf     PORT_SDI
    bsf     PORT_CLK
    bcf     PORT_CLK
    decfsz  d0, f
    goto    tlc5916_send_loop

    bsf     PORT_LE
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


    IFDEF   DEBUG
;**********************************************************************
Debug_output_values

#define setup_mode_old debug_indicator_state_old
#define servo_old debug_throttle_old

    IF 0
debug_output_setup
    movf    setup_mode, w
    subwf   setup_mode_old, w
    bnz     debug_output_servo
    movf    servo, w
    subwf   servo_old, w
    bz      debug_output_indicator

debug_output_servo
    movlw   83                  ; 'S'   
    call    UART_send_w
    movlw   101                 ; 'e'   
    call    UART_send_w
    movlw   116                 ; 't'   
    call    UART_send_w
    movlw   117                 ; 'u'   
    call    UART_send_w
    movlw   112                 ; 'p'   
    call    UART_send_w
    movlw   0x20                ; Space
    call    UART_send_w
    movf    setup_mode, w
    movwf   setup_mode_old
    call    UART_send_signed_char
    movlw   0x20                ; Space
    call    UART_send_w
    movlw   83                  ; 'S'   
    call    UART_send_w
    movlw   101                 ; 'e'   
    call    UART_send_w
    movlw   114                 ; 'r'   
    call    UART_send_w
    movlw   118                 ; 'v'   
    call    UART_send_w
    movlw   111                 ; 'o'   
    call    UART_send_w
    movlw   0x20                ; Space
    call    UART_send_w
    movf    servo, w
    movwf   servo_old
    call    UART_send_signed_char
    movlw   0x0a                ; LF
    call    UART_send_w
    ENDIF    

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
    movlw   0x0a                ; LF
    call    UART_send_w
    ENDIF

debug_output_steering
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
    movlw   0x0a                ; LF
    call    UART_send_w

debug_output_throttle
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
    movlw   0x0a                ; LF
    call    UART_send_w

;    movf    drive_mode, w
;    call    UART_send_signed_char
;    movlw   0x0a                ; LF
;    call    UART_send_w

debug_output_end
    return
    ENDIF

    IF 0
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
    ENDIF

    IF 1
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
    ENDIF


;******************************************************************************
; Send W out via the UART
;******************************************************************************
UART_send_w
    btfss   PIR1, TXIF
    goto    UART_send_w ; Wait for transmitter interrupt flag

    movwf   TXREG	    ; Send data stored in W
    return    


;******************************************************************************
; Send W, which is treated as signed char, as human readable number via the
; UART.
;******************************************************************************
    IFDEF   DEBUG
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
    ENDIF
  

    IFDEF   DEBUG
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

;    movlw   0x0a
;    call    UART_send_w

    return
    ENDIF

    IFDEF   DEBUG
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
UART_send_CR
    movlw   0x0a
    call    UART_send_w
    return
    ENDIF


    END     ; Directive 'end of program'



