    TITLE       Light tables for Wye Tuck's Land Rover Defender S2 SWB
    RADIX       dec

    #include    hw.tmp
    
    
    GLOBAL Init_lights
    GLOBAL Output_lights

    
    ; Functions and variables imported from utils.asm
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
d0  res 1
d1  res 1

;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    BANKSEL light_data
    incf    light_data, f
    
    movf    light_data, w
    BANKSEL SSP1BUF
    movwf   SSP1BUF
    
    btfss   SSP1STAT, BF    ; Wait for transmit done flag BF being set
    goto    $-1
    movf    SSP1BUF, w      ; Clears BF flag

    call    Delay_1ms

    goto    Init_lights

    return


;******************************************************************************
; Delay_1ms
;
; Delays for exactly 1ms at 32 MHz FOSC
;******************************************************************************
Delay_1ms
    BANKSEL d0
	movlw	0x3E
	movwf	d0
	movlw	0x07
	movwf	d1
delay_1ms_0
	decfsz	d0, f
	goto	$+2
	decfsz	d1, f
	goto	delay_1ms_0

	goto	$+1
	nop
	return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
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
