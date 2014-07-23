;******************************************************************************
;
;   lights-test-ws2812-12f1840.asm
;
;   This file contains test code to test the LED outputs on the
;   PIC12F1840 and WS2812 based hardware running at 32 MHz.
;
;   The code never allows the main loop to be reached as it creates an 
;   endless loop during initialization.
;
;   It first switches each light on individually for 6 times with decreasing
;   brighness, then switches all lights fully on for 5 seconds (good for power 
;   consumption measurement), then all lights from lowest to full brightness
;   in 6 steps; each step having 1 second.
;   1 second, and then repeats.
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light output for hardware test
    RADIX       dec

    #include    hw.tmp

    GLOBAL Init_lights
    GLOBAL Output_lights
    
    ; Functions and variables imported from utils.asm
    EXTERN Clear_light_data
    EXTERN Fill_light_data
    EXTERN xl
    EXTERN temp
    EXTERN light_data

    ; Functions and variables imported from ws2812.asm
    EXTERN Init_WS2812   
    EXTERN WS2812_send
    

#define VAL_STEP1 105
#define VAL_STEP2 52
#define VAL_STEP3 26
#define VAL_STEP4 13
#define VAL_STEP5 7
#define VAL_STEP6 1

#define VAL_FULL 255


    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA
d1 res 1
d2 res 1
d3 res 1
cr          res 1
cg          res 1
cb          res 1



;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    call    Init_WS2812
    
_init_loop
    BANKSEL cr
    movlw   VAL_STEP1
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP1
    movwf   cr
    clrf    cg
    clrf    cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP1
    clrf    cr
    movwf   cg
    clrf    cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP1
    clrf    cr
    clrf    cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP2
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP3
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP4
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP5
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    BANKSEL cr
    movlw   VAL_STEP6
    movwf   cr
    movwf   cg
    movwf   cb
    call    _sequence_lights

    movlw   VAL_FULL
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s
    call    Delay_1s
    call    Delay_1s
    call    Delay_1s
    call    Delay_1s

    BANKSEL cr
    movlw   VAL_FULL
    movwf   cr
    clrf    cg
    clrf    cb
    call    _fill_rgb
    call    WS2812_send
    call    Delay_1s

    BANKSEL cr
    movlw   VAL_FULL
    clrf    cr
    movwf   cg
    clrf    cb
    call    _fill_rgb
    call    WS2812_send
    call    Delay_1s

    BANKSEL cr
    movlw   VAL_FULL
    clrf    cr
    clrf    cg
    movwf   cb
    call    _fill_rgb
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP6
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP5
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP4
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP3
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP2
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    movlw   VAL_STEP1
    call    Fill_light_data
    call    WS2812_send
    call    Delay_1s

    call    Clear_light_data
    call    WS2812_send
    call    Delay_1s
    
    goto    _init_loop


_sequence_lights
    movlw   HIGH light_data
    movwf   FSR1H
    movlw   LOW light_data
    movwf   FSR1L

    movlw   NUMBER_OF_LEDS
    movwf   xl
    
_sequence_lights_loop
    call    Clear_light_data
    BANKSEL cr
    movfw   cr
    movwi   FSR1++ 
    movfw   cg
    movwi   FSR1++ 
    movfw   cb
    movwi   FSR1++ 
    call    WS2812_send
    call    Delay_100ms
    decfsz  xl, f
    goto    _sequence_lights_loop
    return

_fill_rgb
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
    movlw   NUMBER_OF_LEDS
    movwf   temp
_fill_rgb_loop
    movfw   cr
    movwi   FSR0++    
    movfw   cg
    movwi   FSR0++    
    movfw   cb
    movwi   FSR0++    
    decfsz  temp, f
    goto    _fill_rgb_loop
    return

;******************************************************************************
; Generated by http://www.piclist.com/cgi-bin/delay.exe (December 7, 2005 version)
;******************************************************************************
Delay_100ms
	movlw	0x6D
	movwf	d1
	movlw	0xBF
	movwf	d2
	movlw	0x02
	movwf	d3
Delay_100ms_0
	decfsz	d1, f
	goto	$+2
	decfsz	d2, f
	goto	$+2
	decfsz	d3, f
	goto	Delay_100ms_0
    return


;******************************************************************************
; Generated by http://www.piclist.com/cgi-bin/delay.exe (December 7, 2005 version)
;******************************************************************************
Delay_1s
	movlw	0x47
	movwf	d1
	movlw	0x71
	movwf	d2
	movlw	0x12
	movwf	d3
Delay_1s_0
	decfsz	d1, f
	goto	$+2
	decfsz	d2, f
	goto	$+2
	decfsz	d3, f
	goto	Delay_1s_0

	goto	$+1
	goto	$+1
	goto	$+1
	return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    return              ; Never called, just here to get things compiling.


    END
