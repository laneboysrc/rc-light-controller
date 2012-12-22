    TITLE       Light tables for the Tamiya XR311
    RADIX       dec

    #include    <p16f628a.inc>

    GLOBAL local_light_table
    GLOBAL local_light_half_table
    GLOBAL slave_light_table
    GLOBAL slave_light_half_table
    GLOBAL local_setup_light_table
    GLOBAL slave_setup_light_table

    ORG 0x010

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
local_light_half_table
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


    END
