;******************************************************************************
;
;   servo-reader.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - Servo reader
    LIST        r=dec
    RADIX       dec

    #include    hw.tmp


    GLOBAL Read_all_channels
    GLOBAL Init_reader

    GLOBAL steering            
    GLOBAL steering_abs       
    GLOBAL steering_reverse
    GLOBAL throttle            
    GLOBAL throttle_abs       
    GLOBAL ch3  


    ; Functions imported from utils.asm
    EXTERN Min
    EXTERN Max
    EXTERN Add_y_to_x
    EXTERN Sub_y_from_x
    EXTERN If_x_eq_y
    EXTERN If_y_lt_x
    EXTERN Min_x_z
    EXTERN Max_x_z
    EXTERN Mul_xl_by_w
    EXTERN Div_x_by_y
    EXTERN Mul_x_by_100
    EXTERN Div_x_by_4
    EXTERN Mul_x_by_6
    EXTERN Add_x_and_780    
    
    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh

    EXTERN startup_mode


; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

; Bitfields in variable flags
#define TH_FLAG_REVERSING_INITIALZED 3


; The initial endpoint delta that is used right after initialization of the
; neutrals. 
; Example: 
;   ep-left = centre - INITIAL_ENDPOINT_DELTA
;   ep-right = centre + INITIAL_ENDPOINT_DELTA
IFNDEF INITIAL_ENDPOINT_DELTA
#define INITIAL_ENDPOINT_DELTA 200
ENDIF

; Specify the frequency in Hz with which the receiver outputs the servo signals.
; Usually this is somewhere betwen 50 and 60 Hz. 
; The HobbyKing HK310 uses 60 Hz.
IFNDEF RECEIVER_OUTPUT_RATE 
#define RECEIVER_OUTPUT_RATE 60
ENDIF

; Time in milliseconds to wait after startup before the neutral positions
; are set and normal operation resumes
IFNDEF STARTUP_WAIT_TIME
#define STARTUP_WAIT_TIME 3000
ENDIF

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_servo_reader UDATA

throttle            res 1
throttle_abs        res 1    
throttle_l          res 1
throttle_h          res 1
throttle_centre_l   res 1
throttle_centre_h   res 1
throttle_epl_l      res 1
throttle_epl_h      res 1
throttle_epr_l      res 1
throttle_epr_h      res 1
throttle_reverse    res 1

steering            res 1
steering_abs        res 1
steering_l          res 1
steering_h          res 1
steering_centre_l   res 1
steering_centre_h   res 1
steering_epl_l      res 1
steering_epl_h      res 1
steering_epr_l      res 1
steering_epr_h      res 1
steering_reverse    res 1

ch3                 res 1
ch3_value           res 1
ch3_ep0             res 1
ch3_ep1             res 1

flags               res 1

init_prescaler      res 1
init_counter        res 1

wl                  res 1
wh                  res 1

d0                  res 1
d1                  res 1
d2                  res 1
d3                  res 1
temp                res 1


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
; Relocatable code section
;******************************************************************************
.code_servo_reader CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init_reader
    ; Load defaults for end points for position 0 and 1 of CH3; discard lower
    ; 4 bits so our math can use bytes only
    movlw   1000 >> 4
    movwf   ch3_ep0

    movlw   2000 >> 4
    movwf   ch3_ep1

    movlw   1 << STARTUP_MODE_NEUTRAL
    movwf   startup_mode    
    
    movlw   RECEIVER_OUTPUT_RATE / 10
    movwf   init_prescaler
    movlw   STARTUP_WAIT_TIME / 100
    movwf   init_counter
    
    return


;******************************************************************************
;******************************************************************************
Read_all_channels
    IFDEF   SEQUENTIAL_CHANNEL_READING  ; {   
    call    Read_all
    ELSE                                ; } {
    call    Read_steering
    call    Read_throttle
    call    Read_ch3
    ENDIF                               ; } SEQUENTIAL_CHANNEL_READING

    movf    startup_mode, f
    bnz     read_neutral
    
    call    Normalize_steering
    call    Normalize_throttle
    call    Normalize_ch3
    
    btfsc   flags, TH_FLAG_REVERSING_INITIALZED
    return

    ; Throttle reversing initialization starts once neutral initialization has
    ; finished.
    ;
    ; We wait until the throttle_abs signal goes >50. If it does we set 
    ; throttle_reverse if throttle is negative, as we assume that the first
    ; throttle input a user will give is "forward".
    ;
    ; The value of 50% may seem high, but don't forget that at this point
    ; the end points are still set very low as this code is execute right
    ; after initialization!

    movlw   50
    subwf   throttle_abs, w
    bnc     read_waiting_for_throttle_direction

    ; Reverse the throttle if it is currently negative    
    btfsc   throttle, 7
    comf    throttle_reverse
    bsf     flags, TH_FLAG_REVERSING_INITIALZED
    return

read_waiting_for_throttle_direction
    ; Clear throttle data until we have determined the direction
    clrf    throttle
    clrf    throttle_abs
    return

read_neutral
    ; We are initializing so pretend steering and throttle are neutral
    clrf    steering            
    clrf    steering_abs
    clrf    throttle
    clrf    throttle_abs

    call    Normalize_ch3       ; Process CH3 normally even during startup


    ; Wait for a certain time after startup before reading neutral
    ; values of steering and throttle so that the signals have time
    ; to stabilize.
    ;
    ; The time can be set in STARTUP_WAIT_TIME with a resolution of 100ms.
    ; We use the repetition rate of the servo signals for timing. The
    ; repetition frequency is divided by 10 and used as prescaler to achieve
    ; a 100ms interval for decreasing init_counter.
    decfsz  init_prescaler
    return
    movlw   RECEIVER_OUTPUT_RATE / 10
    movwf   init_prescaler
    decfsz  init_counter
    return


    ; Use the current steering and throttle values as "neutral"

    movf    throttle_h, w
    movwf   throttle_centre_h
    movf    throttle_l, w
    movwf   throttle_centre_l

    movf    steering_h, w
    movwf   steering_centre_h
    movwf   xh
    movf    steering_l, w
    movwf   steering_centre_l
    movwf   xl

    ; Now that we have determined the center for steering and throttle, 
    ; we need to set initial endpoints around them. We use a value of +/- 
    ; INITIAL_ENDPOINT_DELTA for that. 
    ; If we would use a fixed initial endpoint value we may get into trouble
    ; due to steering trim/sub-trim.

    movlw   HIGH(INITIAL_ENDPOINT_DELTA)
    movwf   yh
    movlw   LOW(INITIAL_ENDPOINT_DELTA)
    movwf   yl

    call    Add_y_to_x
    movf    xh, w
    movwf   steering_epr_h    
    movf    xl, w
    movwf   steering_epr_l    

    movf    steering_centre_h, w
    movwf   xh
    movf    steering_centre_l, w
    movwf   xl
    call    Sub_y_from_x
    movf    xh, w
    movwf   steering_epl_h    
    movf    xl, w
    movwf   steering_epl_l    

    movf    throttle_centre_h, w
    movwf   xh
    movf    throttle_centre_l, w
    movwf   xl
    call    Add_y_to_x
    movf    xh, w
    movwf   throttle_epr_h    
    movf    xl, w
    movwf   throttle_epr_l    

    movf    throttle_centre_h, w
    movwf   xh
    movf    throttle_centre_l, w
    movwf   xl
    call    Sub_y_from_x
    movf    xh, w
    movwf   throttle_epl_h    
    movf    xl, w
    movwf   throttle_epl_l    
    
    clrf    startup_mode
    return
    
    
;******************************************************************************
; Read_all
; 
; Read all servo channels in one go. 
; This function can only be used if you have verified that your receiver
; is indeed outputting all channels in sequence Steering / Throttle / CH3, one 
; after each other.
; This is true for the HK-3000 receiver. 
;
; By using this function we can read all channels in one "loop", and therefore
; increasing response speed of the light controller.
;******************************************************************************
Read_all
    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H       ; Reset the timer to 0
    clrf    TMR1L

    ; Wait until servo signal is LOW 
    ; This ensures that we do not start in the middle of a pulse
all_st_wait_for_low1
    btfsc   PORT_STEERING
    goto    all_st_wait_for_low1

all_st_wait_for_high
    btfss   PORT_STEERING   ; Wait until servo signal is high
    goto    all_st_wait_for_high

    bsf     T1CON, 0    ; Start timer 1

all_st_wait_for_low2
    btfsc   PORT_STEERING   ; Wait until servo signal is LOW again
    goto    all_st_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    movf    TMR1H, w    ; Store read values temporarily; validate later
    movwf   steering_h
    movf    TMR1L, w
    movwf   steering_l

    ; At this point the throttle signal is already being output
    ; from the receiver -- it takes 100ns after the steering pulse goes
    ; low for the throttle to be high. So we can directly wait for the
    ; throttle to be low again to read the measured value.

    clrf    TMR1H       ; Reset the timer to 0 ... 
    movlw   10          ;  but prime it with the no of instructions until here
    movwf   TMR1L
    bsf     T1CON, 0    ; Start timer 1

all_th_wait_for_low2
    btfsc   PORT_THROTTLE   ; Wait until servo signal is LOW
    goto    all_th_wait_for_low2

    clrf    T1CON       ; Stop timer 1

    movf    TMR1H, w    ; Store read values temporarily; validate later
    movwf   throttle_h
    movf    TMR1L, w
    movwf   throttle_l

    clrf    TMR1H       ; Reset the timer to 0 ... 
    movlw   10          ;  but prime it with the no of instructions until here
    movwf   TMR1L
    bsf     T1CON, 0    ; Start timer 1

all_ch3_wait_for_low2
    btfsc   PORT_CH3    ; Wait until servo signal is LOW
    goto    all_ch3_wait_for_low2

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

    ; We are done now measuring all 3 channels, so do throttle and steering 
    ; validation
    
    movf    steering_h, w    
    movwf   TMR1H
    movf    steering_l, w
    movwf   TMR1L
    call    Validate_servo_measurement
    movf    xh, w    
    movwf   steering_h
    movf    xl, w
    movwf   steering_l

    movf    throttle_h, w    
    movwf   TMR1H
    movf    throttle_l, w
    movwf   TMR1L
    call    Validate_servo_measurement
    movf    xh, w    
    movwf   throttle_h
    movf    xl, w
    movwf   throttle_l

    return


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
; Normalize_ch3
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

Normalize_ch3
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
; Normalize_throttle
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
Normalize_throttle
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
; Normalize_steering
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
Normalize_steering
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

    END     ; Directive 'end of program'



