    TITLE       Light tables for Wye Tuck's Land Rover Defender S2 SWB
    RADIX       dec

    #include    hw.tmp
    
    
    GLOBAL Init_lights
    GLOBAL Output_lights

    
    ; Functions and variables imported from utils.asm
    EXTERN Init_TLC5940    
    EXTERN TLC5940_send
    
    EXTERN xl
    EXTERN xh
    EXTERN temp
    EXTERN light_data

    
    ; Functions and variables imported from master.asm
    EXTERN blink_mode
    EXTERN light_mode
    EXTERN drive_mode
    EXTERN setup_mode
    EXTERN startup_mode
    EXTERN servo


; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_PARKING 0        ; Parking lights
#define LIGHT_MODE_LOW_BEAM 1       ; Low beam

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2
#define DRIVE_MODE_BRAKE_ARMED 3
#define DRIVE_MODE_REVERSE_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5

; Bitfields in variable setup_mode
#define SETUP_MODE_INIT 0
#define SETUP_MODE_CENTRE 1
#define SETUP_MODE_LEFT 2
#define SETUP_MODE_RIGHT 3
#define SETUP_MODE_STEERING_REVERSE 4
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

  
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA
dummy   res 2

;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    call    Init_TLC5940

    BANKSEL dummy
    clrf    dummy
    bsf     dummy+1, 0

light_loop
    BANKSEL dummy
    btfsc   dummy+1, 0
    goto    do_up

    decf    dummy, f
    movfw   dummy
    sublw   0
    bnz     do1 

    bsf     dummy+1, 0
    goto    do1    

do_up
    incf    dummy, f
    movfw   dummy
    sublw   0x3f
    bnz     do1 

    bcf     dummy+1, 0
    goto    do1    

do1
    movfw   dummy
    BANKSEL light_data
    movwf   light_data
    movwf   light_data+1
    movwf   light_data+2
    movwf   light_data+3
    movwf   light_data+4
    movwf   light_data+5
    movwf   light_data+6
    movwf   light_data+7
    movwf   light_data+8
    movwf   light_data+9
    movwf   light_data+10
    movwf   light_data+11
    movwf   light_data+12
    movwf   light_data+13
    movwf   light_data+14
    movwf   light_data+15

    call    TLC5940_send

    ; Wait 10ms between light outputs
    movlw   10
    movwf   temp
    call    Delay_1ms
    decfsz  temp, f
    goto    $-2

    goto    light_loop

    return


;******************************************************************************
; Delay_1ms
;
; Delays for exactly 1ms at 32 MHz FOSC
;******************************************************************************
Delay_1ms
	movlw	0x3E
	movwf	xl
	movlw	0x07
	movwf	xh
delay_1ms_0
	decfsz	xl, f
	goto	$+2
	decfsz	xh, f
	goto	delay_1ms_0

	return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights

    ; Clear light_data, i.e. by default all lights are off.
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
    movlw   16          ; There are 16 bytes in light_data
    movwf   temp
    clrw   
Output_lights_clear_loop
    movwi   FSR0++    
    decfsz  temp, f
    bnz     Output_lights_clear_loop


    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    ; Normal mode here
    return


output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    ; do something...
    return


output_lights_setup
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    output_lights_setup_centre
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    output_lights_setup_right
    btfss   setup_mode, SETUP_MODE_STEERING_REVERSE 
    return

    ; do something for steering reverse
    return

output_lights_setup_centre
    return

output_lights_setup_left
    return
    
output_lights_setup_right
    return

output_lights_execute    
;    call    TLC5916_send
    return


    END
