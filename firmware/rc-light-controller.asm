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


#define PORT_TEST_LED   PORTA, 0
#define PORT_TEST_LED2  PORTA, 1
#define PORT_TEST_LED3  PORTB, 3

#define PORT_CH3        PORTB, 5
#define PORT_STEERING   PORTB, 0
#define PORT_THROTTLE   PORTB, 1

; Bitfields in variable servo_available that indicate availability of a 
; particular servo channel
#define CH3 0    
#define THROTTLE 1   
#define STEERING 2    

#define CH3_BUTTON_TIMEOUT 6    ; Time in which we accept double-click of CH3
#define BLINK_COUNTER_VALUE 5   ; 5 * 65.536 ms = ~333 ms = ~1.5 Hz

#define PWM_OFF 0
#define PWM_HALF 0x0f
#define PWM_FULL 0x3f

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
    CBLOCK  0x20

	d0          ; Delay registers
	d1
	d2
	d3
    temp

    wl          ; Temporary parameters for 16 bit functions
    wh
    xl
    xh
    yl 
    yh
    zl
    zh

    send_hi
    send_lo

    servo_available

    steering
    steering_l
    steering_h
    steering_epl_l
    steering_epl_h
    steering_epr_l
    steering_epr_h

    throttle
    throttle_l
    throttle_h
    throttle_centre_l
    throttle_centre_h
    throttle_epl_l
    throttle_epl_h
    throttle_epr_l
    throttle_epr_h
    
    ch3
    ch3_value
    ch3_ep0
    ch3_ep1

    blink_counter
    ch3_click_counter
    ch3_clicks

    mode
    light_mode      ; Light mode: 
                    ;  Bit 0: Stand light
                    ;  Bit 1: Head light
                    ;  Bit 2: Fog lights
                    ;  Bit 3: High beam

    ENDC

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
; Interrupt vector 
;******************************************************************************
    ORG     0x004           


;******************************************************************************
; Process_throttle
;   If POS == CEN:          ; We found dead centre
;       POS_NORMALIZED = 0
;   Else
;       If EPR < CEN:       ; Servo REVERSED
;           If POS < CEN:   ; We are dealing with a right turn
;               POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPR)
;           Else            ; We are dealing with a left turn
;               POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPL)
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;       Else                ; Servo NORMAL
;           If POS < CEN:   ; We are dealing with a left turn
;               POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPL)
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;           Else            ; We are dealing with a right turn
;               POS_NORMALIZED = calculate_normalized_servo_pos(CEN, POS, EPR)
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

    clrf    throttle
    return

throttle_is_valid
    ; Throttle in centre? (note that we preloaded xh/xl just before this)
    ; If yes then set throttle output variable to '0'
    movf    throttle_centre_h, w
    movwf   yh
    movf    throttle_centre_l, w
    movwf   yl
    call    If_x_eq_y
    bnz     throttle_off_centre

    clrf    throttle
    return

throttle_off_centre
    movf    throttle_centre_h, w
    movwf   xh
    movf    throttle_centre_l, w
    movwf   xl
    movf    throttle_epr_h, w
    movwf   yh
    movf    throttle_epr_l, w
    movwf   yl
    call    If_y_lt_x
    bc      throttle_normal

    movf    throttle_h, w
    movwf   yh
    movf    throttle_l, w
    movwf   yl   
    call    If_y_lt_x
    bc      throttle_left

throttle_right
    movf    throttle_epr_h, w
    movwf   zh
    movf    throttle_epr_l, w
    movwf   zl
    call    calculate_normalized_servo_position
    movwf   throttle
    return    

throttle_normal
    movf    throttle_h, w
    movwf   yh
    movf    throttle_l, w
    movwf   yl   
    call    If_y_lt_x
    bc      throttle_right

throttle_left
    movf    throttle_epl_l, w
    movwf   zh
    movf    throttle_epl_l, w
    movwf   zl
    call    calculate_normalized_servo_position
    sublw   0
    movwf   throttle
    return    


;******************************************************************************
; xh/xl: CEN centre pulse width
; yh/yl: POS servo measured pulse width
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
calculate_normalized_servo_position
    ; First swap POS and EP to get EP in the y variable for If_y_lt_x
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
    bc      calculate_normalized_left

    movlw   100
    return

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
    call    Mul_x_by_10     ; xh/hl =  ((CEN - POS) / 4) * 100

    swap_x_y    wh, xh
    swap_x_y    wl, xl
    swap_x_y    yh, zh
    swap_x_y    yl, zl

    ; w = ((CEN - POS) / 4) * 100, x = CEN, y = EP, z = POS

    call    Sub_y_from_x    ; xh/hl =  CEN - EP
    call    Div_x_by_4      ; xh/hl =  (CEN - EP) / 4
    call    Mul_x_by_10     ; xh/hl =  ((CEN - EP) / 4) * 100

    swap_x_y    xh, yh
    swap_x_y    xl, yl
    swap_x_y    wh, xh
    swap_x_y    wl, xl

    ; x = ((CEN - POS) / 4) * 100, y = ((CEN - EP) / 4) * 100

    call    Div_x_by_y
    movf    xl, w
    return    

calculate_ep_gt_cen
    movfw   yl
    subwf   zl, w
    movfw   yh
    skpc                
    incfsz  yh, w       
    subwf   zh, w
    bnc     calculate_normalized_right

    movlw   100
    return

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
    call    Mul_x_by_10     ; xh/hl =  ((POS - CEN) / 4) * 100

    swap_x_y    xh, wh
    swap_x_y    xl, wl
    swap_x_y    xh, zh
    swap_x_y    xl, zl

    ; w = ((POS - CEN) / 4) * 100, x = EP, y = CEN

    call    Sub_y_from_x    ; xh/hl =  EP - CEN
    call    Div_x_by_4      ; xh/hl =  (EP - CEN) / 4
    call    Mul_x_by_10     ; xh/hl =  ((EP - CEN) / 4) * 100

    swap_x_y    xh, yh
    swap_x_y    xl, yl
    swap_x_y    wh, xh
    swap_x_y    wl, xl

    ; x = ((POS - CE) / 4) * 100, y = ((EP - CEN) / 4) * 100

    call    Div_x_by_y
    movf    xl, w
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
Mul_x_by_10
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
    movf    yl, w
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

    movlw   b'00000000' ; Make all ports A output
    movwf   TRISA

    movlw   b'00100110' ; Make all ports B output, except RB5, 
                        ;  RB2 (UART) and RB1 (UART!)
    movwf   TRISB

    movlw   0x3f        ; Limit the PWM to 8 bit
    movwf   PR2


    BANKSEL servo_available
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

    ; For now only define CH3 as being available
    clrf    servo_available
    bsf     servo_available, CH3

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

    ; Initialize Timer 2 for PWM
    movlw   0x00
    movwf   CCPR1L 
    movlw   b'00001100'
            ; |||||||+ CCP1M0 (PWM mode)
            ; ||||||+- CCP1M1 
            ; |||||+-- CCP1M2
            ; ||||+--- CCP1M3
            ; |||+---- CCP1Y (PWM LSB)
            ; ||+----- CCP1X (PWM LSB+1)
            ; |+------ (not implemented)
            ; +------- (not implemented)
    movwf   CCP1CON
    movlw   b'00000110'
            ; |||||||+ T2CKPS0 (Prescaler 1:16)
            ; ||||||+- T2CKPS1 
            ; |||||+-- TMR2ON (Timer2 On bit)
            ; ||||+--- TOUTPS0 (Postscale 1:1)
            ; |||+---- TOUTPS1 
            ; ||+----- TOUTPS2 
            ; |+------ TOUTPS3 
            ; +-------      (not implemented)
    movwf   T2CON


    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter

;   goto    Main_loop    

;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_ch3
    call    Read_throttle
;    call    Read_steering

    call    Process_ch3
;    call    Process_throttle
;    call    Process_steering

    call    Process_ch3_double_click

    call    Service_timer0

    btfsc   light_mode, 0
    bsf     PORT_TEST_LED
    btfss   light_mode, 0
    bcf     PORT_TEST_LED

    btfsc   light_mode, 1
    bsf     PORT_TEST_LED2
    btfss   light_mode, 1
    bcf     PORT_TEST_LED2

;    btfsc   light_mode, 2
;    bsf     PORT_TEST_LED3
;    btfss   light_mode, 2
;    bcf     PORT_TEST_LED3

    movlw   PWM_OFF
    btfsc   light_mode, 2
    movlw   PWM_HALF
    btfsc   light_mode, 3
    movlw   PWM_FULL
    movwf   CCPR1L 


    goto    Main_loop

;******************************************************************************
; Process_ch3_double_click
;******************************************************************************
Process_ch3_double_click
    btfss   ch3, 1
    goto    process_ch3_click_timeout

    bcf     ch3, 1
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter

    movlw   0x43                    ; send 'C'
    goto    UART_send_w        
    return
    
process_ch3_click_timeout
    movf    ch3_clicks, f           ; Any buttons pending?
    skpnz   
    return                          ; No: done

    movf    ch3_click_counter, f    ; Double-click timer expired?
    skpz   
    return                          ; No: wait for more buttons

    movlw   0x50                    ; send 'P'
    call    UART_send_w        

    decfsz  ch3_clicks, f                
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click: switch light mode up (Stand, Head, Fog, High Beam) 
    rlf     light_mode, f
    bsf     light_mode, 0
    movlw   0x0f
    andwf   light_mode, f
    movlw   0x31                    ; send '1'
    goto    UART_send_w        
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
    goto    UART_send_w        
    return

process_ch3_triple_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_quad_click

    ; --------------------------
    ; Triple click: Hazard lights on/off  
    movlw   0x02
    xorwf   mode, f
    movlw   0x33                    ; send '3'
    goto    UART_send_w        
    return

    ; --------------------------
    ; Quad click: all lights off
process_ch3_quad_click
    clrf    ch3_clicks
    clrf    light_mode
    movlw   0x34                    ; send '4'
    goto    UART_send_w        
    return


;******************************************************************************
; Service_timer0
;******************************************************************************
Service_timer0
    btfss   INTCON, T0IF
    return

    bcf     INTCON, T0IF

    movf    ch3_click_counter, f
    skpz     
    decf    ch3_click_counter, f    

    decfsz  blink_counter, f
    return

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    movlw   0x01
    xorwf   mode, f
    return

;******************************************************************************
; Read_ch3
; 
; Read servo channel 3 and write the result in ch3_h/ch3_l
;
; TODO: add timeouts!
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

    movf    TMR1H, w    
    movwf   ch3_value
    movf    TMR1L, w
    movwf   temp
  
    ; Use the middle 12 bit as an 8 bit value since we don't need high
    ; accuracy for the CH3 
    rlf     temp, f
    rlf     ch3_value, f
    rlf     temp, f
    rlf     ch3_value, f
    rlf     temp, f
    rlf     ch3_value, f
    rlf     temp, f
    rlf     ch3_value, f
    return


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

    ; Terminate if the servo is indicated as "not connected"
    btfss   servo_available, STEERING
    return

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

    movf    TMR1H, w    
    movwf   steering_h
    movf    TMR1L, w
    movwf   steering_l
    return


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

    ; Terminate if the servo is indicated as "not connected"
    btfss   servo_available, THROTTLE
    return

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

    movf    TMR1H, w    
    movwf   throttle_h
    movf    TMR1L, w
    movwf   throttle_l
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
;******************************************************************************
Process_steering
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
; Max
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
;   Everything below 600 will be considered invalid.
;   Everything between 600 and 800 will be clamped to 800.
;   Everything between 2300 and 2500 will be clamped to 2300
;   Everything above 2500 will be considered invalid.
;
;   Timeout for measuring high pulse: Use TMR1H bit 4: If set, more than 
;   4096 ms have expired!
;   Timeout for waiting for pulse to go high: Use TMR1H bit 7: If set, more 
;   than 32768 ms have expired! 
;   These tests allow us to use cheap bit test instructions.
;
;
;   Defaults for steering and throttle:
;   1000 .. 1500 .. 2000
;   
;   Defaults for CH3:
;   1000, 2000
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
;       CEN (neutral position)
;           Margin for neutral position: +/- 5%
;           Some speed controlled can configure this from 4..8%
;       POS (measured servo pulse length)
;   If EPL > EPR:
;       EPL > CEN > EPR must be true
;   If EPL < EPR:
;       EPL < CEN < EPR must be true
;
;   We need to convert POS into a range of 
;       -100 .. 0 .. +100   (left .. centre .. right)
;   Note: this normalizes left and right! Due to servo reversing EPL may
;   have a larger or smaller time value than EPR.
;
;   If POS == CEN:          ; We found dead centre
;       POS_NORMALIZED = 0
;   Else
;       If EPL > CEN:       ; Servo REVERSED
;           If POS > CEN:   ; We are dealing with a left turn
;               POS_NORMALIZED = calculate(POS, EPL, CEN)
;               POS_NORMALIZED = 0 - POS_NORMALIZED
;           Else            ; We are dealing with a right turn
;               POS_NORMALIZED = calculate(POS, EPR, CEN)
;       Else                ; Servo NORMAL
;           If POS > CEN:   ; We are dealing with a right turn
;               POS_NORMALIZED = calculate(POS, EPR, CEN)
;           Else            ; We are dealing with a left turn
;               POS_NORMALIZED = calculate(POS, EPL, CEN)
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
;   RA5:    Servo input (Vpp double-usage)
;   RB7:    Servo input (PGD double-usage)
;   RB6:    Servo input (PGC double-usage)
;   RB1:    Connected to RB6 (RX double-usage for slave!
;   RB2:    Slave out (TX Master) / Servo out (Slave)
;   RB3:    LED output with PWM (CCP1)
;   
;   Ports with no special assignment:
;   RA0
;   RA1
;   RA2
;   RA3
;   RA4
;   RA6
;   RA7
;   RB0
;   RB4
;   RB5
;******************************************************************************

