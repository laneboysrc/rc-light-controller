;******************************************************************************
;
;   uils.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Utility functions
    LIST        r=dec
    RADIX       dec

    #include    io.tmp


    GLOBAL  xl
    GLOBAL  xh
    GLOBAL  yl
    GLOBAL  yh
    GLOBAL  zl
    GLOBAL  zh

;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_utils UDATA

xl                  res 1
xh                  res 1
yl                  res 1
yh                  res 1
zl                  res 1
zh                  res 1

d0                  res 1
d1                  res 1
temp                res 1
 

;******************************************************************************
; Relocatable code section
;******************************************************************************
.utils CODE


;******************************************************************************
; Max
;  
; Given two 8-bit values in temp and w, returns the larger one in w
;******************************************************************************
.utils_Max CODE
    GLOBAL Max
Max
    subwf   temp, w
    skpc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min
;  
; Given two 8-bit values in temp and w, returns the smaller one in w
;******************************************************************************
.utils_Min CODE
    GLOBAL Min
Min
    subwf   temp, w
    skpnc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the smaller one in zl/zh.
;******************************************************************************
.utils_Min_x_z CODE
    GLOBAL Min_x_z
Min_x_z
    movf    xl, w
    subwf	zl, w	
    movf	xh, w
    skpc
    addlw   1
    subwf	zh, w
    andlw	b'10000000'	
    skpz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Max_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the larger one in zl/zh.
;******************************************************************************
.utils_Max_x_z CODE
    GLOBAL Max_x_z
Max_x_z
    movf    xl, w
    subwf   zl, w		
    movf    xh, w
    skpc
    addlw   1
    subwf   zh, w		
    andlw   b'10000000'  
    skpnz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Div_x_by_y
;
; xh/xl = xh/xl / yh/yl; Remainder in zh/zl
;
; Based on "32 by 16 Divison" by Nikolai Golovchenko
; http://www.piclist.com/techref/microchip/math/div/div16or32by16to16.htm
;******************************************************************************
.utils_Div_x_by_y CODE
    GLOBAL Div_x_by_y
Div_x_by_y
    clrf    zl      ; Clear remainder
    clrf    zh
    clrf    temp    ; Clear remainder extension
    movlw   16
    movwf   d0
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
    decfsz  d0, f
    goto    div16by16loop

; Shift in last bit
    rlf     xl, f
    rlf     xh, f
    return


;******************************************************************************
; Mul_xl_by_w
;
; Calculates xh/xl = xl * w
;******************************************************************************
.utils_Mul_xl_by_w CODE
    GLOBAL Mul_xl_by_w
Mul_xl_by_w
    clrf    xh
	clrf    d0
    bsf     d0, 3
    rrf     xl, f

mul_xl_by_w_loop
	skpnc
	addwf   xh, f
    rrf     xh, f
    rrf     xl, f
	decfsz  d0, f
    goto    mul_xl_by_w_loop
    return


;******************************************************************************
; Mul_x_by_100
;
; Calculates xh/xl = xh/xl * 100
; Only valid for xh/xl <= 655 as the output is only 16 bits
;******************************************************************************
.utils_Mul_x_by_100 CODE
    GLOBAL Mul_x_by_100
Mul_x_by_100
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
.utils_Div_x_by_4 CODE
    GLOBAL Div_x_by_4
Div_x_by_4
	clrc
	rrf     xh, f
	rrf	    xl, f
	clrc
	rrf     xh, f
	rrf	    xl, f
    return


;******************************************************************************
; Add_y_to_x
;
; This function calculates xh/xl = xh/xl + yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
.utils_Add_y_to_x CODE
    GLOBAL Add_y_to_x
Add_y_to_x
    movf    yl, w
    addwf   xl, f
    movf    yh, w
    skpnc
    incf    yh, W
    addwf   xh, f
    return         


;******************************************************************************
; Sub_y_from_x
;
; This function calculates xh/xl = xh/xl - yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
.utils_Sub_y_from_x CODE
    GLOBAL Sub_y_from_x
Sub_y_from_x
    movf    yl, w
    subwf   xl, f
    movf    yh, w
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
.utils_If_y_lt_x CODE
    GLOBAL If_y_lt_x
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
.utils_If_x_eq_y CODE
    GLOBAL If_x_eq_y
If_x_eq_y
    movfw   xl
    subwf   yl, w
    skpz
    return
    movfw   xh
    subwf   yh, w
    return


;******************************************************************************
; Mul_x_by_6
;
; Calculates xh/xl = xl * 6
;
; Generated by www.piclist.com/cgi-bin/constdivmul.exe (1-May-2002 version)
;******************************************************************************
.utils_Mul_x_by_6 CODE
    GLOBAL Mul_x_by_6
Mul_x_by_6
    ; Shift accumulator left 1 times: xh/xl = xl * 2
	clrc
	rlf	    xl, f
	clrf	xh
	rlf	    xh, f

    ; Copy accumulator to temporary location
	movf	xh, w
	movwf	yh
	movf	xl, w
	movwf	yl

    ; Shift temporary value left 1 times: yh/yl = xl * 4
	clrc
	rlf	    yl, f
	rlf	    yh, f

    ; xh/xl  =  xh/xl + yh/yl  =  xl * 6
	movf	yl, w
	addwf	xl, f
	movf	yh, w
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


;******************************************************************************
; Add_x_and_780
;
; Calculates xh/xl = xh/xl + 780
;******************************************************************************
.utils_Add_x_and_780 CODE
    GLOBAL Add_x_and_780
Add_x_and_780
	movlw	LOW(780)
	addwf	xl, f
	movlw	HIGH(780)
    movwf   yh
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


    END     ; Directive 'end of program'



