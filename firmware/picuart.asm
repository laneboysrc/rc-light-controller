;                       PICUART.ASM 
;                 Microchip MPASM format
;
;                     version 1.04.1
;               Last modified July 24, 2000
;
;                  A Program and Document
;       designed to help others learn how to use the
;                 built-in UART features of
;              the PIC microcontroller family.
;
;The program has been verified on MANY PIC microcontrollers.
;No one has reported any PIC with built-in UART which the
;program does not work with so far.
; 
;                      Written by
;                  Fr. Thomas McGahee
;               tom_mcgahee@sigmais.com
;       primary web site http://redrival.com/mcgahee
;     secondary website http://mcgahee.freeservers.com
;
;     ************************************************
;     *  My thanks to Dwayne Reid and Tony Kubek     *
;     *  and Tony Nixon and all others who responded *
;     *  with suggestions and bug reports!           *
;     ************************************************
;
; note for MPLAB users:
;    set assembler to case-insensitive, 
;    except within strings using /c- option.
;    default radix should be set for decimal.
;    tabs are set for 8 spaces.
;    ignore any warnings about the bank being used.
;
;revision 1.02
;added a dummy interrupt routine
;changed position of clearing of ports
;adjusted a few comments
;
;revision 1.02.1
;saved/restored PCLATH within interrupt.

;revision 1.03
;cleaned up interrupt related sections.
;addressed some issues of bank/page when dealing with interrupts.
;
;revision 1.03.1  April 19, 2000
;added revision level and revison date info to top of document.
;cleaned up lines to keep then shorter.
;
;revision 1.04 July 14, 2000
;added note about program working with all PICs
;added comments for loop check for GIE
;added note from Tony Nixon about TRMT check
;
;revision 1.04.1 July 24, 2000
;added comment about startup delay / MAX232 startup time


;******************************************************************************
;******************************************************************************

; The purpose of this program is to outline all the steps needed to
; use the built-in UART of the PIC16C74A and other PICs.
;
; When I first tried to implement the UART features for the 
; PIC16C74A I discovered that what I thought should have been a 
; simple and straight-forward task was made difficult because:
;
; 1) The information in my PIC manual was incomplete.
; 2) Some of the information in the manual was wrong.
; 3) Although there was SOME errata information on the MicroChip 
;    site, there was no simple guide to setting up the UART.
; 4) Bits and pieces to the puzzle were strewn here and there in 
;    the manual and in posts to the PIC list, but not in one place.
; 5) Microchip did not provide a comprehensive example for using 
;    the UART.
; 6) Information provided by other users of the PIC16C74A device 
;    in several cases turned out to be erroneous.
; 7) Some simplistic solutions offered by others did not provide 
;    any kind of recovery from error conditions such as framing 
;    errors and over-run errors. These "examples" would work for 
;    a few minutes and then hang.
; 8) Many solutions ignored the hardware interface aspects
;    altogether. The simplistic solutions had:
;    A) No mention of the signal inversions and rs232 translations.
;    B) No mention of handshaking required at the PC end of things.
; 9) There are a number of issues regarding the use of interrupts
;    that can really sneak up on you and catch you by suprise. It is
;    always wonderful to have a program that is working beautifully,
;    and then you add a few extra innocent lines to your program, and
;    suddenly you have all kinds of errors. The issue of handling
;    bank and page select from within the interrupt routine is
;    often not fully handled. (I initially fell into this trap.
;    everything worked fine until I started using code in different
;    pages.)

;******************************************************************************
;******************************************************************************

; This assembly file seeks to address as many of these issues as possible.

; Firstly, this is a file that you can assemble under MPLAB and use to
; check out the UART of the PIC16C74.

; just assemble this code exactly as-is. Wire up your MAX232 or other
; form of signal inversion (if talking to a PC or other RS232 device.)
; If you get an echo out of everything you put in, then it means
; your hardware attached to the UART is OK. If not, at least then
; you know that your problem is with the hardware. Check the section
; below that deals with the need for signal inversion, and also
; check the section on cable wiring.

; Secondly, you can make a copy of this file, strip out all the comments
; and have a stripped-down version that you can use as the starting
; point for many PIC projects.

; Thirdly, you can actually read the whole thing and learn in a few 
; minutes what I and others had to agonize over for a fair amount of time.

; There are topics covered here that go beyond just using the UART.
; For example, the section on interrupts points out some interesting
; problems related to page and bank control.

;If you find anything in error in this file, or if you have any comments
;to make or any additions, clarifications, or other constructive
;criticism to make, please do so. I will try to keep an updated
;version of this document/program on my website. See the top of the
;assembly file for my e-mail address and website url. I look forward
;to hearing from all of those who take the time to look over this
;document. I hope you find it useful.

;If you find any PICS that do NOT work with this software,
;please inform me so that I can mention that at the top of the
;document. In this way we can all help one another so that we
;all don't have to go around "re-inventing the wheel"

; Permission is hereby granted for anyone who wants to:
; To use this file as part of their own program.
; To post this file on their website or use it as a FAQ.
; To modify this file in any way they want.
;
;All I ask is that when distributing this file to others
;you include my name, e-mail address, and webpage url.
;the purpose is to make sure people can contact me if they
;have any corrections or additions to make.

;******************************************************************************
;******************************************************************************

; The assmbly code is for the PIC16C74A / PIC16C74B
; If using a different microcontroller, then you will have to
; adjust certain parameters, such as
;	list / include / __config

	list		p=16c74	;this directive must come first
					;Set it for the kind of PIC you are using!

	include	<p16c74.inc>
					;Set this for the kind of PIC you are using!

        list



;we have to set the configuration bits
;__config a & b & c 
;_rc_osc, _xt_osc, _hs_osc, _lp_osc oscillator type
;_wdt_on, _wdt_off watchdog timer
;_cp_on, _cp_off code protect
;_pwrte_on, _pwrte_off power up timer enable

	__config	_hs_osc & _wdt_off & _pwrte_on & _cp_off

					;!!! configure pic as desired...
					;These may vary with different
					;kinds of PICs.

;******************************************************************************
;******************************************************************************          

;************************
;define ram useage	*
;************************

;h'20' is where bank 0 general purpose sram begins. 
;     ends at h'7f'. room for 96 bytes.
;h'a0' is where bank 1 general purpose sram begins. 
;     ends at h'ff'. room for 96 bytes.
;
;so sram available for variable storage is 192 bytes.

;we use cblock statements to define variable space in sram.

;ram data storage declarations. there are two banks, banks 0 and 1. we will
;use primarily bank 0.

		cblock	h'20'	;bank 0  h'20' to h'7f'. 96 locations

		;these allow us to save the context during interrupts.
savew1	;SAVEW1 *MUST* be at location h'20'!
savestatus	
savepclath
savefsr

rx_data		;the received byte from the serial UART
tx_data		;byte to be transmitted via UART

		;it is a good idea to have assigned variables for
		;holding the UART data, even though MOST of the
		;time it is handy to use the W register for transferring
		;this data back and forth.

		endc

;******************************************************************************

;If your code at *any* time uses any of the other banks, then you *MUST* 
;allow for the fact that an interrupt may occur while one of these other 
;banks is active. It is absolutely IMPERATIVE that the FIRST item declared 
;in each bank of system variable RAM is a variable that will ALWAYS contain 
;the saved content of W during an interrupt. In my case I call this 
;variable SAVEW1. Since MPLAB frowns on using the same exact name to specify 
;two separate RAM locations, I name the 1st variable in each bank as SAVEW1, 
;SAVEW2, SAVEW3, etc. Repeat for as many pages as your PIC device actually 
;has.
;
;See inthandler routine for further details.

		cblock	h'A0' ;bank 1

savew2		;SAVEW2 *MUST* be at location h'A0'.
		;Now if a MOVWF SAVEW1 is executed in bank 1,
		;then W gets saved in SAVEW2.

		endc

;NOTE: the PIC16C74 only has two banks. If your PIC has more than
;two banks, then set up a SAVEW3... SAVEW4... as the FIRST variable
;in each additional bank.
;******************************************************************************
;******************************************************************************

;******************************************************************************
; WE BEGIN THE PROGRAM SECTION
;******************************************************************************

;
;program  4kx14 eeprom. (max 0fffh)
		
	org	h'0000'		;set code origin to beginning of rom

start
	goto	initialize	;we must get past interrupt vector at 0004



		
;************************
;INTERRUPT ROUTINE	*
;************************

; for this example we will be using polling instead of interrupts.
; the two "interrupt" flags we need to look at in polling are:

; pir1,rcif which is pir1,5. this is the ReCeiver_Interrupt_Flag.
; 1=receive data available  0=no receive data avaliable.

; pir1,txif which is pir1,4. this is the Transmitter_Interrupt_Flag.
; 1=OK to load data to be transmitted. 0=not ready for data yet

;Although this example will not "use" the interrupts, I will
;include a short "dummy" interrupt section for the sake of those
;who might not know how to save the context (Status/W/FSR)


	org	h'0004'		;interrupt vector location

inthandler
			;global interrupts automatically disabled on entry!
			;first, save w and status so we don't really
			;mess up the system!
	movwf	savew1		;save w register! (at h'20', h'A0', etc.)
	movf	status,w	;w now has copy of status
	clrf	status		;ensure we are in bank 0 now!
	movwf	savestatus	;save status
	movf	pclath,w	;save pclath
	movwf	savepclath	;!!! SUBTLE GOTCHA !!! This one doesn't
				;bite you until you have code that
				;crosses page boundaries. How insidious!
	clrf	pclath		;explicitly select Page 0

	movf	fsr,w
	movwf	savefsr		;save fsr (just in case)


vector_to_interrupt

	btfsc	intcon,t0if	;test to see which interrupt 
	goto	service_t0if	;needs servicing...

	btfsc	intcon,intf	;there can be many different sources 
	goto	service_intf	;of interrupt...
				;add as many checks here as you 
				;have possible interrupt sources.... 
				;We only show two here...

;actual interrupt stuff would go here.
;this is just a dumb old dummy routine, so we will be
;really lazy and do absolutely nothing worthwhile here.

service_t0if
	nop			;!!! or do something useful here...

t0if_done
	bcf	intcon,t0if	;clear interrupt flag that caused interrupt.
	goto	intclean	;restore and return from interrupt!

;the above bcf instruction is just to remind us that if we serviced
;ANY of the interrupt flags, we *HAVE* to clear THAT flag before exiting
;from the interrupt routine. if you don't do this the interrupt will
;be re-entered as soon as you attempt to leave it. And then your
;wonderful UART routines will appear as if they aren't working.

service_intf	nop			;!!! or do something useful here...

intf_done	bcf	intcon,intf	;clear flag that caused interrupt.
		goto	intclean	;restore and return from interrupt!

;
;this interrupt example is only here to remind you of things like
;saving the context and clearing the interrupt flag. It is NOT
;meant to actually do anything particularly useful. It is
;a dummy routine.

intclean
		movf	savefsr,w
		movwf	fsr		;restore fsr
	
		movf	savepclath,w
		movwf	pclath		;restore pclath. (Page=original)

		movf	savestatus,w
		movwf	status		;restore status! (bank=original)

		swapf	savew1,f	;restore w from *original* bank! 
		swapf	savew1,w	;swapf does not affect any flags!


		retfie                  ;return from interrupt!
					;gie is auto-re-enabled.

;********************************
;end of interrupt routine	*
;********************************

;

;*******************************************************************
;*******************************************************************

;************************
;INITIALIZATION ROUTINE *
;************************
;What is shown here is just a sort of bare-bones initialization.
;You would normally add to this the initialization of the other
;ports and features you will be using. I find it useful to assume
;absolutely nothing about the states of the registers. So you
;may find me clearing a register that the manual says defaults
;to cleared on start-up. I would rather waste a few code bytes
;setting up my assumptions rather than finding out later in some
;obscure erata sheet that there is a "bug" in the way something
;initializes. Better safe than sorry.

;Ah, yes! !!! SUBTLE GOTCHA !!! the ORDER in which initialization steps
;are performed MAY be important. Re-arrange the order of the
;following items at your own risk!

initialize
				;initialize ports and registers

	bcf	status,rp0	;first do page 0 stuff. yep, page 0

gie01	bcf	intcon,gie	;turn gie off 
	btfsc	intcon,gie	;MicroChip recommends this check!
	goto 	gie01		;!!! GOTCHA !!! without this check
				;you are not sure gie is cleared!

;If an IRQ happens when the "bcf intcon,gie" line is executing, 
;the GIE bit will be cleared, but the interrupt will still occur. 
;If the interrupt routine has a RETFIE instruction at the
;end, (which is the usual state of affairs),then GIE will 
;be set to 1 again. 
;
;This GIE error will only occur *randomly* in your
;program, and the CAUSE of such *intermittent* failures can be a real
;pain to track down. 
;Hmmmm. I wonder if you have any already existing code that you
;have written that does *not* do a loop check for GIE?
;If so. it is an accident waiting to happen.



	clrf	sspcon		;sync serial not used if in async mode.
				;I don't know if you really need to
				;clear this register, but it doesn't
				;hurt to do so. At least this way any
				;assumptions that we make about sspcon
				;are based on the fact that we
				;cleared it.

	clrf	pir1		;clear peripheral flags
	clrf	pir2		;all of them
 
	clrf	porta		;clear all i/o registers...
	clrf	portb
	clrf	portc
	clrf	portd
	clrf	porte

 	bsf     status,rp0	;allow access to page 1 stuff!

;********************************
;********** page 1 stuff        *
;********************************

	clrf	pie1		;disable peripheral interrupts
	clrf	pie2		;all of them

	movlw	b'00000111'	;set all i/o as digital. no analog.
	movwf	adcon1
				;normally set porta i/o here...
				;GOTCHA!!
				;remember that ra4 is OPEN DRAIN and may 
				;require a pullup resistor in MOST 
				;applications! Failure to observe this
				;fact has caused many a headache for the
				;unwary PIC user!

				;normally set portb i/o here...

				;normally set portc i/o here..

	movlw	b'11000000'	;set portc direction for i/o pins
	movwf	trisc		;0=output  1=input
				; rc7 input _uart_rc
				;!!! rc6 input _uart_tx. Actually an output,
				;!!! BUT for UART use rc6 MUST be programmed
				;!!! as an INPUT! 
				;(How wonderfully counter-intuitive!)

;!!! GOTCHA !!! portc,6 *MUST* be programmed as an INPUT, even though it is 
;used as a transmitter output!!!
;Note that it seems that many PICs will actually work with portc,6 set
;either way. However, you are only *sure* things will work right *always*
;and for *all* PICs if you follow the Microchip Errata Note and set
;portc,6 to a "1". I wouldn't play Russian Roullette with this one if
;I were you. Go with what ALWAYS works!
 
				;normally set portd i/o here...
 
				;normally set porte i/o here: *3* bit i/o port.
				;bits 0,1,2 are set for output
				;other bits 3,4,5,6,7 are set to 0
				;!!! (since we want to run in i/o mode)
	movlw	b'00000000'	;set porte direction for i/o pins
	movwf	trise		;0=output  1=input

				;Change according to your needs. But. 
				;note: re4 *MUST* be 0, as it sets the
				; pspmode to general purpose i/o.
				; safest to write bits 7,6,5,4,3 with 0
				; in *most* implementations.
				;of course if you are interfacing a RAM
				;chip here, then ignore this recommendation.

				;option register handles several details...

;!!! Mini GOTCHA !!! The OPTION register MUST be referred to as
;OPTION_REG, *NOT* OPTION, since OPTION is an assembler directive name.

	bsf     option_reg,not_rbpu	;!rbpu! rb_pullup 0=enabled 
				; 1=disabled. enabling is based on individual 
				;port-latch values. currently pullups are 
				;disabled.
	bsf     option_reg,intedg	;intedg 0=inc on falling 1=inc on 
				; rising edge. <<note: intedg and t0se use 
				; opposite definitions!>>
				;currently set for rising edge detection.
	bcf     option_reg,t0cs	;t0cs timer0clocksource 0=internal clkout
				;1=ra4/int. currently set for internal clkout

	bcf     option_reg,t0se	;t0se timer0signaledge 0=inc on rising 1=inc 
				; on falling edge.
				; <<note: intedg and t0se use opposite 
				; definition!>>
	bcf     option_reg,psa	;psa prescaler assignment 0=tmr0 1=wdt
				;ps2-ps0 determine prescaler rate, which is
				;dependent also on whether tmr0 or wdt is 
				;selected:
			;wdt from 0-7 is div by 1 2 4 8 16 32 64 128
			;tmr0 from 0-7 is div by 2 4 8 16 32 64 128 256
			;if wdt is assigned prescaler, then tmr0 is div by 1
			; here we will set prescaler to divide by 16 for tmr0
			; !!! set this any way you want. 
			; This is just an example that works.
	bcf     option_reg,ps2	;ps2 
	bsf     option_reg,ps1	;ps1
	bsf     option_reg,ps0	;ps0

;intcon register: bit assignments
;
;enables... 1=enable 0=disable
;<7>=gie=global_int_enable
;<6>=peie=peripheral_int_enable
;<5>=t0ie=t0_int_enable (enables <2> t0if)
;<4>=inte=int_enable (rb0/int) (enables <1> intf)
;<3>=rbie=rb_int_enable (enables <0> rbif)
;
;intcon flags. software reset. 0=reset 1=flagged
;<2>=t0if=t0_int_flag
;<1>=intf=int_flag (rb0/int)
;<0>=rbif=rb_int_flag (rb7-rb4)

	clrf	intcon		;in this example we have no interrupts used.
;
;pie1 peripheral interrupt enable 1 register:
; bit assignments. 1=enable  0=disable
;
;<7>=pspie=parallel_slave_port_int_enable
;<6>=adie=a/d_int_enable
;<5>=rcie=receiver_int_enable for uart (may use later)
;<4>=txie=transmit_int_enable for uart (may use later)
;<3>=sspie=sync_serial_int_enable
;<2>=ccp1ie=ccp1_int_enable
;<1>=tmr2ie=timer2_int_enable
;<0>=tmr1ie=timer1_int_enable

	clrf	pie1		;no interrupts used in this example

;
;pie2 register: single bit assignment. 1=enable  0=disable
;
;<0>=ccp2ie=capture/compare2_int_enable

	bcf	pie2,ccp2ie	;disable ccp2ie

;uart specific initialization
				;txsta=Transmit STAtus and control register.
				;take nothing for granted.
	bcf	txsta,csrc	; <7> (0) don't care in asynch mode
	bcf	txsta,tx9	; <6>  0  select 8 bit mode
	bsf	txsta,txen	; <5>  1  enable transmit function 
				;      *MUST* be 1 for transmit to work!!!
	bcf	txsta,sync	; <4>  0 asynchronous mode. 
				;      *MUST* be 0 !!!
				;      If NOT 0 the async mode is NOT selected!
				; <3>  (0) not implemented
	bcf	txsta,brgh	; <2>  0 disable high baud rate generator !!!
				; !!!  errata sheet says NOT to set high for 
				;      16C74A due to excessive receive errors!
				; 1    (0) trmt is read only.
	bcf	txsta,tx9d	; <0>  (0)  tx9d data cleared to 0.

 ;!!! GOTCHA !!! The manual actually suggests that you SHOULD use
 ;txsta,brgh = 1. The errata sheet states that with the 16C74A version
 ;you should NOT set txsta,brgh high, as the '74A part often will
 ;have excessive read errors when this bit is set high.
 ;SO SET IT LOW! The '74B and later parts do not have this problem.
 ;For the sake of sanity, SET IT LOW unless you really really need
 ;to have the high baud rate feature. Eventually some one will
 ;use your software with a '74A device and you will be sitting
 ;there wondering why your PIC no longers works reliably.

;Now we have to set the value for spbrg (Serial Programmable
;Baud Rate Generator). This looks easy. Microchip gives us a couple
;of PAGES on how to do this, and they have even been so
;considerate as to provide us with several handy tables.

;!!! GOTCHA !!! The formula presented in the book is in terms of
;baudrate. YOU have to re-arrange it (as I have done here) to
;give the expression in terms of spbrg_value.
;(Why doesn't Microchip just give us the formula in terms of
;spbrg value in the first place???)

;OK, so you figure you'll just skip the formulas and use the nice
;tables that Microchip so graciously presented us with.
;   !!! DOUBLE GOTCHA !!!.
;   Do NOT use the baud rate tables in the data sheets!
;   There are some errors in the tables.
;   for example, the table for BRGH=1 at 4MHz async says 57.6kbps at
;   spbrg=25. That is incorrect. 9600bps is correct for spbrg=25 @ 4MHz.
;   It *looks* like the BRGH=1 tables are the only ones incorrect, but
;   you should double-check, or just use the proper formula to get 
;   the value. Yet another good reason to set BRGH=0!

;   I STRONGLY suggest using calculations to determine spbrg value
;
;   For brgh=0       baudrate=Fosc/(64(spbrg+1))
;   So when brgh=0   spbrg_value = (xtal_freq/(baudrate*d'64'))-1

;   For brgh=1       baudrate=Fosc/(16(spbrg+1)) 
;   So when brgh=1   spbrg_value = (xtal_freq/(baudrate*d'16'))-1


xtal_freq	=      	d'16000000'	;crystal frequency in Hertz.
					;!!! set for YOUR crystal.
baudrate	=	d'4800'	;desired baudrate.
				;now calculate spbrg_value...
spbrg_value	=	(xtal_freq/(baudrate*d'64'))-1
				;this is based on txsta,brgh=0.
				;if you have set txsta,brgh to 1, then you
				;will have to use the alternate formula
				;for when brgh=1. See formulas above...
				;BUT, remember that for '74A you should
				;always choose brgh=0.

	movlw	spbrg_value	;set baud rate generator value
	movwf	spbrg

;***************************************************************************		
;* Tony Kubek made the following additional suggestions:
;*
;* ;Calculation of baudrate could be a little better by adjusting 
;* ;for rounding.
;*
;* ; base frequency
;* XTAL_FREQ       EQU     d'16000000'        ; OSC freq in Hz
;*
;* baudrate = d'4800'
;*
;* ; calculates baudrate when BRGH = 1, adjust for rounding errors
;* spbrg_value = (((d'10'*XTAL_FREQ/(d'16'*baudrate))+d'5')/d'10')-1
;*
;* ; calculates baudrate when BRGH = 0, adjust for rounding errors
;* spbrg_value = (((d'10'*XTAL_FREQ/(d'64'*baudrate))+d'5')/d'10')-1
;*
;* movlw	spbrg_value	;set baud rate generator value
;* movwf	spbrg
;*
;***************************************************************************

;********************************
;********** page 1 stuff done!	*
;********************************

;revert back to page 0 

	bcf	status,rp0	;allow access to page 0 stuff again. (normal)

 
;more uart specific initialization

				;rcsta=ReCeive STAtus and control register
				;assume nothing.

	bsf	rcsta,spen	; 7 spen 1=rx/tx set for serial uart mode
				;   !!! very important to set spen=1
	bcf	rcsta,rx9	; 6 rc8/9 0=8 bit mode
	bcf	rcsta,sren	; 5 sren 0=don't care in uart mode
	bsf	rcsta,cren	; 4 cren 1=enable constant reception
				;!!! (and low clears errors)
				; 3 not used / 0 / don't care
	bcf	rcsta,ferr	; 2 ferr input framing error bit. 1=error
				; 1 oerr input overrun error bit. 1=error
				;!!! (reset oerr by neg pulse clearing cren)
				;you can't clear this bit by using bcf.
				;It is only cleared when you pulse cren low. 
	bcf	rcsta,rx9d	; 0 rx9d input (9th data bit). ignore.


      ;GOTCHA! If you are using a MAX232 or similar device that uses
      ;charge pumping, you might want to place a delay routine
      ;right HERE to ensure that the charge pump voltage has risen
      ;to its proper operating level before any actual RS232
      ;level i/o is attempted! Another alternative is to place a
      ;DELAY routine right at the beginning of the initialization
      ;routine. The length of the delay depends on what kind of "stuff"
      ;you have on-board. Typical delays might range from a few
      ;milliseconds to several seconds, depending on what hardware
      ;you have in your system. Many subtle problems crop up when
      ;the PIC attempts to deal with hardware that is not yeat in
      ;a known stable condition.

;DELAY routine goes HERE or at beginning of initialization
;routine. I have not actually included a delay routine, because
;in many cases you will already have some sort of delay routines
;already written, and it is useful to just call these existing
;routines.

;we need to initialize some things, so do it here.

	movf	rcreg,w		;clear uart receiver
	movf	rcreg,w		; including fifo
	movf	rcreg,w		; which is three deep.

;!!! GOTCHA !!! if you do not initially do these three reads the
;receiver may come up with an error condition on start-up.
;Of course in our case we have included error recovery code
;that would actually fix this problem, but putting this code
;fragment here makes us more aware of the problem right up front.

	movlw	0		;any character will do.
	movwf	txreg		;send out dummy character
				; to get transmit flag valid!

;!!! SUPER HUMONGOUS GOTCHA !!! if you forget to send out an initial dummy 
;character, then the txif flag never goes high and your serial input
;routine will go round and round in circles forever waiting for txif
;to go high. This is a WONDERFUL BUG that is guaranteed to drive
;you nuts and make all your hair fall out. (well, actually
;your hair doesn't really FALL out. You end up pulling it all
;out in utter frustration.)


;ready now to begin main user program.
		
main
	bsf	intcon,gie	;enable interrupts if you are using any!
				;in this example we don't, but since many
				;people *will* be using interrupts for other
				;things, we will include code that will
				;work properly with interrupts.	

loop
	call	ser_in		;get UART input into W and rx_data
	call	transmitw	;send W to the UART transmitter
	goto	loop		;blithely echo characters forever...

;*****************************************************************

;!!! GOTCHA !!! The PC uses rs232 transmission of the UART data.
;In rs232 transmission the UART data stream is INVERTED. So if
;you want to "talk" or "listen" to devices like the PC you will
;need to have some form of inversion on both the transmit and
;receive lines. There are a number of devices specifically
;designed for rs232 signal conversion. For the PC I have found
;that you can use an HCT device, such as the 74HCT00, which is
;a quad NAND device. For input FROM the PC to the PIC use a
;22k resistor in series with two inputs of one of the NAND gates.
;connect the output of this NAND gate to the PIC receiver input.
;For output FROM the PIC to the PC just direct wire the 
;transmitter output to two inputs of one of the other NAND gates.
;The output of this NAND gate then goes directly to the PC's
;receiver input. Not exactly elegant, but it works.

;The gate that connects to the 22k may be connected up to a
;device that implements the REAL RS232 specification, in which
;case you may have some negative voltages at the input of the 
;22k resistor. Normally this should be no big deal, since the
;built-in input diodes of the HCT series will kick in and prevent
;damage to the NAND gate. However, in a commercial application
;you might want the extra insurance of using a REAL RS232
;interface chip such as the MAX232, ICL232, LT1081, TSC232,
;DS14C232, etc. They are all pin for pin compatible and use
;4 capacitors to generate the proper RS232 voltage levels
;from a single +5 volt supply. Each device contains two inverters.
;One is for converting from CMOS to RS232 levels, and the other is
;for converting from RS232 to CMOS levels. 
;Well worth the investment. Available in several package styles.
;Make sure you use the proper capacitor values. The MAX232 and
;MAX232A, for example, use different capacitor values. Read
;the datasheets!.

;See the GOTCHA note in the initialization section about the 
;adviseability of adding a DELAY on power-up so that no RS232
;transmissions/receptions are attempted until the MAX232
;(or whatever you are using) has a chance to reach the proper
;operating voltage. It is maddening to have an intermittent
;problem caused by power-up issues. When you have a problem
;that seems to be associated with the current phase of the moon,
;it is enough to drive you bonkers! A few lines of code designed
;to prevent such problems is a small price to pay for your
;sanity.

;Many a sleepless evening has been spent by persons who were
;unaware of the hardware inversion issues involved in rs232!
;Don't let yourself become one of the casualties!

;<UART> refers to the software level protocol, which is
;marking high followed by a start bit that goes low for one
;bit period, followed by 8 (or 9) NORMAL data bits sent out in the
;order db0 db1 db2 db3 db4 db5 db6 db7 followed by at LEAST
;one high level stop bit, followed by as much time marking high 
;as your little heart desires.

;<RS232> refers not to the data per se, but to the signal levels
;used to transfer the data, the type of connectors used,
;and the physical pinouts of the connectors used.

;*******************************************************************
;*******************************************************************
;The really really really important thing to remember is that
;if you are communicating with another PIC or a PC or other device
;that has implemented rs232, then YOU have to somehow implement
;at least some reasonable facsimile of rs232 on the PIC hardware
;YOU are designing. Leave out the inverters and you will be
;scratching your head until you are bald trying to figure out
;what the heck is going on (or ISN'T going on, to be more exact!)
;*******************************************************************
;*******************************************************************

;If directly connecting two PICS together at close range you
;can skip the whole rs232 inversion scheme on input and output.
;In this case just direct connect the TX of PIC(A) to the RX of
;PIC(B). Then connect the RX of PIC(A) to the TX of PIC(B).
;In this mode both devices send/receive in UART data protocol,
;and since there is no hardware inversion at either end, you
;save a whole bunch of headaches and extra expense.

;OK, so you fully understand the RS232 inversion issue and take
;care of that problem with a little hardware. You feel like you
;are really getting somewhere with this whole UART thing. 
;Think again!

;!!! GOTCHA !!! On the PC side of the connector that will be 
;connecting to the PIC you must wire the handshaking lines
;so that they "fake" one another into seeing valid handshake
;signals, even though the PIC does not generate these signals.

;!!! DOUBLE GOTCHA !!! You discover that the PC serial port
;is so wonderfully "standard" that it has two totally different
;connecters associated with it. One has 9 pins and the other
;has 25 pins. Go with the 9 pin version if at all possible.

;There are several ways to wire up the handshaking lines at the
;point where they come into the PIC hardware. Here is one setup
;that has always worked for me:

;________________________________________________________________
;PC   | ALT.  |PC SIDE| out>----->in     | DB9            |PIC 
;SIG. |CABLE  |*CABLE*| in<------<out    | PIC SIDE CABLE |SIGNAL
;NAME |(DB25F)|DB9F   | SIGNAL DIRECTION | CONNECTION ETC.|NAME
;----------------------------------------------------------------
;GND	(7)	5 <---------------------> ------------------PIC GND
;TX	(2)	3 >---------------------> *INVERTER TO >----PIC RX
;RX	(3)	2 <---------------------< *INVERTER FROM <--PIC TX

;RTS	(4)	7 <----------------------+ TO PC CTS
;                                    | (connect at PIC side)
;CTS	(5)	8 >----------------------+ TO PC RTS

;DSR	(6)	6 >----------------------+ TO PC CD
;                                    | (connect at PIC side)
;CD	(20)	4 <----------------------+ TO PC DSR

;* NOTE the inverters required between the PIC RX and TX and the
;cable. Save your hair. Put them in. Splurge a little. Use a MAX232.

;Not sure whether you are going to need the inverted or non-inverted
;signals for a particular project? Then make it so it will work
;with BOTH! Use berg type jumpers to select the proper signal type.
;(a berg style jumper is a little jumper socket that can be placed
;across .1" spaced square posts that solder to the pc board).


;****************
;SER_IN         *
;SERIAL INPUT   *
;****************

;enter with nothing
;exit with received serial data in W and in variable rx_data

;to receive:
;to enable reception of a byte, cren must be =1. on any error, recover by
;pulsing cren low then back to high. when a byte has been received the 
;rcif flag will be set. rcif is automatically cleared when rcreg is read
;and empty. rcreg is double buffered, so it is a two byte deep fifo. if a
;third byte comes in, then oerr is set. you can still recover the two bytes
;in the fifo, but the third (newest) is lost. cren must be pulsed negative
;to clear the oerr flag. on a framing error ferr is set. ferr is 
;automatically reset when rcreg is read, so errors must be tested for 
;*before* rcreg is read. it is *NOT* recommended that you ignore the 
;error flags. eventually an error will cause the receiver to hang up 
;if you don't clear the error condition.

;note: the subroutine SER_IN handles all the details for you.

ser_in
	btfsc	rcsta,oerr
	goto	overerror	;if overflow error...
	btfsc	rcsta,ferr
	goto	frameerror	;if framing error...
uart_ready
	btfss	pir1,rcif
	goto	ser_in		;if not ready, wait...
				;eventually we get something!
uart_gotit
	bcf	intcon,gie	;turn gie off. This is IMPORTANT!
	btfsc	intcon,gie	;MicroChip recommends this check!
	goto 	uart_gotit	;!!! GOTCHA !!! without this check
				;you are not sure gie is cleared!


;!!! GOTCHA !!! I originally didn't have the proper interrupt
;handling items here, and the routine sometimes worked, and
;sometimes crashed until I added the interrupt handling.

	movf	rcreg,w		;recover uart data
	bsf	intcon,gie	;re-enable interrupts!!
	movwf	rx_data		;save for later
	return

overerror	   		;over-run errors are usually
				;caused by the incoming data
				;building up in the fifo.
				;this is often the case when
				;the program has not read the
				;uart in a while.
				;flushing the fifo will
				;allow normal input to resume.
				;note that flushing the fifo
				;also automatically clears 
				;the ferr flag.
				;pulsing cren resets the oerr flag

	bcf	intcon,gie	;turn gie off. This is IMPORTANT!
	btfsc	intcon,gie	;MicroChip recommends this check!
	goto 	overerror	;!!! GOTCHA !!! without this check
				;you are not sure gie is cleared!

	bcf	rcsta,cren	;pulse cren off...
	movf	rcreg,w		;flush fifo
	movf	rcreg,w		; all three elements.
	movf	rcreg,w
	bsf	rcsta,cren	;turn cren back on.
				;this pulsing of cren
				;will clear the oerr flag.
	bsf	intcon,gie	;enable interrupts.
	goto	ser_in		;try again...

frameerror			;framing errors are usually
				;due to wrong baud rate
				;coming in.

	bcf	intcon,gie	;turn gie off. This is IMPORTANT!
	btfsc	intcon,gie	;MicroChip recommends this check!
	goto 	frameerror	;!!! GOTCHA !!! without this check
				;you are not sure gie is cleared!

	movf	rcreg,w		;reading rcreg clears ferr flag.
	bsf	intcon,gie	;enable interrupts.
	goto	ser_in		;try again...

;!!! GOTCHA !!! Now, there will be wonderfully intelligent people out 
;there who will look at all this error recovery stuff and say to themselves:
;  "I'm a *real* man, macho as they come. I don't need no steeeenking
;   error correction crap. I can save a couple of bytes by just leaving
;   out all that error correction crud. Heck, I've had my own routines
;   running for weeks without any of that stupid error recovery junk,
;   and I have YET to have a single problem."

;Sorry, buddy. You've got a problem, but you just don't know it yet.
;But that's OK. Your customers will discover the problem FOR you
;when they start using your wonderful PIC project in the real world.
;Then you can explain to THEM how you saved a few precious bytes of
;code space by leaving out the error recovery routines.

;Error recovery routines: you don't need them until you need them.
;Then you REALLY need them!

;*************************************************
;* TRANSMIT and TRANSMITW Serial Output Routines *
;*************************************************

;TRANSMIT subroutine:
;enter with data in tx_data.
;returns with data in tx_data and w.

;alternately, we have an entry point called 
;TRANSMITW subroutine:
;enter with data in W.
;returns with data in W.

;I usually find the TRANSMITW routine to be more useful,
;since I usually have the data in W anyway.

;(note: as part of initialization you MUST have sent out
;some piece of data to txreg. This "primes" the transmit
;circuitry and ensures that the txif flag works properly.)

;you may load transmit data when txif=1. this is reset automatically
;when data is loaded. can not be reset by user using any other method!
;data transmission occurs when txreg is loaded with data and txen is set=1.
;!!! GOTCHA !!! normally just keep txen=1 all the time.

;data transmission occurs when txreg is loaded with data and txen is set=1
;txmt and txif are set/reset automatically. txmt shows the state of the 
;shift reg. But. You guessed it. !!! GOTCHA !!! txmt is in bank 1. 
;Don't you just LOVE switching banks? I detest it.
;In my routines I use txif exclusively. Works like a charm.

transmit
	movf	tx_data,w	;copy tx_data to w.
transmitw
				;transmitw is most common entry point.
				;(output what is in W)
	btfss	pir1,txif
	goto	transmitw	;wait for transmitter interrupt flag
gietx	bcf	intcon,gie	;disable interrupts
	btfsc	intcon,gie	;making SURE they are disabled!
	goto	gietx
	movwf	txreg		;load data to be sent...
	bsf	intcon,gie	;re-enable interrupts
	return			;tx_data unchanged. 
				;transmitted data is in W

				;if entry point was <transmit>, then
				;tx_data is unchanged, and W is a copy
				;of tx_data
 
	end

;**********************************************************************
;Tony Nixon offers the following advice:
;
;A lot of times I have used the following subroutine just after sending 1
;or 2 bytes of data so that I always know the TXREG is empty.
;
; ------------------------------------
; WAIT UNTIL RS232 IS FINISHED SENDING
; ------------------------------------
;
;TransWt bsf STATUS,RP0
;TxWt btfss TXSTA,TRMT ; (1) transmission is complete if hi
;goto TxWt
;clrf STATUS ; RAM Page 0
;return
;
;
;It seems that if the UART is disabled for awhile during program
;execution, and then it is enabled again at some other point, then the
;data in the TXREG is transmitted. This caused me a lot of headaches
;especially while developing ROMzap.
;***********************************************************************



;WHEW! No more GOTCHAS to report. ENJOY!

;==============================================================================
; end of program. Additional Notes follow...
;==============================================================================


;******************************************************************************
;******************************************************************************
;you can test your hardware by assembling the code in this program
;and testing with a PC running HyperTerminal, which is included
;with Windows95/98/NT. You have to set the com port and baud rate,
;and I recommend setting handshaking off even if you have implemented
;the suggested faking of the handshake signals.
;
;You say you have Windows 95/98/NT but you can't find this 
;HyperTerminal thingie? If you have it installed it is under
;Start/ Programs/ Accessories/ Communications/ HyperTerminal.

;Using MPLAB, assemble this PICUART file and then program your PIC.
;Put the PIC into your pc board. (It don't work without it!)
;Connect the appropriate cable up between your hardware and your
;PC. Turn on PIC!!! Run hyperterminal and set it up as described above.
;
;If you have your hardware properly designed with the necessary
;inverters and all, then whatever you type on the keyboard of the
;PC will be echoed back by the PIC and show up on the monitor.
;
;If this doesn't work, then you have a hardware problem, because
;this software has been tested, and I *know* it works correctly.
;
;Most likely problems are:

; You have been staying up too late and forgot to turn the PIC
; hardware ON!

; You left out the inverters even though we warned you about that
; 'til we were blue in the face.

; You don't have the PIC side of the cable wired to "fake" the
; handshake signals.

; You are a total basket case and don't know which end of a 
; soldering iron to hold. (You are technologically challenged).
; as a result you did your soldering of the PIC using a welder.
; Check for solder bridges.

; You didn't set up HyperTerminal correctly.

; You are totally wasted and made the mistake of trying to
; "improve" on this software instead of just assembling it AS-IS.

; Don't fool around with it until you get it working AS-IS.
; THEN you can fool around with it all you want. Just don't
; blame me for the wasted hours and misery which you have thus
; inflicted upon yourself.

;*****************************************************************
;
;If you can't find HyperTerminal on your PC, you can load it
;if you have the original CD. Go to Start/ Settings/ Control Panel/
;Add New Programs/ Windows Setup/ Communications/ click Details/
;select HyperTerminal/ click on Apply.
;You will be asked to load the Windows CDROM.
;Once loaded you will find Hyperterminal under Start/ Programs/ 
;Accessories/ Communications/ HyperTerminal. Don't forget to
;set it up for the proper baud rate, com port, etc. Choose
;No Handshaking and eliminate at least one potential headache.
;Once you have HyperTerminal running to your satisfaction,
;place an Icon shortcut to this version of HyperTerminal
;so you don't have to re-configure it each time you use it.
;
;If you have a MAC or are running Linux or any other non-Windows
;operating system, I cannot help you. However, I am sure that there
;must be some sort of simple terminal program available for your
;operating system. Ask around or do a web search. If you find
;something that works for you, let me know the details and I can
;pass that info along to others.
;
;If you have found this program/document/tutorial useful,
;please take a moment to drop me an e-mail. It is only through
;feedback that I find out whether the stuff I am putting out is
;really all that useful to others. And who knows, you might 
;have some useful feedback that I can then pass along to others.
;
; Fr. Tom McGahee
; tom_mcgahee@sigmais.com
;*****************************************************************

