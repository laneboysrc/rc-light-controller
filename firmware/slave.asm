;******************************************************************************
;
;   rc-light-controller-slave.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller
    LIST        r=dec
    RADIX       dec

#define INCLUDE_CONFIG
    #include    hw.tmp


    GLOBAL servo


    ; Functions imported from utils.asm
    EXTERN TLC5916_send
    EXTERN UART_read_byte

     
    ; Functions and variables imported from steering_wheel_servo.asm
    EXTERN Init_steering_wheel_servo
    EXTERN Make_steering_wheel_servo_pulse     


#define SLAVE_MAGIC_BYTE    0x87

#define TMR0_PRESCALER 16           
#define PWM_BASE_TIME 2400      ; We need 2400 us or more to precisely output
                                ;  a servo pulse in the main loop
#define PWM_DUTY_CYCLE 20       ; Duty cycle of how long the half bright LEDs
                                ;  are on. Value in percent. 

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_slave_int UDATA_SHR   ; 16 Bytes that are accessible via any bank!

savew               res 1   ; Interrupt save registers
savestatus	        res 1
;savepclath          res 1
;savefsr             res 1


.data_slave UDATA           

servo_sync_flag     res 1
pwm_counter         res 1
int_temp            res 1
int_d0              res 1

uart_light_mode     res 1
uart_light_mode_half res 1
uart_servo          res 1

light_mode          res 1
light_mode_half     res 1
servo               res 1


;******************************************************************************
; Reset vector 
;******************************************************************************
.code_reset CODE    0x000           

    goto    Init


;******************************************************************************
; Interrupt vector
;******************************************************************************
.code_int   CODE     0x004   
        
Interrupt_handler
	movwf	savew           ; Save W register                               (1)
	movf	STATUS, w       ; W now has copy of status                      (1)
	movwf	savestatus	    ; Save status                                   (1)

;	movf	PCLATH, w       ; Save pclath
;	movwf	savepclath	
;	clrf	PCLATH		    ; Explicitly select Page 0
;	movf	FSR, w
;	movwf	savefsr		    ; Save FSR (just in case)

;	btfss	INTCON, T0IF
;	goto	int_clean   

	BANKSEL pwm_counter                                                    ;(1)
    comf    pwm_counter, f                                                 ;(1)
    btfss   pwm_counter, 0                                                 ;(1)
    goto    int_full_brightness                                            ;(2)


int_half_brightness
    ; Set timer 0 to a percentage of the period to achieve a PWM signal at
    ; a relatively high frequency. The original algorightm cause an annoying 
    ; flicker on camera.
    movlw   256 - (PWM_BASE_TIME / TMR0_PRESCALER * PWM_DUTY_CYCLE / 100)  ;(1)
    movwf   TMR0                                                           ;(1)
    movf    light_mode_half, w                                             ;(1)
    goto    int_output_lights                                              ;(2)


int_full_brightness
    clrf    servo_sync_flag                                                ;(1)
    movlw   256 - (PWM_BASE_TIME / TMR0_PRESCALER)                         ;(1)
    movwf   TMR0                                                           ;(1)
    movf    light_mode, w                                                  ;(1)
;   goto    int_output_lights

int_output_lights
    movwf   int_temp                                                       ;(1)
    movlw   8                                                              ;(1)
    movwf   int_d0                                                         ;(1)     16 cycles until here

int_tlc5916_send_loop
    rlf     int_temp, f                                                    ;(1)
    skpc                                                                   ;(2)  
    bcf     PORT_SDI                                                        
    skpnc                                                                  ;(1)
    bsf     PORT_SDI                                                       ;(1)
    bsf     PORT_CLK                                                       ;(1)
    bcf     PORT_CLK                                                       ;(1)
    decfsz  int_d0, f                                                      ;(1)
    goto    int_tlc5916_send_loop                                          ;(2)     10 cycles * 8

    bsf     PORT_LE                                                        ;(1) 
    bcf     PORT_LE                                                        ;(1) 
    bcf     PORT_OE                                                        ;(1) 

int_lights_done

int_t0if_done
	bcf	    INTCON, T0IF    ; Clear interrupt flag that caused interrupt   ;(1) 

int_clean
;		movf	savefsr, w
;		movwf	FSR		    ; Restore FSR
;		movf	savepclath, w
;		movwf	PCLATH      ; Restore PCLATH (Page=original)
		movf	savestatus, w                                              ;(1) 
		movwf	STATUS      ; Restore status! (bank=original)              ;(1) 
		swapf	savew, f    ; Restore W from *original* bank!              ;(1)
		swapf	savew, w    ; Swapf does not affect any flags!             ;(1)
		retfie                                                             ;(5)     13 cycles

                                                                           ; TOTAL: 110 cycles = 110 us


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

    call    Init_steering_wheel_servo

    BANKSEL INTCON
    bcf     INTCON, T0IF    ; Clear Timer0 Interrupt Flag    
    bcf     PIR1, CCP1IF    ; Clear Timer1 Compare Interrupt Flag

	bsf	    INTCON, T0IE    ; Enable Timer0 interrupt
	bsf	    INTCON, GIE     ; Enable interrupts

;   goto    Main_loop    


;******************************************************************************
; Main program
;******************************************************************************
Main_loop
    call    Read_UART
    call    Set_light_mode

    BANKSEL servo
    movfw   servo
    call    Make_steering_wheel_servo_pulse   

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

read_UART_byte_2
    call    UART_read_byte
    BANKSEL uart_light_mode
    movwf   uart_light_mode         ; Store 2nd byte
    sublw   SLAVE_MAGIC_BYTE        ; Is it the magic byte?
    bz      read_UART_byte_2        ; Yes: we must be out of sync...

read_UART_byte_3
    call    UART_read_byte
    BANKSEL uart_light_mode_half
    movwf   uart_light_mode_half
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2

read_UART_byte_4
    call    UART_read_byte
    BANKSEL uart_servo
    movwf   uart_servo
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2
    return


;******************************************************************************
Set_light_mode
    BANKSEL uart_light_mode
    movf    uart_light_mode, w
    movwf   light_mode
    movf    uart_light_mode_half, w
    iorwf   light_mode, w
    movwf   light_mode_half
    return    


;******************************************************************************
Process_steering_wheel_servo
IFDEF ENABLE_SERVO_OUTPUT    
    ; Synchronize with the interrupt to ensure the servo pulse is not
    ; interrupted and stays precise (i.e. no servo chatter)
    BANKSEL servo_sync_flag
    bsf     servo_sync_flag, 0
    btfsc   servo_sync_flag, 0
    goto    $ - 1

    movf    uart_servo, w
    movwf   servo    
    
    call    Make_steering_wheel_servo_pulse
ENDIF
    return

    END


;******************************************************************************
; Timing architecture:
; ====================
; Timer0 will be used to provide the timing for the LED PWM. To ensure that
; there is no visible flicker on cameras, we want to keep the PWM frequency
; as high as possible. 
;
; However, to ensure that we output a precise pulse for the servo we must not 
; have the interrupt interfere with the pulse generation. Ideally we would use 
; the CCP1/RB3 pin to let the timer hardware pull the pin low, but the pin is 
; tied up in the hardware design.
;
; The servo pulse we generate has a worst-case of 2220 us. The interrupt 
; duration is approx 110 us. Therefore we need 2400 us between two interrupts
; (including a bit of safety margin).
;
; By setting the Timer0 prescaler to 16 we can achieve 4096 us. To achieve
; 2400 us we need to preload the Timer0 with a value of 
;
;     256 - (2400 / 16)  = 106
;
; To achieve a 20% PWM, we need the short Timer0 period to be 
;
;     2400 * (20 / 100)  = 480 us
;
; which translates in a preload value of 
;
;     256 - (480 / 16)   = 226
;
; With these figures the PWM frequency is ~350 Hz.
;
; The servo pulse is generated using Timer1 in "Compare mode, generate software 
; interrupt on match" mode. The servo pulse is sent after each UART command.
; Since the master sends the UART based on reading the RC receiver, the repeat 
; timing should relatively match the normal 20 ms interval between pulses.
;******************************************************************************

