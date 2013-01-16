    TITLE       Light tables for the Tamiya XR311
    RADIX       dec

    #include    hw.tmp
    
    
    GLOBAL Init_local_lights
    GLOBAL Output_local_lights
    GLOBAL Output_slave

    
    EXTERN TLC5916_send
    EXTERN light_data
    
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

  

#define LIGHT_TABLE_LOCAL 0
#define LIGHT_TABLE_LOCAL_HALF 1
#define LIGHT_TABLE_LOCAL_BLINK 2
#define LIGHT_TABLE_SLAVE 3
#define LIGHT_TABLE_SLAVE_HALF 4
#define LIGHT_TABLE_LOCAL_SETUP 5
#define LIGHT_TABLE_SLAVE_SETUP 6
    
    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA

d0                  res 1
temp                res 1



;============================================================================
.light_table CODE    0x010 
;============================================================================


;============================================================================
local_light_table
    addwf   PCL, f

            ; +------- OUT7 Low beam right
            ; |+------ OUT6 Low beam left
            ; ||+----- OUT5 Reversing lights
            ; |||+---- OUT4 Parking lights
            ; ||||+--- OUT3 Tail/Brake/Indicator left
            ; |||||+-- OUT2 Tail/Brake/Indicator right
            ; ||||||+- OUT1 Indicator left
            ; |||||||+ OUT0 Indicator right
    dt      b'00010000'     ; Parking lights
    dt      b'11000000'     ; Low beam
    dt      b'00000000'     ; Fog lamps
    dt      b'00000000'     ; High beam
    dt      b'00001100'     ; Brake
    dt      b'00100000'     ; Reverse

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "local_light_table CROSSES PAGE BOUNDARY!"
    ENDIF

    
;============================================================================
local_light_half_table
    addwf   PCL, f

            ; +------- OUT7 Low beam right
            ; |+------ OUT6 Low beam left
            ; ||+----- OUT5 Reversing lights
            ; |||+---- OUT4 Parking lights
            ; ||||+--- OUT3 Tail/Brake/Indicator left
            ; |||||+-- OUT2 Tail/Brake/Indicator right
            ; ||||||+- OUT1 Indicator left
            ; |||||||+ OUT0 Indicator right
    dt      b'00001100'     ; Parking lights
    dt      b'00000000'     ; Low beam
    dt      b'00000000'     ; Fog lamps
    dt      b'00000000'     ; High beam
    dt      b'00000000'     ; Brake
    dt      b'00000000'     ; Reverse

    IF ((HIGH ($)) != (HIGH (local_light_half_table)))
        ERROR "local_light_half_table CROSSES PAGE BOUNDARY!"
    ENDIF


;============================================================================
local_light_blink_table
    addwf   PCL, f

            ; +------- OUT7 Low beam right
            ; |+------ OUT6 Low beam left
            ; ||+----- OUT5 Reversing lights
            ; |||+---- OUT4 Parking lights
            ; ||||+--- OUT3 Tail/Brake/Indicator left
            ; |||||+-- OUT2 Tail/Brake/Indicator right
            ; ||||||+- OUT1 Indicator left
            ; |||||||+ OUT0 Indicator right
    dt      b'00001010'     ; Indicator left
    dt      b'00000101'     ; Indicator right
    dt      b'00001111'     ; Hazard lights

    IF ((HIGH ($)) != (HIGH (local_light_blink_table)))
        ERROR "local_light_blink_table CROSSES PAGE BOUNDARY!"
    ENDIF
    

;============================================================================
slave_light_table
    addwf   PCL, f

            ; +------- (not used, must be 0!)     
            ; |+------ OUT6 
            ; ||+----- OUT5 
            ; |||+---- OUT4
            ; ||||+--- OUT3
            ; |||||+-- OUT2
            ; ||||||+- OUT1
            ; |||||||+ OUT0
    dt      b'00000000'     ; Parking lights
    dt      b'00000000'     ; Low beam
    dt      b'00000000'     ; Fog lamps
    dt      b'00000000'     ; High beam
    dt      b'00000000'     ; Brake
    dt      b'00000000'     ; Reverse
    dt      b'00000000'     ; Indicator left
    dt      b'00000000'     ; Indicator right
    dt      b'00000000'     ; Hazard lights

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
    dt      b'00000000'     ; Parking lights
    dt      b'00000000'     ; Low beam
    dt      b'00000000'     ; Fog lamps
    dt      b'00000000'     ; High beam
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
    dt      b'00000001'     ; Centre
    dt      b'00000010'     ; Left
    dt      b'00000100'     ; Right

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
    dt      b'00000000'     ; Centre
    dt      b'00000000'     ; Left
    dt      b'00000000'     ; Right

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "slave_setup_light_table CROSSES PAGE BOUNDARY!"
    ENDIF


;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_local_lights
;******************************************************************************
Init_local_lights
    clrf    light_data
    clrf    light_data+1
    comf    light_data+1, f
    call    TLC5916_send
    return


;******************************************************************************
; Output_slave
;******************************************************************************
Output_slave
    return

    
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

    movf    light_data, w         
    movwf   light_data+1

    movlw   1 << LIGHT_TABLE_LOCAL_HALF
    movwf   d0
    call    Output_get_state

    movlw   1 << LIGHT_TABLE_LOCAL_BLINK
    movwf   d0
    call    Output_get_blink_state

    ; Special processing for all indicators and hazard lights if blink flag is 
    ; in off period
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_local_lights_blink_off    

    movfw   temp
    iorwf   light_data+1, f
    goto    output_local_lights_end    

output_local_lights_blink_off 
    ; To achieve US style combined indicators/tail/brake lights we have
    ; to do trickery to blink them properly
    movfw   light_data+1
    iorwf   light_data, w
    andwf   temp, f

    movfw   temp
    iorwf   light_data, f
    comf    temp, w
    andwf   light_data+1, f

output_local_lights_end    
    ; Remove half bright LEDs that are also fully lit to avoid more than
    ; 20mA going into the LEDs as the two TLC5916 are in parallel now.    
    comf    light_data+1, w
    andwf   light_data, f  

    call    TLC5916_send
    return

output_local_lights_setup
    movlw   1 << LIGHT_TABLE_LOCAL_SETUP
    movwf   d0
    call    Output_get_setup_state
    movfw   temp
    movwf   light_data+1
    clrf    light_data
    call    TLC5916_send
    return    

output_local_startup
    swapf   startup_mode, w     ; Move bits 4..7 to 0..3
    andlw   0x07                ; Mask out bits 0..2
    movwf   light_data+1
    clrf    light_data
    call    TLC5916_send
    return    
    
    
;******************************************************************************
; Output_get_state
;
; d0 contains the light table index to process.
; Resulting lights are stored in light_data.
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
    goto    output_local_get_state_end
    movlw   5
    call    light_table
    iorwf   light_data, f

output_local_get_state_end
    return


;******************************************************************************
; Output_get_blink_state
;
; d0 contains the light table index to process.
; Resulting lights are stored in temp.
;******************************************************************************
Output_get_blink_state
    clrf    temp
    
    ; Indicator left    
output_local_get_state_indicator_left
    btfss   blink_mode, BLINK_MODE_INDICATOR_LEFT
    goto    output_local_get_state_indicator_right
    movlw   0
    call    light_table
    iorwf   temp, f
    
    ; Indicator right
output_local_get_state_indicator_right
    btfss   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    goto    output_local_get_state_hazard
    movlw   1
    call    light_table
    iorwf   temp, f
   
    ; Hazard lights 
output_local_get_state_hazard
    btfss   blink_mode, BLINK_MODE_HAZARD
    goto    output_local_get_state_end
    movlw   2
    call    light_table
    iorwf   temp, f

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
;   2:  local_blink
;   4:  slave
;   8:  slave_half
;   16: local_setup
;   32: slave_setup
;
; Resulting light pattern is in w
;******************************************************************************
light_table
    btfsc   d0, LIGHT_TABLE_LOCAL
    goto    local_light_table
    btfsc   d0, LIGHT_TABLE_LOCAL_HALF
    goto    local_light_half_table
    btfsc   d0, LIGHT_TABLE_LOCAL_BLINK
    goto    local_light_blink_table
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
