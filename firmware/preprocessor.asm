;******************************************************************************
;
;   preprocessor.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - Pre-processor
    LIST        r=dec
    RADIX       dec

#define INCLUDE_CONFIG
    #include    hw.tmp


    GLOBAL startup_mode


    ; Functions and variables imported from utils.asm
    EXTERN UART_send_w


    ; Functions and variables imported from *_reader.asm
    EXTERN Read_all_channels
    EXTERN Init_reader
    
    EXTERN steering            
    EXTERN steering_abs       
    EXTERN throttle            
    EXTERN throttle_abs       
    EXTERN ch3                 


; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

#define SLAVE_MAGIC_BYTE    0x87

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_preprocessor UDATA

startup_mode        res 1


;******************************************************************************
; Reset vector 
;******************************************************************************
.code_preprocessor_reset CODE 0x000           
    goto    Init


;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_preprocessor CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init
    ;-----------------------------
    ; Initialise the chip (macro included from hw_*.tmp)
    IO_INIT_PREPROCESSOR

    call Init_reader

;   goto    Main_loop    


;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_all_channels
    call    Output_preprocessed_channels
    goto    Main_loop


;******************************************************************************
; Output_preprocessed_channels
;
;******************************************************************************
Output_preprocessed_channels
    movlw   SLAVE_MAGIC_BYTE    ; Magic byte for synchronization
    call    UART_send_w        
    
    movf    steering, w
    call    UART_send_w        
    
    movf    throttle, w
    call    UART_send_w        

    ; Mask out other bits used in ch3 for toggle and intialization
    ; Add startup mode to the transmitted byte. Since we are using bits 4, 5 
    ; of startup_mode only this works fine.
    movf    ch3, w                       
    andlw   0x01                        
    iorwf   startup_mode, w              
    call    UART_send_w        
    
    return


    END     ; Directive 'end of program'



