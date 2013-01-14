    TITLE       Light tables for the Tamiya XR311
    RADIX       dec

    #include    io_master.tmp


    GLOBAL local_light_table
    GLOBAL local_light_half_table
    GLOBAL local_light_blink_table
    GLOBAL slave_light_table
    GLOBAL slave_light_half_table
    GLOBAL local_setup_light_table
    GLOBAL slave_setup_light_table

    ORG 0x010


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


    END
