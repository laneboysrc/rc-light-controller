;******************************************************************************
;
;   slave-tlc5940.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller slave driving TLC5940
    LIST        r=dec
    RADIX       dec

#define INCLUDE_CONFIG
    #include    hw.tmp

    ; Functions imported from utils.asm
    EXTERN Init_TLC5940    
    EXTERN TLC5940_send
    EXTERN UART_read_byte
    
    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh
    EXTERN temp
    EXTERN light_data


#define SLAVE_MAGIC_BYTE 0x87

IFNDEF ENABLE_UART
    ERROR "########################"
    ERROR "To use the slave you must add '-D ENABLE_UART' to the CFLAGS in the makefile!"
    ERROR "########################"
ENDIF


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_slave UDATA           
count   RES     1

;******************************************************************************
; Reset vector 
;******************************************************************************
.code_reset CODE    0x000           

    goto    Init


;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_slave CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init
    ;-----------------------------
    ; Initialise the chip (macro included from hw_*.tmp)
    IO_INIT_SLAVE

    call    Clear_light_data
;   goto    Main_loop    


;******************************************************************************
; Main program
;******************************************************************************
Main_loop
    call    Read_UART
    call    TLC5940_send    

    goto    Main_loop


;******************************************************************************
; Read_UART
;
; This function returns after having successfully received a complete
; protocol frame via the UART.
;******************************************************************************
Read_UART
    call    UART_read_byte
    sublw   SLAVE_MAGIC_BYTE        ; First byte the magic byte?
    bnz     Read_UART               ; No: wait for 0x8f to appear

read_UART_synchronized
    BANKSEL count
    movlw   16
    movwf   count
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L

read_UART_byte
    call    UART_read_byte          ; Read a byte
    movwi   FSR0++                  ; Store it in light_data[FSR0++]

    sublw   SLAVE_MAGIC_BYTE        ; Is it the magic byte?
    bz      read_UART_synchronized  ; Yes: we must be out of sync...

    BANKSEL count
    decfsz  count, f                ; All bytes done?
    goto    read_UART_byte

    return


;******************************************************************************
; Clear_light_data
;
; Clear all light_data variables, i.e. by default all lights are off.
;******************************************************************************
Clear_light_data
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
    movlw   16          ; There are 16 bytes in light_data
    movwf   temp
    clrw   
clear_light_data_loop
    movwi   FSR0++    
    decfsz  temp, f
    goto    clear_light_data_loop
    return


    END
