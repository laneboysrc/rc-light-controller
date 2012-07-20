    TITLE       Light tables for the Axial SCX10 Dingo
    RADIX       dec

    #include    <p16f628a.inc>

    GLOBAL local_light_table
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
    dt      b'00000001'     ; Stand lights
    dt      b'00000011'     ; Head lights
    dt      b'00000111'     ; Fog lights
    dt      b'00001111'     ; High beam
    dt      b'00010000'     ; Brake lights
    dt      b'00100000'     ; Reverse lights
    dt      b'10000000'     ; Indicator left
    dt      b'01000000'     ; Indicator right
    dt      b'11000000'     ; Hazard lights

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
    dt      b'00000001'     ; Stand lights
    dt      b'00000011'     ; Head lights
    dt      b'00000111'     ; Fog lights
    dt      b'00001111'     ; High beam
    dt      b'00010000'     ; Brake lights
    dt      b'00000000'     ; Reverse lights
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
    dt      b'00010000'     ; Stand lights
    dt      b'00010000'     ; Head lights
    dt      b'00010000'     ; Fog lights
    dt      b'00010000'     ; High beam
    dt      b'00000000'     ; Brake lights
    dt      b'00000000'     ; Reverse lights
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
    dt      b'11000000'     ; Centre
    dt      b'10000000'     ; Left
    dt      b'01000000'     ; Right

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
    dt      b'10000000'     ; Left
    dt      b'01000000'     ; Right

    IF ((HIGH ($)) != (HIGH (local_light_table)))
        ERROR "slave_setup_light_table CROSSES PAGE BOUNDARY!"
    ENDIF


    END
