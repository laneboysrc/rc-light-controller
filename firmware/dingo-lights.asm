    TITLE       Light tables for the Axial SCX10 Dingo
    RADIX       dec

    #include    hw.tmp


    GLOBAL Init_lights
    GLOBAL Output_lights


    ; Functions and variables imported from utils.asm
    EXTERN TLC5916_send
    EXTERN UART_send_w
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
#define LIGHT_MODE_FOG 2            ; Fog lamps
#define LIGHT_MODE_HIGH_BEAM 3      ; High beam

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
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral
#define STARTUP_MODE_REVERSING 5    ; Waiting for Forward/Left to obtain direction

#define LIGHT_TABLE_LOCAL 0
#define LIGHT_TABLE_SLAVE 1
#define LIGHT_TABLE_SLAVE_HALF 2
#define LIGHT_TABLE_LOCAL_SETUP 3
#define LIGHT_TABLE_SLAVE_SETUP 4

    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA

d0                  res 1
temp                res 1


;============================================================================
;============================================================================
;============================================================================
.code_dingo_light_tables CODE 0x010

;============================================================================
local_light_table
    addwf   PCL, f

            ; +------- OUT7     
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'00000000'     ; Parking lights
    dt      b'00000000'     ; Low beam
    dt      b'00010100'     ; Fog lamps
    dt      b'00011100'     ; High beam
    dt      b'00000000'     ; Brake
    dt      b'00000000'     ; Reverse
    dt      b'00000000'     ; Indicator left
    dt      b'00000000'     ; Indicator right
    dt      b'00000000'     ; Hazard lights

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "local_light_table CROSSES PAGE BOUNDARY!"
    ENDIF
    

;============================================================================
slave_light_table
    addwf   PCL, f

            ; BUG! The Dingo uses bit 7, which must be 0 at all times
            ; to preserve the UART protocol, which reserves values 0x80..0x87
            ; for the magic byte used for synchronization. The reason why
            ; it still works is that 0x87 is the magic byte used in the 
            ; Dingo, but since OUT0..OUT2 are unused it can never appear.
            ; TODO: use AND 0x7F in the UART send code in the master.

            ; +------- (not used, must be 0!)     
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'00000000'     ; Parking lights
    dt      b'00001000'     ; Low beam
    dt      b'00001000'     ; Fog lamps
    dt      b'00001000'     ; High beam
    dt      b'00010000'     ; Brake
    dt      b'00100000'     ; Reverse
    dt      b'01000000'     ; Indicator left
    dt      b'10000000'     ; Indicator right
    dt      b'11000000'     ; Hazard lights

    IF ((HIGH ($)) != (HIGH (slave_light_table)))
        ERROR "slave_light_table CROSSES PAGE BOUNDARY!"
    ENDIF


;============================================================================
slave_light_half_table
    addwf   PCL, f

            ; +------- (not used, must be 0!)      
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'00010100'     ; Parking lights
    dt      b'00010100'     ; Low beam
    dt      b'00010100'     ; Fog lamps
    dt      b'00010100'     ; High beam
    dt      b'00000000'     ; Brake
    dt      b'00000000'     ; Reverse
    dt      b'00000000'     ; Indicator left
    dt      b'00000000'     ; Indicator right
    dt      b'00000000'     ; Hazard lights

    IF ((HIGH ($)) != (HIGH (slave_light_half_table)))
        ERROR "slave_light_half_table CROSSES PAGE BOUNDARY!"
    ENDIF

;============================================================================
local_setup_light_table
    addwf   PCL, f

            ; +------- OUT7     
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'00000000'     ; Centre
    dt      b'00000000'     ; Left
    dt      b'00000000'     ; Right

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "local_setup_light_table CROSSES PAGE BOUNDARY!"
    ENDIF


;============================================================================
slave_setup_light_table
    addwf   PCL, f

            ; +------- OUT7     
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'11000000'     ; Centre
    dt      b'01000000'     ; Left
    dt      b'10000000'     ; Right

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "slave_setup_light_table CROSSES PAGE BOUNDARY!"
    ENDIF



;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    clrf    light_data
    call    TLC5916_send
    return
    
    
;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    movf    setup_mode, f
    bnz     output_local_lights_setup

    movlw   1 << LIGHT_TABLE_LOCAL
    movwf   d0
    call    Output_get_state
    call    TLC5916_send
    return

output_local_lights_setup
    movlw   1 << LIGHT_TABLE_LOCAL_SETUP
    movwf   d0
    call    Output_get_setup_state
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
    clrf    light_data

    ; Parking lights
    btfss   light_mode, LIGHT_MODE_PARKING
    goto    output_local_get_state_low_beam
    movlw   0
    call    light_table
    iorwf   light_data, f

    ; Low beam
output_local_get_state_low_beam
    btfss   light_mode, LIGHT_MODE_LOW_BEAM
    goto    output_local_get_state_fog
    movlw   1
    call    light_table
    iorwf   light_data, f

    ; Fog lamps    
output_local_get_state_fog
    btfss   light_mode, LIGHT_MODE_FOG
    goto    output_local_get_state_high_beam
    movlw   2
    call    light_table
    iorwf   light_data, f

    ; High beam    
output_local_get_state_high_beam
    btfss   light_mode, LIGHT_MODE_HIGH_BEAM
    goto    output_local_get_state_brake
    movlw   3
    call    light_table
    iorwf   light_data, f

    ; Brake lights    
output_local_get_state_brake
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    output_local_get_state_reverse
    movlw   4
    call    light_table
    iorwf   light_data, f

    ; Reverse lights        
output_local_get_state_reverse
    btfss   drive_mode, DRIVE_MODE_REVERSE
    goto    output_local_get_state_indicator_left
    movlw   5
    call    light_table
    iorwf   light_data, f

    ; Indicator left    
output_local_get_state_indicator_left
    ; Skip all indicators and hazard lights if blink flag is in off period
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_local_get_state_end

    btfss   blink_mode, BLINK_MODE_INDICATOR_LEFT
    goto    output_local_get_state_indicator_right
    movlw   6
    call    light_table
    iorwf   light_data, f
    
    ; Indicator right
output_local_get_state_indicator_right
    btfss   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    goto    output_local_get_state_hazard
    movlw   7
    call    light_table
    iorwf   light_data, f
   
    ; Hazard lights 
output_local_get_state_hazard
    btfss   blink_mode, BLINK_MODE_HAZARD
    goto    output_local_get_state_end
    movlw   8
    call    light_table
    iorwf   light_data, f

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
    movwf   light_data
    return


;******************************************************************************
; light_table
;
; Retrieve a line from the light table.
; w: the line we request
; d0 indicates which light table we request:
;   0: local
;   1: slave
;   2: slave_half
;   4: local_setup
;   8: slave_setup
;
; Resulting light pattern is in w
;******************************************************************************
light_table
    btfsc   d0, LIGHT_TABLE_LOCAL
    goto    local_light_table
    btfsc   d0, LIGHT_TABLE_SLAVE
    goto    slave_light_table
    btfsc   d0, LIGHT_TABLE_SLAVE_HALF
    goto    slave_light_half_table
    btfsc   d0, LIGHT_TABLE_LOCAL_SETUP              
    goto    local_setup_light_table
    btfsc   d0, LIGHT_TABLE_SLAVE_SETUP               
    goto    slave_setup_light_table
    return
    

    END
