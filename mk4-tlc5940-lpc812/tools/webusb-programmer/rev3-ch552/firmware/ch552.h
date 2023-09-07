#pragma once
/*

Header file for CH552 microcontrollers for the SDCC compiler.

Clean-room creation based on the CH552 datasheet.


Copyright (c) 2021 Werner Lane

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


// ***************************************************************************
// General 8051 registers
__sfr __at (0xf0) B;            // General purpose register B
__sfr __at (0xe0) ACC;          // Accumulator

#define PSW_ADDR (0xd0)
__sfr __at (PSW_ADDR) PSW;          // Program Status Word
__sbit __at (PSW_ADDR | 7) CY;      // Carry flag
__sbit __at (PSW_ADDR | 6) AC;      // Auxillary carry flag
__sbit __at (PSW_ADDR | 5) F0;      // Bit-addressable general purpose flag 0
__sbit __at (PSW_ADDR | 4) RS1;     // Register bank select bit high
__sbit __at (PSW_ADDR | 3) RS0;     // Register bank select bit low
__sbit __at (PSW_ADDR | 2) OV;      // Overflow flag
__sbit __at (PSW_ADDR | 1) F1;      // Bit-addressable general purpose flag 1
__sbit __at (PSW_ADDR | 0) P;       // Parity flag

__sfr __at (0xb1) GLOBAL_CFG;   // Global Configuration Register (only writable in safe mode)
#define bBOOT_LOAD (1 << 5)     // Boot loader status bit (read-only)
#define bSW_RESET (1 << 4)      // Software reset bit
#define bCODE_WE (1 << 3)       // Flash-ROM and Data-Flash write enable bit
#define bDATA_WE (1 << 2)       // Data-Flash write enable bit
#define bLDO3V3_OFF (1 << 1)    // 3.3V LDO disable
#define bWDOG_EN (1 << 0)       // Watchdog reset enable

// These SFR share the same address!
__sfr __at (0xa1) SAFE_MOD;     // Safe Mode Control Register (write-only)
__sfr __at (0xa1) CHIP_ID;      // Chip ID number (read-only)

__sfr __at (0x83) DPH;          // Data Pointer High
__sfr __at (0x82) DPL;          // Data Pointer Low
__sfr __at (0x81) SP;           // Stack Pointer


// ***************************************************************************
// Power and sleep control registers
__sfr __at (0xff) WDOG_COUNT;   // Watchdog counter register
__sfr __at (0xfe) RESET_KEEP;   // This register is preserved over resets

__sfr __at (0xa9) WAKE_CTRL;    // Sleep wake-up control register
#define bWAK_BY_USB (1 << 7)    // Enable wake-up by USB
#define bWAK_RXD1_LO (1 << 6)   // Enable wake-up when RXD1 pin goes low
#define bWAK_P1_5_LO (1 << 5)   // Enable wake-up when P1.5 goes low
#define bWAK_P1_4_LO (1 << 4)   // Enable wake-up when P1.4 goes low
#define bWAK_P1_3_LO (1 << 3)   // Enable wake-up when P1.3 goes low
#define bWAK_RST_HI (1 << 2)    // Enable wake-up when RST pin goes high
#define bWAK_P3_2E_3L (1 << 1)  // Enable wake-up on any edge of P3.2 or when P3.3 goes low
#define bWAK_RXD0_LO (1 << 0)   // Enable wake-up when RXD0 pin goes low

__sfr __at (0x87) PCON;         // Power Control Register
#define SMOD (1 << 7)           // UART0 baudrate divider selection
#define bRST_FLAG1 (1 << 5)     // Reset flag high bit (read-only)
#define bRST_FLAG0 (1 << 4)     // Reset flag low bit (read-only)
#define GF1 (1 << 3)            // General purpose flag 1
#define GF0 (1 << 2)            // General purpose flag 0
#define PD (1 << 1)             // Power-down (enter sleep mode)

#define MASK_RST_FLAG (bRST_FLAG1 | bRST_FLAG0) // Mask for reset flag
#define RST_FLAG_SW (0)                         // Software reset
#define RST_FLAG_POR (bRST_FLAG0)               // Power-on reset
#define RST_FLAG_WDOG (bRST_FLAG)               // Watchdog reset
#define RST_FLAG_PIN (bRST_FLAG1 | bRST_FLAG0)  // RST pin reset


// ***************************************************************************
// Flash-ROM and Data-Flash registers
__sfr16 __at((0x85<<8) | 0x84) ROM_ADDR;
__sfr __at (0x84) ROM_ADDR_L;       // address low byte for flash-ROM
__sfr __at (0x85) ROM_ADDR_H;       // address high byte for flash-ROM
__sfr16 __at((0x8f<<8) | 0x8e) ROM_DATA;
__sfr __at (0x8e) ROM_DATA_L;       // Data low byte for Flash-ROM write operation; Data byte for Data-Flash read/write operation
__sfr __at (0x8f) ROM_DATA_H;       // Data high byte for Flash-ROM write operation

// These SFR share the same address!
__sfr __at (0x86) ROM_CTRL;         // Flash-ROM control (write-only)
#define ROM_CMD_WRITE (0x9a)        // Flash-ROM word or Data-Flash byte write command
#define ROM_CMD_READ (0x8e)         // Data-Flash byte read command
__sfr __at (0x86) ROM_STATUS;       // Flash-ROM status (read-only)
#define bROM_ADDR_OK (1 << 6)       // Flash-ROM write address valid flag
#define bROM_CMD_ERR (1 << 1)       // Flash-ROM command error flag

#define DATA_FLASH_ADDR (0xc000)    // Start address of Data-Flash
#define BOOT_LOAD_ADDR (0x3800)     // Start address of the bootloader
#define ROM_CFG_ADDR (0x3ff8)       // Chip configuration address

#define ROM_CHIP_ID_HX (0x3ffa)     // 6 bytes comprising of chip ID and unique serial number
#define ROM_CHIP_ID_LO (0x3ffc)
#define ROM_CHIP_ID_HI (0x3ffe)
#define ID_CH552 (0x52)
#define ID_CH551 (0x51)


// ***************************************************************************
// Clock control registers
__sfr __at (0xb9) CLOCK_CFG;    // System clock configuration
#define bOSC_EN_INT (1 << 7)    // Enable and select the internal oscillator
#define bOSC_EN_XT (1 << 6)     // Enable the external oscillator
#define bWDOG_IF_TO (1 << 5)    // Watchdog timer interrupt flag (read-only)
#define bROM_CLK_FAST (1 << 4)  // Flash-ROM reference clock frequency selection
#define bRST (1 << 3)           // Status of the RST pin (read-only)

#define MASK_SYS_CK_SEL (0x07)  // Mask for system clock
#define SYS_CK_32M (0x07)       // 32 MHz system clock (when using the internal 24 HMz oscillator)
#define SYS_CK_24M (0x06)       // 24 MHz system clock
#define SYS_CK_16M (0x05)       // 16 MHz system clock
#define SYS_CK_12M (0x04)       // 12 MHz system clock
#define SYS_CK_6M (0x03)        // 6 MHz system clock
#define SYS_CK_3M (0x02)        // 3 MHz system clock
#define SYS_CK_750K (0x01)      // 750 KHz system clock
#define SYS_CK_187K (0x00)      // 187.5 KHz system clock


// ***************************************************************************
// Interrupts
#define INT_NO_INT0 (0)             // Interrupt number INT0
#define INT_NO_TMR0 (1)             // Interrupt number Timer0
#define INT_NO_INT1 (2)             // Interrupt number INT1
#define INT_NO_TMR1 (3)             // Interrupt number Timer1
#define INT_NO_UART0 (4)            // Interrupt number UART0
#define INT_NO_TMR2 (5)             // Interrupt number Timer2
#define INT_NO_SPI0 (6)             // Interrupt number SPI0
#define INT_NO_TKEY (7)             // Interrupt number Touch-key timer
#define INT_NO_USB (8)              // Interrupt number USB
#define INT_NO_ADC (9)              // Interrupt number ADC
#define INT_NO_UART1 (10)           // Interrupt number UART1
#define INT_NO_PWMX (11)            // Interrupt number PWM1 / PWM2
#define INT_NO_GPIO (12)            // Interrupt number GPIO
#define INT_NO_WDOG (13)            // Interrupt number Watchdog timer

#define INT_ADDR_INT0 (0x0003)      // Interrupt vector address for INT0
#define INT_ADDR_TMR0 (0x000B)      // Interrupt vector address for Timer0
#define INT_ADDR_INT1 (0x0013)      // Interrupt vector address for INT1
#define INT_ADDR_TMR1 (0x001B)      // Interrupt vector address for Timer1
#define INT_ADDR_UART0 (0x0023)     // Interrupt vector address for UART0
#define INT_ADDR_TMR2 (0x002B)      // Interrupt vector address for Timer2
#define INT_ADDR_SPI0 (0x0033)      // Interrupt vector address for SPI0
#define INT_ADDR_TKEY (0x003B)      // Interrupt vector address for Touch-key timer
#define INT_ADDR_USB (0x0043)       // Interrupt vector address for USB
#define INT_ADDR_ADC (0x004B)       // Interrupt vector address for ADC
#define INT_ADDR_UART1 (0x0053)     // Interrupt vector address for UART1
#define INT_ADDR_PWMX (0x005B)      // Interrupt vector address for PWM1 / PWM2
#define INT_ADDR_GPIO (0x0063)      // Interrupt vector address for GPIO
#define INT_ADDR_WDOG (0x006B)      // Interrupt vector address for Watchdog timer

__sfr __at (0xe9) IP_EX;            // Extended interrupt priority control
#define bIP_LEVEL (1 << 7)          // Current interrupt nesting level
#define bIP_GPIO (1 << 6)           // GPIO input interrupt priority
#define bIP_PWMX (1 << 5)           // PWM1 / PWM2 interrupt priority
#define bIP_UART1 (1 << 4)          // UART1 interrupt priority
#define bIP_ADC (1 << 3)            // ADC interrupt priority
#define bIP_USB (1 << 2)            // USB interrupt priority
#define bIP_TKEY (1 << 1)           // Touch-key timer interrupt priority
#define bIP_SPI0 (1 << 0)           // SPI0 interrupt priority

#define IE_EX_ADDR (0xe8)
__sfr __at (IE_EX_ADDR) IE_EX;          // Extended interrupt enable register
__sbit __at (IE_EX_ADDR | 7) IE_WDOG;   // Watchdog timer interrupt enable
__sbit __at (IE_EX_ADDR | 6) IE_GPIO;   // GPIO interrupt enable
__sbit __at (IE_EX_ADDR | 5) IE_PWMX;   // PWM1 / PWM2 interrupt enable
__sbit __at (IE_EX_ADDR | 4) IE_UART1;  // UART1 interrupt enable
__sbit __at (IE_EX_ADDR | 3) IE_ADC;    // ADC interrupt enable
__sbit __at (IE_EX_ADDR | 2) IE_USB;    // USB interrupt enable
__sbit __at (IE_EX_ADDR | 1) IE_TKEY;   // Touch-key timer interrupt enable
__sbit __at (IE_EX_ADDR | 0) IE_SPI0;   // SPI0 interrupt enable

__sfr __at (0xc7) GPIO_IE;          // GPIO interrupt enable register
#define bIE_IO_EDGE (1 << 7)        // GPIO pin interrupt mode: select level or edge
#define bIE_RXD1_LO (1 << 6)        // RXD1 low / falling edge interrupt enable
#define bIE_P1_5_LO (1 << 5)        // P1.5 low / falling edge interrupt enable
#define bIE_P1_4_LO (1 << 4)        // P1.4 low / falling edge interrupt enable
#define bIE_P1_3_LO (1 << 3)        // P1.3 low / falling edge interrupt enable
#define bIE_RST_HI (1 << 2)         // RST high / rising edge interrupt enable
#define bIE_P3_1_LO (1 << 1)        // P3.1 low / falling edge interrupt enable
#define bIE_RXD0_LO (1 << 0)        // RXD0 low / falling edge interrupt enable

#define IP_ADDR (0xb8)
__sfr __at (IP_ADDR) IP;            // Interrupt priority control register
__sbit __at (IP_ADDR | 7) PH_FLAG;  // High priority interrupt in progress (read-only)
__sbit __at (IP_ADDR | 6) PL_FLAG;  // Low priority interrupt in progress (read-only)
__sbit __at (IP_ADDR | 5) PT2;      // Timer2 interrupt priority
__sbit __at (IP_ADDR | 4) PS;       // UART0 interrupt priority
__sbit __at (IP_ADDR | 3) PT1;      // Timer1 interrupt priority
__sbit __at (IP_ADDR | 2) PX1;      // External interrupt 1 (INT1) priority
__sbit __at (IP_ADDR | 1) PT0;      // Timer0 interrupt priority
__sbit __at (IP_ADDR | 0) PX0;      // External interrupt 0 (INT0) priority

#define IE_ADDR (0xa8)
__sfr __at (IE_ADDR) IE;            // Interrupt enable register
__sbit __at (IE_ADDR | 7) EA;       // Global interrupt enable
__sbit __at (IE_ADDR | 6) E_DIS;    // Disable global interrupts (during Flash-ROM operations)
__sbit __at (IE_ADDR | 5) ET;       // Timer2 interrupt enable
__sbit __at (IE_ADDR | 4) ES;       // UART0 interrupt enable
__sbit __at (IE_ADDR | 3) ET1;      // Timer1 interrupt enable
__sbit __at (IE_ADDR | 2) EX1;      // External interrupt enable (INT1)
__sbit __at (IE_ADDR | 1) ET0;      // Timer0 interrupt enable
__sbit __at (IE_ADDR | 0) EX0;      // External interrupt enable (INT0)


// ***************************************************************************
// GPIO registers
#define P1_ADDR (0x90)              // Port P1 address
__sfr __at (P1_ADDR) P1;            // Port P1
__sbit __at(P1_ADDR | 7) SCK;       // SPI0 clock
__sbit __at(P1_ADDR | 7) TXD1;      // UART1 TX
__sbit __at(P1_ADDR | 7) TIN5;      // Touch-key TIN5
__sbit __at(P1_ADDR | 6) MISO;      // SPI0 MISO
__sbit __at(P1_ADDR | 6) RXD1;      // UART1 RX
__sbit __at(P1_ADDR | 6) TIN4;      // Touch-key TIN4
__sbit __at(P1_ADDR | 5) MOSI;      // SPI0 MOSI
__sbit __at(P1_ADDR | 5) PWM1;      // PWM1 output
__sbit __at(P1_ADDR | 5) TIN3;      // Touch-key TIN3
__sbit __at(P1_ADDR | 5) UCC2;      // CC2 for USB type-C
__sbit __at(P1_ADDR | 5) AIN2;      // AIN2 for ADC
__sbit __at(P1_ADDR | 4) T2_;       // Timer2 external inputalternate pin
__sbit __at(P1_ADDR | 4) CAP1_;     // Timer2 capture 1 input alternate pin
__sbit __at(P1_ADDR | 4) SCS;       // SPI0 CS
__sbit __at(P1_ADDR | 4) TIN2;      // Touch-key TIN2
__sbit __at(P1_ADDR | 4) UCC1;      // USB type-C CC1
__sbit __at(P1_ADDR | 4) AIN1;      // AIN1 for ADC
__sbit __at(P1_ADDR | 3) TXD_;      // UART0 TX alternate pin
__sbit __at(P1_ADDR | 2) RXD_;      // UART0 RX alternate pin
__sbit __at(P1_ADDR | 1) T2EX;      // Timer2 external trigger input
__sbit __at(P1_ADDR | 1) CAP2;      // Timer2 capture 2 input
__sbit __at(P1_ADDR | 1) TIN1;      // Touch-key TIN1
__sbit __at(P1_ADDR | 1) VBUS2;     // USB type-C VBUS2
__sbit __at(P1_ADDR | 1) AIN0;      // ADC AIN0
__sbit __at(P1_ADDR | 0) T2;        // Timer2 external input
__sbit __at(P1_ADDR | 0) CAP1;      // Timer2 capture 1 input
__sbit __at(P1_ADDR | 0) TIN0;      // TIN0 for Touch-key

__sfr __at (0x92) P1_MOD_OC;        // P1 output mode register
__sfr __at (0x93) P1_DIR_PU;        // P1 direction control and pull-up enable

#define P2_ADDR (0xa0)              // Port P2 address
__sfr __at (P2_ADDR) P2;            // Port P2

#define P3_ADDR (0xb0)              // Port P3 address
__sfr __at (P3_ADDR) P3;            // Port P3
__sbit __at(P3_ADDR | 7) UDM;       // ReadOnly: pin UDM input
__sbit __at(P3_ADDR | 6) UDP;       // ReadOnly: pin UDP input
__sbit __at(P3_ADDR | 5) T1;        // external count input for timer1
__sbit __at(P3_ADDR | 4) PWM2;      // PWM output for PWM2
__sbit __at(P3_ADDR | 4) RXD1_;     // alternate pin for RXD1
__sbit __at(P3_ADDR | 4) T0;        // external count input for timer0
__sbit __at(P3_ADDR | 3) INT1;      // external interrupt 1 input
__sbit __at(P3_ADDR | 2) TXD1_;     // alternate pin for TXD1
__sbit __at(P3_ADDR | 2) INT0;      // external interrupt 0 input
__sbit __at(P3_ADDR | 2) VBUS1;     // VBUS1 for USB type-C
__sbit __at(P3_ADDR | 2) AIN3;      // AIN3 for ADC
__sbit __at(P3_ADDR | 1) PWM2_;     // alternate pin for PWM2
__sbit __at(P3_ADDR | 1) TXD;       // TXD output for UART0
__sbit __at(P3_ADDR | 0) PWM1_;     // alternate pin for PWM1
__sbit __at(P3_ADDR | 0) RXD;       // RXD input for UART0

__sfr __at (0x96) P3_MOD_OC;        // P3 output mode register
__sfr __at (0x97) P3_DIR_PU;        // P3 direction control and pull-up enable

__sfr __at (0xc6) PIN_FUNC;         // Pin function selection register
#define bUSB_IO_EN (1 << 7)         // USB UDP / UDM pin enable
#define bIO_INT_ACT (1 << 6)        // GPIO interrupt request activatoin status (read-only)
#define bUART1_PIN_X (1 << 5)       // UART1 alternate pin mapping
#define bUART0_PIN_X (1 << 4)       // UART0 alternate pin mapping
#define bPWM2_PIN_X (1 << 3)        // PWM2 alternate pin mapping
#define bPWM1_PIN_X (1 << 2)        // PWM1 alternate pin mapping
#define bT2EX_PIN_X (1 << 1)        // Timer2 T2EX / CAP2 alternate pin mapping
#define bT2_PIN_X (1 << 0)          // Timer2 T2 / CAP1 alternate pin mapping

__sfr __at (0xa2) XBUS_AUX;         // Bus auxillary setup register
#define bUART0_TX (1 << 7)          // UART0 is transmitting (read-only)
#define bUART0_RX (1 << 6)          // UART0 is receiving (read-only)
#define bSAFE_MOD_ACT (1 << 5)      // Safe mode status (read-only)
#define GF2 (1 << 3)                // General purpose flag 2
#define bDPTR_AUTO_INC (1 << 2)     // Enable DPTR auto-increment afterd MOVX_@DPTR instruction
#define DPS (1 << 0)                // Dual DPTR data pointer selection


// ***************************************************************************
// Timer0 and Timer1 registers
__sfr __at (0x8d) TH1;              // Timer1 counter high byte
__sfr __at (0x8c) TH0;              // Timer0 counter high byte
__sfr __at (0x8b) TL1;              // Timer1 counter low byte
__sfr __at (0x8a) TL0;              // Timer0 counter low byte

__sfr __at (0x89) TMOD;             // Timer0/1 mode register
#define bT1_GATE (1 << 7)           // Timer1 gate enable bit
#define bT1_CT (1 << 6)             // Timer1 timer or counter mode
#define bT1_M1 (1 << 5)             // Timer1 mode select bit high
#define bT1_M0 (1 << 4)             // Timer1 mode select bit low
#define bT0_GATE (1 << 3)           // Timer0 gate enable bit
#define bT0_CT (1 << 2)             // Timer0 timer or counter mode
#define bT0_M1 (1 << 1)             // Timer0mode select bit high
#define bT0_M0 (1 << 0)             // Timer0 mode select bit low

#define MASK_T1_MODE (bT1_M1 | bT1_M0)   // Mask for Timer1 mode
#define T1_MODE_0 (0)
#define T1_MODE_1 (bT1_M0)
#define T1_MODE_2 (bT1_M1)
#define T1_MODE_3 (bT1_M1 | bT1_M0)

#define MASK_T0_MODE (bT0_M1 | bT0_M0)   // Mask for Timer1 mode
#define T0_MODE_0 (0)
#define T0_MODE_1 (bT0_M0)
#define T0_MODE_2 (bT0_M1)
#define T0_MODE_3 (bT0_M1 | bT0_M0)

#define TCON_ADDR (0x88)
__sfr __at (TCON_ADDR) TCON;        // Timer0/1 control register
__sbit __at(TCON_ADDR | 7) TF1;     // Timer1 overflow interrupt flag
__sbit __at(TCON_ADDR | 6) TR1;     // Timer1 start/stop
__sbit __at(TCON_ADDR | 5) TF0;     // Timer0 overflow interrupt flag
__sbit __at(TCON_ADDR | 4) TR0;     // Timer0 start/stop
__sbit __at(TCON_ADDR | 3) IE1;     // INT1 extern interrupt 1 flag
__sbit __at(TCON_ADDR | 2) IT1;     // INT1 extern interrupt 1 trigger mode control
__sbit __at(TCON_ADDR | 1) IE0;     // INT0 extern interrupt 0 flag
__sbit __at(TCON_ADDR | 0) IT0;     // INT0 extern interrupt 0 trigger mode control


// ***************************************************************************
// Timer2 registers
__sfr __at (0xcd) TH2;              // Timer2 counter high byte
__sfr __at (0xcc) TL2;              // Timer2 counter low byte
__sfr16 __at((0xcd<<8) | 0xcc) T2COUNT;
__sfr __at (0xcf) T2CAP1H;          // Timer2 capture 1 high byte
__sfr __at (0xce) T2CAP1L;          // Timer2 capture 1 low byte
__sfr16 __at((0xcf<<8) | 0xce) T2CAP1;
__sfr __at (0xcb) RCAP2H;           // Timer2 count reload / capture 2 high byte
__sfr __at (0xca) RCAP2L;           // Timer2 count reload / capture 2 low byte
__sfr16 __at((0xcb<<8) | 0xca) RCAP2;


__sfr __at (0xc9) T2MOD;            // Timer2 mode register
#define bTMR_CLK (1 << 7)           // Use fsys for Timer 0/1/2
#define bT2_CLK (1 << 6)            // Timer2 internal clock frequency selection
#define bT1_CLK (1 << 5)            // Timer1 internal clock frequency selection
#define bT0_CLK (1 << 4)            // Timer0 internal clock frequency selection
#define bT2_CAP_M1 (1 << 3)         // Timer2 capture mode high bit
#define bT2_CAP_M0 (1 << 2)         // Timer2 capture mode low bit
#define T2OE (1 << 1)               // Timer2 clock output enable: 0=disable output, 1=enable clock output at T2 pin, frequency = TF2/2
#define bT2_CAP1_EN (1 << 0)        // Timer2 capture 1 enable

#define MASK_T2_CAP_MODE (bT2_CAP_M1 | bT2_CAP_M0)   // Mask for Timer2 capture mode
#define T2_CAP_MODE_FALLING_EDGE (0)
#define T2_CAP_MODE_ANY_EDGE (bT2_CAP_M0)
#define T2_CAP_MODE_RISING_EDGE (bT2_CAP_M1 | bT2_CAP_M0)

#define T2CON_ADDR (0xc8)
__sfr __at (T2CON_ADDR) T2CON;      // Timer2 control register
__sbit __at(T2CON_ADDR | 7) TF2;    // Timer2 overflow interrupt flag
__sbit __at(T2CON_ADDR | 7) CAP1F;  // Timer2 capture 1 interrupt flag
__sbit __at(T2CON_ADDR | 6) EXF2;   // Timer2 external trigger flag
__sbit __at(T2CON_ADDR | 5) RCLK;   // UART0 receive clock source selection
__sbit __at(T2CON_ADDR | 4) TCLK;   // UART0 transmit clock source selection
__sbit __at(T2CON_ADDR | 3) EXEN2;  // Timer2 T2EX trigger enable bit
__sbit __at(T2CON_ADDR | 2) TR2;    // Timer2 start/stop
__sbit __at(T2CON_ADDR | 1) C_T2;   // Timer2 clock source selection
__sbit __at(T2CON_ADDR | 0) CP_RL2; // Timer2 function selection


// ***************************************************************************
// PWM1 and PWM2 registers
__sfr __at (0x9e) PWM_CK_SE;        // PWM clock divider setting

__sfr __at (0x9d) PWM_CTRL;         // PWM1 and PWM2 control register
#define bPWM_IE_END (1 << 7)        // PWM end-of-cycle interrupt enable
#define bPWM2_POLAR (1 << 6)        // PWM2 output polarity
#define bPWM1_POLAR (1 << 5)        // PWM1 output polarity
#define bPWM_IF_END (1 << 4)        // PWM end-of-cycle interrupt flag
#define bPWM2_OUT_EN (1 << 3)       // PWM2 output enable
#define bPWM1_OUT_EN (1 << 2)       // PWM1 output enable
#define bPWM_CLR_ALL (1 << 1)       // Clear PWM1 / PWM2 counters and FIFOs
__sfr __at(0x9c) PWM_DATA1;         // PWM1 data register
__sfr __at(0x9b) PWM_DATA2;         // PWM2 data register


// ***************************************************************************
// UART registers
#define SCON_ADDR (0x98)
__sfr __at(SCON_ADDR) SCON;         // UART0 control register
__sbit __at(SCON_ADDR | 7) SM0;     // UART0 mode selection bit 0
__sbit __at(SCON_ADDR | 6) SM1;     // UART0 mode selection bit 1
__sbit __at(SCON_ADDR | 5) SM2;     // UART0 mode selection bit 2
__sbit __at(SCON_ADDR | 4) REN;     // UART0 receive enable
__sbit __at(SCON_ADDR | 3) TB8;     // UART0 transmit 9th data bit
__sbit __at(SCON_ADDR | 2) RB8;     // UART0 receive 9th data bit
__sbit __at(SCON_ADDR | 1) TI;      // UART0 transmit interrupt flag
__sbit __at(SCON_ADDR | 0) RI;      // UART0 receive interrupt flag

#define MASK_UART0_MODE (0xe0)      // Mask for UART0 mode
#define UART0_MODE_0 (0)            // Shift Register, baudrate fixed at fsys/12
#define UART0_MODE_1 (0x40)         // 8-bit UART, baudrate determined by Timer1 or Timer2
#define UART0_MODE_2 (0x80)         // 9-bit UART, baudrate fixed at fsys/128 or fsys/32 (refer to SMOD)
#define UART0_MODE_3 (0xc0)         // 9-bit UART, baudrate determined by Timer1 or Timer2

__sfr __at(0x99) SBUF;              // UART0 data register

#define SCON1_ADDR (0xc0)           // UART1 control register
__sbit __at(SCON1_ADDR | 7) U1SM0;  // UART1 mode selection
__sbit __at(SCON1_ADDR | 5) U1SMOD; // UART1 baudrate divider selection
__sbit __at(SCON1_ADDR | 4) U1REN;  // UART1 receive enable
__sbit __at(SCON1_ADDR | 3) U1TB8;  // UART1 transmit 9th data bit
__sbit __at(SCON1_ADDR | 2) U1RB8;  // UART1 receive 9th data bit
__sbit __at(SCON1_ADDR | 1) U1TI;   // UART1 transmit interrupt flag
__sbit __at(SCON1_ADDR | 0) U1RI;   // UART1 receive interrupt flag

__sfr __at(0xc1) SBUF1;             // UART1 data register
__sfr __at(0xc2) SBAUD1;            // UART1 baudrate setting


// ***************************************************************************
// SPI0 registers
__sfr __at(0xfc) SPI0_SETUP;        // SPI0 setup register
#define bS0_MODE_SLV (1 << 7)       // SPI0 master/slave mode selection
#define bS0_IE_FIFO_OV (1 << 6)     // SPI0 FIFO overflow interrupt enable
#define bS0_IE_FIRST (1 << 5)       // SPI0 First byte received interrupt enable
#define bS0_IE_BYTE (1 << 4)        // SPI0 data transfer complete interrupt enable
#define bS0_BIT_ORDER (1 << 3)      // SPI0 MSB first / LSB first seiection
#define bS0_SLV_SELT (1 << 1)       // SPI0 slave mode chip select active (read-only)
#define bS0_SLV_PRELOAD (1 << 0)    // SPI0 slave pre-loaded data status (read-only)

// These SFR share the same address!
__sfr __at(0xfb) SPI0_S_PRE;        // SPI0 slave mode pre-loaded data register
__sfr __at(0xfb) SPI0_CK_SE;        // SPI0 master mode clock divider selection

__sfr __at(0xfa) SPI0_CTRL;         // SPI0 control register
#define bS0_MISO_OE (1 << 7)        // SPI0 MISO output enable
#define bS0_MOSI_OE (1 << 6)        // SPI0 MOSI output enable
#define bS0_SCK_OE (1 << 5)         // SPI0 SCK output enable
#define bS0_DATA_DIR (1 << 4)       // SPI0 data direction
#define bS0_MST_CLK (1 << 3)        // SPI0 master clock idle high/low selection
#define bS0_2_WIRE (1 << 2)         // SPI0 full-duplex / half-duplex mode
#define bS0_CLR_ALL (1 << 1)        // ???
#define bS0_AUTO_IF (1 << 0)        // SPI0 auto-clear S0_IF_BYTE when accessing the FIFO

__sfr __at(0xf9) SPI0_DATA;         // SPI0 send and receive FIFO

#define SPI0_STAT_ADDR (0xf8)
__sfr __at(SPI0_STAT_ADDR) SPI0_STAT;       // SPI0 status register
__sbit __at(SPI0_STAT_ADDR | 7) S0_FST_ACT; // SPI0 slave first byte received status (read-only)
__sbit __at(SPI0_STAT_ADDR | 6) S0_IF_OV;   // SPI0 slave FIFO overflow interrupt flag
__sbit __at(SPI0_STAT_ADDR | 5) S0_IF_FIRST; // SPI0 slave first byte received interrupt flag
__sbit __at(SPI0_STAT_ADDR | 4) S0_IF_BYTE; // SPI0 dat transfer complete interrupt flag
__sbit __at(SPI0_STAT_ADDR | 3) S0_FREE;    // SPI0 free (not shifting) (read-only)
__sbit __at(SPI0_STAT_ADDR | 2) S0_T_FIFO;  // SPI0 transmit FIFO counter value (read-only)
__sbit __at(SPI0_STAT_ADDR | 0) S0_R_FIFO;  // SPI0 receive FIFO counter value (read-only)


// ***************************************************************************
// ADC registers
#define ADC_CTRL_ADDR (0x80)
__sfr __at(ADC_CTRL_ADDR) ADC_CTRL; // ADC control register
__sbit __at(ADC_CTRL_ADDR | 7) CMPO;        // Comparator result (read-only)
__sbit __at(ADC_CTRL_ADDR | 6) CMP_IF;      // Comparator result changed
__sbit __at(ADC_CTRL_ADDR | 5) ADC_IF;      // ADC conversion completed interrupt flag
__sbit __at(ADC_CTRL_ADDR | 4) ADC_START;   // Start ADC conversion
__sbit __at(ADC_CTRL_ADDR | 3) CMP_CHAN;    // Comparator negative input selection
__sbit __at(ADC_CTRL_ADDR | 1) ADC_CHAN1;   // ADC and Comparator positive input selection high bit
__sbit __at(ADC_CTRL_ADDR | 0) ADC_CHAN0;   // ADC and Comparator positive input selection low bit

#define MASK_ADC_CHAN (0x03)        // Mask for ADC channel selection
#define ADC_CHAN_AIN0_P11 (0x00)    // AIN0 / P1.1
#define ADC_CHAN_AIN1_P14 (0x01)    // AIN1 / P1.4
#define ADC_CHAN_AIN2_P15 (0x02)    // AIN2 / P1.5
#define ADC_CHAN_AIN3_P32 (0x03)    // AIN3 / P3.2

__sfr __at(0x9a) ADC_CFG;           // ADC configuration register
#define bADC_EN (1 << 3)            // ADC power control
#define bCMP_EN (1 << 2)            // Comparator power control
#define bADC_CLK (1 << 0)           // ADC clock selection

__sfr __at(0x9f) ADC_DATA;          // ADC sampled data



// ***************************************************************************
// USB device registers
__sfr __at(0x91) USB_C_CTRL;        // USB type-C control register
#define bVBUS2_PD_EN (1 << 7)       // Enable 10K pull-down on VBUS2
#define bUCC2_PD_EN (1 << 6)        // Enable 5.1K pull-down on CC2
#define bUCC2_PU1_EN (1 << 5)       // CC2 pull-up control high bit
#define bUCC2_PU0_EN (1 << 4)       // CC2 pull-up control low bit
#define bVBUS1_PD_EN (1 << 3)       // Enable 10K pull-down on VBUS1
#define bUCC1_PD_EN (1 << 2)        // Enable 5.1K pull-down on CC1
#define bUCC1_PU1_EN (1 << 1)       // CC1 pull-up control high bit
#define bUCC1_PU0_EN (1 << 0)       // CC1 pull-up control low bit

#define MASK_UCC1_PU (bUCC1_PU1_EN | bUCC1_PU0_EN) // Mask for CC1 pull-up control
#define UCC1_PU_DISABLED (0)
#define UCC1_PU_56K (bUCC1_PU0_EN)
#define UCC1_PU_22K (bUCC1_PU1_EN)
#define UCC1_PU_10K (bUCC1_PU1_EN | bUCC1_PU0_EN)

#define MASK_UCC2_PU (bUCC2_PU1_EN | bUCC2_PU0_EN) // Mask for CC2 pull-up control
#define UCC2_PU_DISABLED (0)
#define UCC2_PU_56K (bUCC2_PU0_EN)
#define UCC2_PU_22K (bUCC2_PU1_EN)
#define UCC2_PU_10K (bUCC2_PU1_EN | bUCC2_PU0_EN)

#define USB_INT_FG_ADDR (0xd8)
__sfr __at(USB_INT_FG_ADDR) USB_INT_FG;         // USB interrupt flag
__sbit __at(USB_INT_FG_ADDR | 7) U_IS_NAK;      // NAK response received (read-only)
__sbit __at(USB_INT_FG_ADDR | 6) U_TOG_OK;      // DATA0/DATA1 synchronization flag match status (read-only)
__sbit __at(USB_INT_FG_ADDR | 5) U_SIE_FREE;    // USB processor idle (read-only)
__sbit __at(USB_INT_FG_ADDR | 4) UIF_FIFO_OV;   // FIFO overflow interrupt flag
__sbit __at(USB_INT_FG_ADDR | 2) UIF_SUSPEND;   // USB suspend or resume event interrupt flag
__sbit __at(USB_INT_FG_ADDR | 1) UIF_TRANSFER;  // USB transfer complete interrupt flag
__sbit __at(USB_INT_FG_ADDR | 0) UIF_BUS_RST;   // USB reset event interrupt flag

__sfr __at(0xd9) USB_INT_ST;        // USB interrupt status (read-only)
#define bUIS_IS_NAK (1 << 7)        // ReadOnly: indicate current USB transfer is NAK received
#define bUIS_TOG_OK (1 << 6)        // ReadOnly: indicate current USB transfer toggle is OK
#define bUIS_TOKEN1 (1 << 5)        // Token PID code bit 1
#define bUIS_TOKEN0 (1 << 4)        // Token PID code bit 0

#define MASK_UIS_ENDP (0x0f)        // Mask for the endpoint number of the current transfer (read-only)
#define MASK_UIS_TOKEN (0x30)       // Mask for the received token PID code (read-only)
#define UIS_TOKEN_OUT (0x00)
#define UIS_TOKEN_SOF (0x10)
#define UIS_TOKEN_IN (0x20)
#define UIS_TOKEN_SETUP (0x30)

__sfr __at(0xda) USB_MIS_ST;        // USB miscellaneous status (read-only)
#define bUMS_SIE_FREE (1 << 5)      // USB protocol processor idle (read-only)
#define bUMS_R_FIFO_RDY (1 << 4)    // USB receive FIFO data ready (read-only)
#define bUMS_BUS_RESET (1 << 3)     // USB bus reset (read-only)
#define bUMS_SUSPEND (1 << 2)       // USB suspend requested (read-only)

__sfr __at(0xdb) USB_RX_LEN;        // Number of bytes received from the endpoint

__sfr __at(0xe1) USB_INT_EN;         // USB interrupt enable
#define bUIE_DEV_SOF (1 << 7)       // Enable SOF interrupt
#define bUIE_DEV_NAK (1 << 6)       // Enable NAK interrupt
#define bUIE_FIFO_OV (1 << 4)       // Enable FIFO overflow interrupt
#define bUIE_SUSPEND (1 << 2)       // Enable USB suspend or resume interrupt
#define bUIE_TRANSFER (1 << 1)      // Enable USB transfer complete interrupt
#define bUIE_BUS_RST (1 << 0)       // Enable USB reset interrupt

__sfr __at(0xe2) USB_CTRL;          // USB control register
#define bUC_LOW_SPEED (1 << 6)      // Select full-speed or low-speed USB mode
#define bUC_DEV_PU_EN (1 << 5)      // USB device enable and internal pull-up enable (alias for bUC_SYS_CTRL1)
#define bUC_SYS_CTRL1 (1 << 5)      // USB system control high bit
#define bUC_SYS_CTRL0 (1 << 4)      // USB system control low bit
#define bUC_INT_BUSY (1 << 3)       // Automatically respond with NAK until UIF_TRANSFER is cleared
#define bUC_RESET_SIE (1 << 2)      // Reset the USB protocol processor
#define bUC_CLR_ALL (1 << 1)        // Clear USB FIFO and USB interrupt flags
#define bUC_DMA_EN (1 << 0)         // USB DMA and DMA interrupt enable

#define MASK_UC_SYS_CTRL (bUC_SYS_CTRL1 | bUC_SYS_CTRL0)    // Mask for USB system control
#define UC_SYS_CTRL_DISABLED (0)                            // Disable USB device function, turn off internal pull-up
#define UC_SYS_CTRL_ENABLED (bUC_SYS_CTRL0)                 // Enable USB device function, turn off internal pull-up (requires external pull-up)
#define UC_SYS_CTRL_ENABLED_PULLUP (bUC_SYS_CTRL1)          // Enable USB device function, turn on internal pull-up

__sfr __at(0xe3) USB_DEV_AD;        // USB device address
#define bUDA_GP_BIT (1 << 7)        // General purpose bit
#define MASK_USB_ADDR (0x7f)        // Mask for the 7-bit USB device address

__sfr __at(0xd1) UDEV_CTRL;         // USB device physical port control register
#define bUD_PD_DIS (1 << 7)         // Disable UDP/UDM pull-down
#define bUD_DP_PIN (1 << 5)         // UDP pin state (read-only)
#define bUD_DM_PIN (1 << 5)         // UDM pin state (read-only)
#define bUD_LOW_SPEED (1 << 2)      // Enable USB physical port low-speed mode
#define bUD_GP_BIT (1 << 1)         // General purpose bit
#define bUD_PORT_EN (1 << 0)        // USB physical port enable

__sfr __at(0xd2) UEP1_CTRL;         // Endpoint 1 control register
#define bUEP_R_TOG (1 << 7)         // RX expecting DATA0 / DATA1
#define bUEP_T_TOG (1 << 6)         // TX send DATA0 / DATA1
#define bUEP_AUTO_TOG (1 << 4)      // Automatic toggle between DATA0 and DATA1 after trasnfer complete
#define bUEP_R_RES1 (1 << 3)        // SETUP/OUT handshake response type high bit
#define bUEP_R_RES0 (1 << 2)        // SETUP/OUT handshake response type high bit
#define bUEP_T_RES1 (1 << 1)        // IN handshake response type high bit
#define bUEP_T_RES0 (1 << 0)        // IN handshake response type low bit

#define MASK_UEP_R_RES (bUEP_R_RES1 | bUEP_R_RES0) // Mask for SETUP/OUT handshake response type
#define UEP_R_RES_ACK (0)
#define UEP_R_RES_TOUT (bUEP_R_RES0)
#define UEP_R_RES_NAK (bUEP_R_RES1)
#define UEP_R_RES_STALL (bUEP_R_RES1 | bUEP_R_RES0)

#define MASK_UEP_T_RES (bUEP_T_RES1 | bUEP_T_RES0) // Mask for IN handshake response type
#define UEP_T_RES_ACK (0)
#define UEP_T_RES_TOUT (bUEP_T_RES0)
#define UEP_T_RES_NAK (bUEP_T_RES1)
#define UEP_T_RES_STALL (bUEP_T_RES1 | bUEP_T_RES0)

__sfr __at(0xd3) UEP1_T_LEN;        // Endpoint 1 transmit byte count
__sfr __at(0xd4) UEP2_CTRL;         // Endpoint 2 control register
__sfr __at(0xd5) UEP2_T_LEN;        // Endpoint 2 transmit byte count
__sfr __at(0xd6) UEP3_CTRL;         // Endpoint 3 control register
__sfr __at(0xd7) UEP3_T_LEN;        // Endpoint 3 transmit byte count
__sfr __at(0xdc) UEP0_CTRL;         // Endpoint 0 control register
__sfr __at(0xdd) UEP0_T_LEN;        // Endpoint 0 transmit byte count
__sfr __at(0xde) UEP4_CTRL;         // Endpoint 4 control register
__sfr __at(0xdf) UEP4_T_LEN;        // Endpoint 4 transmit byte count

__sfr __at(0xea) UEP4_1_MOD;        // Endpoint 4 / Endpoint 1 mode
#define bUEP1_RX_EN (1 << 7)        // Endpoint 1 OUT enable
#define bUEP1_TX_EN (1 << 6)        // Endpoint 1 IN enable
#define bUEP1_BUF_MOD (1 << 4)      // Endpoint 1 buffer mode
#define bUEP4_RX_EN (1 << 3)        // Endpoint 4 OUT enable
#define bUEP4_TX_EN (1 << 2)        // Endpoint 4 IN enable

__sfr __at(0xeb) UEP2_3_MOD;        // Endpoint 2 / Endpoint 3 mode
#define bUEP3_RX_EN (1 << 7)        // Endpoint 3 OUT enable
#define bUEP3_TX_EN (1 << 6)        // Endpoint 3 IN enable
#define bUEP3_BUF_MOD (1 << 4)      // Endpoint 3 buffer mode
#define bUEP2_RX_EN (1 << 3)        // Endpoint 2 OUT enable
#define bUEP2_TX_EN (1 << 2)        // Endpoint 2 IN enable
#define bUEP2_BUF_MOD (1 << 0)      // Endpoint 2 buffer mode

/*

Endpoint 1/2/3 buffer modes:
----------------------------
bUEPn_BUF_MOD
| bUEPn_TX_EN
| | bUEPn_RX_EN
| | |
x 0 0   Endpoint disabled, UEPn_DMA not used

0 0 1   UEPn_DMA: OUT endpoint, 64 bytes receive buffer

0 1 0   UEPn_DMA: IN endpoint, 64 bytes transmit buffer

0 1 1   UEPn_DMA: OUT endpoint, 64 bytes receive buffer
        UEPn_DMA+64: IN endpoint, 64 bytes transmit buffer

1 0 1   UEPn_DMA: OUT endpoint, 2x 64 bytes receive buffers; active buffer selected by bUEP_R_TOG

1 1 0   UEPn_DMA: IN endpoint, 2x 64 bytes transmit buffers; active buffer selected by bUEP_T_TOG

1 1 1   UEPn_DMA: OUT endpoint, 2x 64 bytes receive buffer; active buffer selected by bUEP_R_TOG
        UEPn_DMA+128: IN endpoint, 2x 64 bytes transmit buffer; active buffer selected by bUEP_T_TOG


Endpoint 0/4 buffer modes:
--------------------------
bUEP4_TX_EN
| bUEP4_RX_EN
| |
0 0     UEP0_DMA: Endpoint 0 IN/OUT, 64 bytes shared buffer

0 1     UEP0_DMA: Endpoint 0 IN/OUT, 64 bytes shared buffer
        UEP0_DMA+64: Endpoint 4 OUT, 64-byte receive buffer

1 0     UEP0_DMA: Endpoint 0 IN/OUT, 64 bytes shared buffer
        UEP0_DMA+64: Endpoint 4 IN,  64-byte transmit buffer

1 1     UEP0_DMA: Endpoint 0 IN/OUT, 64 bytes shared buffer
        UEP0_DMA+64: Endpoint 4 OUT, 64-byte receive buffer
        UEP0_DMA+128: Endpoint 4 IN, 64-byte transmit buffer


Note that according to the datasheet, if the endpoint size is less than 64 bytes
we have to reserve two additional bytes!

*/

__sfr16 __at((0xed<<8) | 0xec) UEP0_DMA;
__sfr __at(0xec) UEP0_DMA_L;        // Endpoint 0 buffer start address low byte
__sfr __at(0xed) UEP0_DMA_H;        // Endpoint 0 buffer start address high byte
__sfr16 __at((0xef<<8) | 0xee) UEP1_DMA;
__sfr __at(0xee) UEP1_DMA_L;        // Endpoint 1 buffer start address low byte
__sfr __at(0xef) UEP1_DMA_H;        // Endpoint 1 buffer start address high byte
__sfr16 __at((0xe5<<8) | 0xe4) UEP2_DMA;
__sfr __at(0xe4) UEP2_DMA_L;        // Endpoint 2 buffer start address low byte
__sfr __at(0xe5) UEP2_DMA_H;        // Endpoint 2 buffer start address high byte
__sfr16 __at((0xe7<<8) | 0xe6) UEP3_DMA;
__sfr __at(0xe6) UEP3_DMA_L;        // Endpoint 3 buffer start address low byte
__sfr __at(0xe7) UEP3_DMA_H;        // Endpoint 3 buffer start address high byte


// ***************************************************************************
// Touch-key registers
__sfr __at(0xc3) TKEY_CTRL;         // Touch-key control register
#define bTKC_IF (1 << 7)            // Touch-key timer interrupt flag (read-only)
#define bTKC_2MS (1 << 4)           // Touch-key timer 1 ms / 2 ms selection
#define bTKC_CHAN2 (1 << 2)         // Touch-key channel selection bit 2
#define bTKC_CHAN1 (1 << 1)         // Touch-key channel selection bit 1
#define bTKC_CHAN0 (1 << 0)         // Touch-key channel selection bit 0

#define MASK_TKC_CHAN (bTKC_CHAN2 | bTKC_CHAN1 | bTKC_CHAN0) // Mask for Touch-key channel selection
#define TKC_CHAN_DISABLED (0)
#define TKC_CHAN_TIN0_P10 (bTKC_CHAN0)              // Channel TIN0 / P1.0
#define TKC_CHAN_TIN1_P11 (bTKC_CHAN1)              // Channel TIN1 / P1.1
#define TKC_CHAN_TIN2_P14 (bTKC_CHAN1 | bTKC_CHAN0) // Channel TIN2 / P1.4
#define TKC_CHAN_TIN3_P15 (bTKC_CHAN2)              // Channel TIN3 / P1.5
#define TKC_CHAN_TIN4_P16 (bTKC_CHAN2 | bTKC_CHAN0) // Channel TIN4 / P1.6
#define TKC_CHAN_TIN5_P17 (bTKC_CHAN2 | bTKC_CHAN1) // Channel TIN5 / P1.7

__sfr16 __at((0xc5<<8) | 0xc4) TKEY_DAT;
__sfr __at(0xc4) TKEY_DATL;         // Touch-key data low byte (read-only)
__sfr __at(0xc5) TKEY_DATH;         // Touch-key data high byte (read-only)
#define MASK_TKEY_DATH (0x3f)       // The high byte only contains 6 valid data bits
#define bTKD_CHG (1 << 7)   80      // Current data maybe invalid


