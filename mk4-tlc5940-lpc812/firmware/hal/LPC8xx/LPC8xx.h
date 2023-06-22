/****************************************************************************
 *   $Id:: LPC8xx.h 6437 2012-10-31 11:06:06Z dep00694                     $
 *   Project: NXP LPC8xx software example
 *
 *   Description:
 *     CMSIS Cortex-M0+ Core Peripheral Access Layer Header File for
 *     NXP LPC800 Device Series
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.

 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors'
 * relevant copyright in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers. This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
****************************************************************************/
#ifndef __LPC8xx_H__
#define __LPC8xx_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup LPC8xx_Definitions LPC8xx Definitions
  This file defines all structures and symbols for LPC8xx:
    - Registers and bitfields
    - peripheral base address
    - PIO definitions
  @{
*/


/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup LPC8xx_CMSIS LPC8xx CMSIS Definitions
  Configuration of the Cortex-M0+ Processor and Core Peripherals
  @{
*/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */
typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/
  Reset_IRQn                    = -15,    /*!< 1 Reset Vector, invoked on Power up and warm reset*/
  NonMaskableInt_IRQn           = -14,    /*!< 2 Non Maskable Interrupt                           */
  HardFault_IRQn                = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                   */
  SVCall_IRQn                   = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                     */
  PendSV_IRQn                   = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                     */
  SysTick_IRQn                  = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                 */

/******  LPC8xx Specific Interrupt Numbers ********************************************************/
  SPI0_IRQn                     = 0,        /*!< SPI0                                             */
  SPI1_IRQn                     = 1,        /*!< SPI1                                             */
  Reserved0_IRQn                = 2,        /*!< Reserved Interrupt                               */
  UART0_IRQn                    = 3,        /*!< USART0                                            */
  UART1_IRQn                    = 4,        /*!< USART1                                            */
  UART2_IRQn                    = 5,        /*!< USART2                                            */
  Reserved1_IRQn                = 6,        /*!< Reserved Interrupt                               */
  Reserved2_IRQn                = 7,        /*!< Reserved Interrupt                               */
  I2C_IRQn                      = 8,        /*!< I2C                                              */
  SCT_IRQn                      = 9,        /*!< SCT                                              */
  MRT_IRQn                      = 10,       /*!< MRT                                              */
  CMP_IRQn                      = 11,       /*!< CMP                                              */
  WDT_IRQn                      = 12,      /*!< WDT                                              */
  BOD_IRQn                      = 13,       /*!< BOD                                              */
  Reserved3_IRQn                = 14,       /*!< Reserved Interrupt                               */
  WKT_IRQn                      = 15,       /*!< WKT Interrupt                                    */
  Reserved4_IRQn                = 16,       /*!< Reserved Interrupt                               */
  Reserved5_IRQn                = 17,       /*!< Reserved Interrupt                               */
  Reserved6_IRQn                = 18,       /*!< Reserved Interrupt                               */
  Reserved7_IRQn                = 19,       /*!< Reserved Interrupt                               */
  Reserved8_IRQn                = 20,       /*!< Reserved Interrupt                               */
  Reserved9_IRQn                = 21,       /*!< Reserved Interrupt                               */
  Reserved10_IRQn               = 22,       /*!< Reserved Interrupt                               */
  Reserved11_IRQn               = 23,       /*!< Reserved Interrupt                               */
  PININT0_IRQn               	  = 24,       /*!< External Interrupt 0                             */
  PININT1_IRQn                  = 25,       /*!< External Interrupt 1                             */
  PININT2_IRQn                  = 26,       /*!< External Interrupt 2                             */
  PININT3_IRQn                  = 27,       /*!< External Interrupt 3                             */
  PININT4_IRQn                  = 28,       /*!< External Interrupt 4                             */
  PININT5_IRQn                  = 29,       /*!< External Interrupt 5                             */
  PININT6_IRQn                  = 30,       /*!< External Interrupt 6                             */
  PININT7_IRQn                  = 31,       /*!< External Interrupt 7                             */
} IRQn_Type;

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0+ Processor and Core Peripherals */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */
#define __VTOR_PRESENT            1         /**< Defines if an VTOR is present or not */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */

/*@}*/ /* end of group LPC8xx_CMSIS */


#include "core_cm0plus.h"                  /* Cortex-M0+ processor and core peripherals          */
#include "system_LPC8xx.h"                 /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral Registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SYSCON) --------------------------------------*/
/** @addtogroup LPC8xx_SYSCON LPC8xx System Control Block
  @{
*/
typedef struct
{
  __IO uint32_t SYSMEMREMAP;            /*!< Offset: 0x000 System memory remap (R/W) */
  __IO uint32_t PRESETCTRL;             /*!< Offset: 0x004 Peripheral reset control (R/W) */
  __IO uint32_t SYSPLLCTRL;             /*!< Offset: 0x008 System PLL control (R/W) */
  __IO uint32_t SYSPLLSTAT;             /*!< Offset: 0x00C System PLL status (R/W ) */
       uint32_t RESERVED0[4];

  __IO uint32_t SYSOSCCTRL;             /*!< Offset: 0x020 System oscillator control (R/W) */
  __IO uint32_t WDTOSCCTRL;             /*!< Offset: 0x024 Watchdog oscillator control (R/W) */
       uint32_t RESERVED1[2];
  __IO uint32_t SYSRSTSTAT;             /*!< Offset: 0x030 System reset status Register (R/W ) */
       uint32_t RESERVED2[3];
  __IO uint32_t SYSPLLCLKSEL;           /*!< Offset: 0x040 System PLL clock source select (R/W) */
  __IO uint32_t SYSPLLCLKUEN;           /*!< Offset: 0x044 System PLL clock source update enable (R/W) */
       uint32_t RESERVED3[10];

  __IO uint32_t MAINCLKSEL;             /*!< Offset: 0x070 Main clock source select (R/W) */
  __IO uint32_t MAINCLKUEN;             /*!< Offset: 0x074 Main clock source update enable (R/W) */
  __IO uint32_t SYSAHBCLKDIV;           /*!< Offset: 0x078 System AHB clock divider (R/W) */
       uint32_t RESERVED4[1];

  __IO uint32_t SYSAHBCLKCTRL;          /*!< Offset: 0x080 System AHB clock control (R/W) */
       uint32_t RESERVED5[4];
  __IO uint32_t UARTCLKDIV;             /*!< Offset: 0x094 UART clock divider (R/W) */
       uint32_t RESERVED6[18];

  __IO uint32_t CLKOUTSEL;              /*!< Offset: 0x0E0 CLKOUT clock source select (R/W) */
  __IO uint32_t CLKOUTUEN;              /*!< Offset: 0x0E4 CLKOUT clock source update enable (R/W) */
  __IO uint32_t CLKOUTDIV;              /*!< Offset: 0x0E8 CLKOUT clock divider (R/W) */
       uint32_t RESERVED7;
  __IO uint32_t UARTFRGDIV;             /*!< Offset: 0x0F0 UART fractional divider SUB(R/W) */
  __IO uint32_t UARTFRGMULT;             /*!< Offset: 0x0F4 UART fractional divider ADD(R/W) */
       uint32_t RESERVED8[1];
  __IO uint32_t EXTTRACECMD;            /*!< (@ 0x400480FC) External trace buffer command register  */
  __IO uint32_t PIOPORCAP0;             /*!< Offset: 0x100 POR captured PIO status 0 (R/ ) */
       uint32_t RESERVED9[12];
  __IO uint32_t IOCONCLKDIV[7];       /*!< (@0x40048134-14C) Peripheral clock x to the IOCON block for programmable glitch filter */
  __IO uint32_t BODCTRL;                /*!< Offset: 0x150 BOD control (R/W) */
  __IO uint32_t SYSTCKCAL;              /*!< Offset: 0x154 System tick counter calibration (R/W) */
       uint32_t RESERVED10[6];
  __IO uint32_t IRQLATENCY;             /*!< (@ 0x40048170) IRQ delay */
  __IO uint32_t NMISRC;                 /*!< (@ 0x40048174) NMI Source Control     */
  __IO uint32_t PINTSEL[8];             /*!< (@ 0x40048178) GPIO Pin Interrupt Select register 0 */
       uint32_t RESERVED11[27];
  __IO uint32_t STARTERP0;              /*!< Offset: 0x204 Start logic signal enable Register 0 (R/W) */
       uint32_t RESERVED12[3];
  __IO uint32_t STARTERP1;              /*!< Offset: 0x214 Start logic signal enable Register 0 (R/W) */
       uint32_t RESERVED13[6];
  __IO uint32_t PDSLEEPCFG;             /*!< Offset: 0x230 Power-down states in Deep-sleep mode (R/W) */
  __IO uint32_t PDAWAKECFG;             /*!< Offset: 0x234 Power-down states after wake-up (R/W) */
  __IO uint32_t PDRUNCFG;               /*!< Offset: 0x238 Power-down configuration Register (R/W) */
       uint32_t RESERVED14[111];
  __I  uint32_t DEVICE_ID;              /*!< Offset: 0x3F8 Device ID (R/ ) */
} LPC_SYSCON_TypeDef;
/*@}*/ /* end of group LPC8xx_SYSCON */


/**
  * @brief Product name title=UM10462 Chapter title=LPC8xx I/O configuration Modification date=3/16/2011 Major revision=0 Minor revision=3  (IOCONFIG)
  */

typedef struct {                            /*!< (@ 0x40044000) IOCONFIG Structure     */
  __IO uint32_t PIO0_17;                    /*!< (@ 0x40044000) I/O configuration for pin PIO0_17 */
  __IO uint32_t PIO0_13;                    /*!< (@ 0x40044004) I/O configuration for pin PIO0_13 */
  __IO uint32_t PIO0_12;                    /*!< (@ 0x40044008) I/O configuration for pin PIO0_12 */
  __IO uint32_t PIO0_5;                     /*!< (@ 0x4004400C) I/O configuration for pin PIO0_5 */
  __IO uint32_t PIO0_4;                     /*!< (@ 0x40044010) I/O configuration for pin PIO0_4 */
  __IO uint32_t PIO0_3;                     /*!< (@ 0x40044014) I/O configuration for pin PIO0_3 */
  __IO uint32_t PIO0_2;                     /*!< (@ 0x40044018) I/O configuration for pin PIO0_2 */
  __IO uint32_t PIO0_11;                    /*!< (@ 0x4004401C) I/O configuration for pin PIO0_11 */
  __IO uint32_t PIO0_10;                    /*!< (@ 0x40044020) I/O configuration for pin PIO0_10 */
  __IO uint32_t PIO0_16;                    /*!< (@ 0x40044024) I/O configuration for pin PIO0_16 */
  __IO uint32_t PIO0_15;                    /*!< (@ 0x40044028) I/O configuration for pin PIO0_15 */
  __IO uint32_t PIO0_1;                     /*!< (@ 0x4004402C) I/O configuration for pin PIO0_1 */
  __IO uint32_t Reserved;                   /*!< (@ 0x40044030) I/O configuration for pin (Reserved) */
  __IO uint32_t PIO0_9;                     /*!< (@ 0x40044034) I/O configuration for pin PIO0_9 */
  __IO uint32_t PIO0_8;                     /*!< (@ 0x40044038) I/O configuration for pin PIO0_8 */
  __IO uint32_t PIO0_7;                     /*!< (@ 0x4004403C) I/O configuration for pin PIO0_7 */
  __IO uint32_t PIO0_6;                     /*!< (@ 0x40044040) I/O configuration for pin PIO0_6 */
  __IO uint32_t PIO0_0;                     /*!< (@ 0x40044044) I/O configuration for pin PIO0_0 */
  __IO uint32_t PIO0_14;                    /*!< (@ 0x40044048) I/O configuration for pin PIO0_14 */
} LPC_IOCON_TypeDef;
/*@}*/ /* end of group LPC8xx_IOCON */

/**
  * @brief Product name title=UM10462 Chapter title=LPC8xx Flash programming firmware Major revision=0 Minor revision=3  (FLASHCTRL)
  */
typedef struct {                            /*!< (@ 0x40040000) FLASHCTRL Structure    */
  __I  uint32_t  RESERVED0[4];
  __IO uint32_t  FLASHCFG;                          /*!< (@ 0x40040010) Flash configuration register                           */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  FMSSTART;                          /*!< (@ 0x40040020) Signature start address register                       */
  __IO uint32_t  FMSSTOP;                           /*!< (@ 0x40040024) Signature stop-address register                        */
  __I  uint32_t  RESERVED2;
  __I  uint32_t  FMSW0;
} LPC_FLASHCTRL_TypeDef;
/*@}*/ /* end of group LPC8xx_FLASHCTRL */


/*------------- Power Management Unit (PMU) --------------------------*/
/** @addtogroup LPC8xx_PMU LPC8xx Power Management Unit
  @{
*/
typedef struct
{
  __IO uint32_t PCON;                   /*!< Offset: 0x000 Power control Register (R/W) */
  __IO uint32_t GPREG0;                 /*!< Offset: 0x004 General purpose Register 0 (R/W) */
  __IO uint32_t GPREG1;                 /*!< Offset: 0x008 General purpose Register 1 (R/W) */
  __IO uint32_t GPREG2;                 /*!< Offset: 0x00C General purpose Register 2 (R/W) */
  __IO uint32_t GPREG3;                 /*!< Offset: 0x010 General purpose Register 3 (R/W) */
  __IO uint32_t DPDCTRL;                /*!< Offset: 0x014 Deep power-down control register (R/W) */
} LPC_PMU_TypeDef;
/*@}*/ /* end of group LPC8xx_PMU */


/*------------- Switch Matrix Port --------------------------*/
/** @addtogroup LPC8xx_SWM LPC8xx Switch Matrix Port
  @{
*/
typedef struct
{
  union {
    __IO uint32_t PINASSIGN[9];
    struct {
      __IO uint32_t PINASSIGN0;
      __IO uint32_t PINASSIGN1;
      __IO uint32_t PINASSIGN2;
      __IO uint32_t PINASSIGN3;
      __IO uint32_t PINASSIGN4;
      __IO uint32_t PINASSIGN5;
      __IO uint32_t PINASSIGN6;
      __IO uint32_t PINASSIGN7;
      __IO uint32_t PINASSIGN8;
    };
  };
  __I  uint32_t  RESERVED0[103];
  __IO uint32_t  PINENABLE0;
} LPC_SWM_TypeDef;
/*@}*/ /* end of group LPC8xx_SWM */


// ------------------------------------------------------------------------------------------------
// -----                                       GPIO_PORT                                      -----
// ------------------------------------------------------------------------------------------------

/**
  * @brief Product name title=UM10462 Chapter title=LPC8xx GPIO Modification date=3/17/2011 Major revision=0 Minor revision=3  (GPIO_PORT)
  */

typedef struct {
  __IO uint8_t B0[18];                   /*!< (@ 0xA0000000) Byte pin registers port 0 */
  __I  uint16_t RESERVED0[2039];
//  __IO uint32_t W0[18];                  /*!< (@ 0xA0001000) Word pin registers port 0 */
//       uint32_t RESERVED1[1006];
// WLA: for compatibility with LPC832 declare all 32 port pins
  __IO uint32_t W0[32];                  /*!< (@ 0xA0001000) Word pin registers port 0 */
       uint32_t RESERVED1[992];
  __IO uint32_t DIR0;                          /* 0x2000 */
       uint32_t RESERVED2[31];
  __IO uint32_t MASK0;                                  /* 0x2080 */
       uint32_t RESERVED3[31];
  __IO uint32_t PIN0;                          /* 0x2100 */
       uint32_t RESERVED4[31];
  __IO uint32_t MPIN0;                                   /* 0x2180 */
       uint32_t RESERVED5[31];
  __IO uint32_t SET0;                         /* 0x2200 */
       uint32_t RESERVED6[31];
  __O  uint32_t CLR0;                         /* 0x2280 */
       uint32_t RESERVED7[31];
  __O  uint32_t NOT0;                                    /* 0x2300 */

} LPC_GPIO_PORT_TypeDef;


// ------------------------------------------------------------------------------------------------
// -----                                     PIN_INT                                     -----
// ------------------------------------------------------------------------------------------------

/**
  * @brief Product name title=UM10462 Chapter title=LPC8xx GPIO Modification date=3/17/2011 Major revision=0 Minor revision=3  (PIN_INT)
  */

typedef struct {                            /*!< (@ 0xA0004000) PIN_INT Structure */
  __IO uint32_t ISEL;                       /*!< (@ 0xA0004000) Pin Interrupt Mode register */
  __IO uint32_t IENR;                       /*!< (@ 0xA0004004) Pin Interrupt Enable (Rising) register */
  __IO uint32_t SIENR;                      /*!< (@ 0xA0004008) Set Pin Interrupt Enable (Rising) register */
  __IO uint32_t CIENR;                      /*!< (@ 0xA000400C) Clear Pin Interrupt Enable (Rising) register */
  __IO uint32_t IENF;                       /*!< (@ 0xA0004010) Pin Interrupt Enable Falling Edge / Active Level register */
  __IO uint32_t SIENF;                      /*!< (@ 0xA0004014) Set Pin Interrupt Enable Falling Edge / Active Level register */
  __IO uint32_t CIENF;                      /*!< (@ 0xA0004018) Clear Pin Interrupt Enable Falling Edge / Active Level address */
  __IO uint32_t RISE;                       /*!< (@ 0xA000401C) Pin Interrupt Rising Edge register */
  __IO uint32_t FALL;                       /*!< (@ 0xA0004020) Pin Interrupt Falling Edge register */
  __IO uint32_t IST;                        /*!< (@ 0xA0004024) Pin Interrupt Status register */
  __IO uint32_t PMCTRL;                     /*!< (@ 0xA0004028) GPIO pattern match interrupt control register          */
  __IO uint32_t PMSRC;                      /*!< (@ 0xA000402C) GPIO pattern match interrupt bit-slice source register */
  __IO uint32_t PMCFG;                      /*!< (@ 0xA0004030) GPIO pattern match interrupt bit slice configuration register */
} LPC_PIN_INT_TypeDef;


/*------------- CRC Engine (CRC) -----------------------------------------*/
/** @addtogroup LPC8xx_CRC
  @{
*/
typedef struct
{
  __IO uint32_t MODE;
  __IO uint32_t SEED;
  union {
  __I  uint32_t SUM;
  __O  uint32_t WR_DATA_DWORD;
  __O  uint16_t WR_DATA_WORD;
       uint16_t RESERVED_WORD;
  __O  uint8_t WR_DATA_BYTE;
       uint8_t RESERVED_BYTE[3];
  };
} LPC_CRC_TypeDef;
/*@}*/ /* end of group LPC8xx_CRC */

/*------------- Comparator (CMP) --------------------------------------------------*/
/** @addtogroup LPC8xx_CMP LPC8xx Comparator
  @{
*/
typedef struct {                            /*!< (@ 0x40024000) CMP Structure          */
  __IO uint32_t  CTRL;                      /*!< (@ 0x40024000) Comparator control register */
  __IO uint32_t  LAD;                       /*!< (@ 0x40024004) Voltage ladder register */
} LPC_CMP_TypeDef;
/*@}*/ /* end of group LPC8xx_CMP */


/*------------- Wakeup Timer (WKT) --------------------------------------------------*/
/** @addtogroup LPC8xx_WKT
  @{
*/
typedef struct {                            /*!< (@ 0x40028000) WKT Structure          */
  __IO uint32_t  CTRL;                      /*!< (@ 0x40028000) Alarm/Wakeup Timer Control register */
       uint32_t  Reserved[2];
  __IO uint32_t  COUNT;                     /*!< (@ 0x4002800C) Alarm/Wakeup TImer counter register */
} LPC_WKT_TypeDef;
/*@}*/ /* end of group LPC8xx_WKT */


/*------------- Multi-Rate Timer(MRT) --------------------------------------------------*/
typedef struct {
__IO uint32_t INTVAL;
__IO uint32_t TIMER;
__IO uint32_t CTRL;
__IO uint32_t STAT;
} MRT_Channel_cfg_Type;

typedef struct {
  MRT_Channel_cfg_Type Channel[4];
   uint32_t Reserved0[1];
  __IO uint32_t IDLE_CH;
  __IO uint32_t IRQ_FLAG;
} LPC_MRT_TypeDef;


/*------------- Universal Asynchronous Receiver Transmitter (USART) -----------*/
/** @addtogroup LPC8xx_UART LPC8xx Universal Asynchronous Receiver/Transmitter
  @{
*/
/**
  * @brief Product name title=LPC8xx MCU Chapter title=USART Modification date=4/18/2012 Major revision=0 Minor revision=9  (USART)
  */
typedef struct
{
  __IO uint32_t  CFG;								/* 0x00 */
  __IO uint32_t  CTRL;
  __IO uint32_t  STAT;
  __IO uint32_t  INTENSET;
  __O  uint32_t  INTENCLR;					/* 0x10 */
  __I  uint32_t  RXDATA;
  __I  uint32_t  RXDATA_STAT;
  __IO uint32_t  TXDATA;
  __IO uint32_t  BRG;								/* 0x20 */
  __IO uint32_t  INTSTAT;
} LPC_USART_TypeDef;

/*@}*/ /* end of group LPC8xx_USART */


/*------------- Synchronous Serial Interface Controller (SPI) -----------------------*/
/** @addtogroup LPC8xx_SPI LPC8xx Synchronous Serial Port
  @{
*/
typedef struct
{
  __IO uint32_t  CFG;			    /* 0x00 */
  __IO uint32_t  DLY;
  __IO uint32_t  STAT;
  __IO uint32_t  INTENSET;
  __O  uint32_t  INTENCLR;		/* 0x10 */
  __I  uint32_t  RXDAT;
  __IO uint32_t  TXDATCTL;
  __IO uint32_t  TXDAT;
  __IO uint32_t  TXCTRL;		  /* 0x20 */
  __IO uint32_t  DIV;
  __I  uint32_t  INTSTAT;
} LPC_SPI_TypeDef;
/*@}*/ /* end of group LPC8xx_SPI */


/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
/** @addtogroup LPC8xx_I2C I2C-Bus Interface
  @{
*/
typedef struct
{
  __IO uint32_t  CFG;			  /* 0x00 */
  __IO uint32_t  STAT;
  __IO uint32_t  INTENSET;
  __O  uint32_t  INTENCLR;
  __IO uint32_t  TIMEOUT;		/* 0x10 */
  __IO uint32_t  DIV;
  __IO uint32_t  INTSTAT;
       uint32_t  Reserved0[1];
  __IO uint32_t  MSTCTL;			  /* 0x20 */
  __IO uint32_t  MSTTIME;
  __IO uint32_t  MSTDAT;
       uint32_t  Reserved1[5];
  __IO uint32_t  SLVCTL;			  /* 0x40 */
  __IO uint32_t  SLVDAT;
  __IO uint32_t  SLVADR0;
  __IO uint32_t  SLVADR1;
  __IO uint32_t  SLVADR2;			  /* 0x50 */
  __IO uint32_t  SLVADR3;
  __IO uint32_t  SLVQUAL0;
       uint32_t  Reserved2[9];
  __I  uint32_t  MONRXDAT;			/* 0x80 */
} LPC_I2C_TypeDef;

/*@}*/ /* end of group LPC8xx_I2C */

/**
  * @brief State Configurable Timer (SCT) (SCT)
  */

/**
  * @brief Product name title=UM10430 Chapter title=LPC8xx State Configurable Timer (SCT) Modification date=1/18/2011 Major revision=0 Minor revision=7  (SCT)
  */

#define CONFIG_SCT_nEV   (6)             /* Number of events */
#define CONFIG_SCT_nRG   (5)             /* Number of match/compare registers */
#define CONFIG_SCT_nOU   (4)             /* Number of outputs */

typedef struct
{
    __IO  uint32_t CONFIG;              /* 0x000 Configuration Register */
    union {
        __IO uint32_t CTRL_U;           /* 0x004 Control Register */
        struct {
            __IO uint16_t CTRL_L;       /* 0x004 low control register */
            __IO uint16_t CTRL_H;       /* 0x006 high control register */
        };
    };
    __IO uint16_t LIMIT_L;              /* 0x008 limit register for counter L */
    __IO uint16_t LIMIT_H;              /* 0x00A limit register for counter H */
    __IO uint16_t HALT_L;               /* 0x00C halt register for counter L */
    __IO uint16_t HALT_H;               /* 0x00E halt register for counter H */
    __IO uint16_t STOP_L;               /* 0x010 stop register for counter L */
    __IO uint16_t STOP_H;               /* 0x012 stop register for counter H */
    __IO uint16_t START_L;              /* 0x014 start register for counter L */
    __IO uint16_t START_H;              /* 0x016 start register for counter H */
         uint32_t RESERVED1[10];        /* 0x018-0x03C reserved */
    union {
        __IO uint32_t COUNT_U;          /* 0x040 counter register */
        struct {
            __IO uint16_t COUNT_L;      /* 0x040 counter register for counter L */
            __IO uint16_t COUNT_H;      /* 0x042 counter register for counter H */
        };
    };
    __IO uint16_t STATE_L;              /* 0x044 state register for counter L */
    __IO uint16_t STATE_H;              /* 0x046 state register for counter H */
    __I  uint32_t INPUT;                /* 0x048 input register */
    __IO uint16_t REGMODE_L;            /* 0x04C match - capture registers mode register L */
    __IO uint16_t REGMODE_H;            /* 0x04E match - capture registers mode register H */
    __IO uint32_t OUTPUT;               /* 0x050 output register */
    __IO uint32_t OUTPUTDIRCTRL;        /* 0x054 Output counter direction Control Register */
    __IO uint32_t RES;                  /* 0x058 conflict resolution register */
         uint32_t RESERVED2[37];        /* 0x05C-0x0EC reserved */
    __IO uint32_t EVEN;                 /* 0x0F0 event enable register */
    __IO uint32_t EVFLAG;               /* 0x0F4 event flag register */
    __IO uint32_t CONEN;                /* 0x0F8 conflict enable register */
    __IO uint32_t CONFLAG;              /* 0x0FC conflict flag register */

    union {
        __IO union {                    /* 0x100-... Match / Capture value */
            uint32_t U;                 /*       SCTMATCH[i].U  Unified 32-bit register */
            struct {
                uint16_t L;             /*       SCTMATCH[i].L  Access to L value */
                uint16_t H;             /*       SCTMATCH[i].H  Access to H value */
            };
        } MATCH[CONFIG_SCT_nRG];
        __I union {
            uint32_t U;                 /*       SCTCAP[i].U  Unified 32-bit register */
            struct {
                uint16_t L;             /*       SCTCAP[i].L  Access to H value */
                uint16_t H;             /*       SCTCAP[i].H  Access to H value */
            };
        } CAP[CONFIG_SCT_nRG];
    };


         uint32_t RESERVED3[32-CONFIG_SCT_nRG];      /* ...-0x17C reserved */

    union {
        __IO uint16_t MATCH_L[CONFIG_SCT_nRG];       /* 0x180-... Match Value L counter */
        __I  uint16_t CAP_L[CONFIG_SCT_nRG];         /* 0x180-... Capture Value L counter */
    };
         uint16_t RESERVED4[32-CONFIG_SCT_nRG];      /* ...-0x1BE reserved */
    union {
        __IO uint16_t MATCH_H[CONFIG_SCT_nRG];       /* 0x1C0-... Match Value H counter */
        __I  uint16_t CAP_H[CONFIG_SCT_nRG];         /* 0x1C0-... Capture Value H counter */
    };

         uint16_t RESERVED5[32-CONFIG_SCT_nRG];      /* ...-0x1FE reserved */


    union {
        __IO union {                    /* 0x200-... Match Reload / Capture Control value */
            uint32_t U;                 /*       SCTMATCHREL[i].U  Unified 32-bit register */
            struct {
                uint16_t L;             /*       SCTMATCHREL[i].L  Access to L value */
                uint16_t H;             /*       SCTMATCHREL[i].H  Access to H value */
            };
        } MATCHREL[CONFIG_SCT_nRG];
        __IO union {
            uint32_t U;                 /*       SCTCAPCTRL[i].U  Unified 32-bit register */
            struct {
                uint16_t L;             /*       SCTCAPCTRL[i].L  Access to H value */
                uint16_t H;             /*       SCTCAPCTRL[i].H  Access to H value */
            };
        } CAPCTRL[CONFIG_SCT_nRG];
    };

         uint32_t RESERVED6[32-CONFIG_SCT_nRG];      /* ...-0x27C reserved */

    union {
        __IO uint16_t MATCHREL_L[CONFIG_SCT_nRG];    /* 0x280-... Match Reload value L counter */
        __IO uint16_t CAPCTRL_L[CONFIG_SCT_nRG];     /* 0x280-... Capture Control value L counter */
    };
         uint16_t RESERVED7[32-CONFIG_SCT_nRG];      /* ...-0x2BE reserved */
    union {
        __IO uint16_t MATCHREL_H[CONFIG_SCT_nRG];    /* 0x2C0-... Match Reload value H counter */
        __IO uint16_t CAPCTRL_H[CONFIG_SCT_nRG];     /* 0x2C0-... Capture Control value H counter */
    };
         uint16_t RESERVED8[32-CONFIG_SCT_nRG];      /* ...-0x2FE reserved */

    __IO struct {                       /* 0x300-0x3FC  SCTEVENT[i].STATE / SCTEVENT[i].CTRL*/
        uint32_t STATE;                 /* Event State Register */
        uint32_t CTRL;                  /* Event Control Register */
    } EVENT[CONFIG_SCT_nEV];

         uint32_t RESERVED9[128-2*CONFIG_SCT_nEV];   /* ...-0x4FC reserved */

    __IO struct {                       /* 0x500-0x57C  SCTOUT[i].SET / SCTOUT[i].CLR */
        uint32_t SET;                   /* Output n Set Register */
        uint32_t CLR;                   /* Output n Clear Register */
    } OUT[CONFIG_SCT_nOU];

         uint32_t RESERVED10[191-2*CONFIG_SCT_nOU];  /* ...-0x7F8 reserved */

    __I  uint32_t MODULECONTENT;        /* 0x7FC Module Content */

} LPC_SCT_TypeDef;
/*@}*/ /* end of group LPC8xx_SCT */


/*------------- Watchdog Timer (WWDT) -----------------------------------------*/
/** @addtogroup LPC8xx_WDT LPC8xx WatchDog Timer
  @{
*/
typedef struct
{
  __IO uint32_t MOD;                    /*!< Offset: 0x000 Watchdog mode register (R/W) */
  __IO uint32_t TC;                     /*!< Offset: 0x004 Watchdog timer constant register (R/W) */
  __O  uint32_t FEED;                   /*!< Offset: 0x008 Watchdog feed sequence register (W) */
  __I  uint32_t TV;                     /*!< Offset: 0x00C Watchdog timer value register (R) */
       uint32_t RESERVED;               /*!< Offset: 0x010 RESERVED                          */
  __IO uint32_t WARNINT;                /*!< Offset: 0x014 Watchdog timer warning int. register (R/W) */
  __IO uint32_t WINDOW;                 /*!< Offset: 0x018 Watchdog timer window value register (R/W) */
} LPC_WWDT_TypeDef;
/*@}*/ /* end of group LPC8xx_WDT */


#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define LPC_FLASH_BASE        (0x00000000UL)
#define LPC_RAM_BASE          (0x10000000UL)
#define LPC_ROM_BASE          (0x1FFF0000UL)
#define LPC_APB0_BASE         (0x40000000UL)
#define LPC_AHB_BASE          (0x50000000UL)

/* APB0 peripherals */
#define LPC_WWDT_BASE         (LPC_APB0_BASE + 0x00000)
#define LPC_MRT_BASE          (LPC_APB0_BASE + 0x04000)
#define LPC_WKT_BASE          (LPC_APB0_BASE + 0x08000)
#define LPC_SWM_BASE          (LPC_APB0_BASE + 0x0C000)
#define LPC_PMU_BASE          (LPC_APB0_BASE + 0x20000)
#define LPC_CMP_BASE          (LPC_APB0_BASE + 0x24000)

#define LPC_FLASHCTRL_BASE    (LPC_APB0_BASE + 0x40000)
#define LPC_IOCON_BASE        (LPC_APB0_BASE + 0x44000)
#define LPC_SYSCON_BASE       (LPC_APB0_BASE + 0x48000)
#define LPC_I2C_BASE          (LPC_APB0_BASE + 0x50000)
#define LPC_SPI0_BASE         (LPC_APB0_BASE + 0x58000)
#define LPC_SPI1_BASE         (LPC_APB0_BASE + 0x5C000)
#define LPC_USART0_BASE       (LPC_APB0_BASE + 0x64000)
#define LPC_USART1_BASE       (LPC_APB0_BASE + 0x68000)
#define LPC_USART2_BASE       (LPC_APB0_BASE + 0x6C000)

/* AHB peripherals                                                            */
#define LPC_CRC_BASE         (LPC_AHB_BASE + 0x00000)
#define LPC_SCT_BASE         (LPC_AHB_BASE + 0x04000)

#define LPC_GPIO_PORT_BASE    (0xA0000000)
#define LPC_PIN_INT_BASE     (LPC_GPIO_PORT_BASE  + 0x4000)

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define LPC_WWDT              ((LPC_WWDT_TypeDef   *) LPC_WWDT_BASE  )
#define LPC_MRT               ((LPC_MRT_TypeDef    *) LPC_MRT_BASE   )


#define LPC_WKT               ((LPC_WKT_TypeDef    *) LPC_WKT_BASE   )
#define LPC_SWM               ((LPC_SWM_TypeDef    *) LPC_SWM_BASE   )
#define LPC_PMU               ((LPC_PMU_TypeDef    *) LPC_PMU_BASE   )
#define LPC_CMP               ((LPC_CMP_TypeDef    *) LPC_CMP_BASE   )

#define LPC_FLASHCTRL         ((LPC_FLASHCTRL_TypeDef *) LPC_FLASHCTRL_BASE )
#define LPC_IOCON             ((LPC_IOCON_TypeDef  *) LPC_IOCON_BASE )
#define LPC_SYSCON            ((LPC_SYSCON_TypeDef *) LPC_SYSCON_BASE)
#define LPC_I2C               ((LPC_I2C_TypeDef    *) LPC_I2C_BASE   )
#define LPC_SPI0              ((LPC_SPI_TypeDef    *) LPC_SPI0_BASE  )
#define LPC_SPI1              ((LPC_SPI_TypeDef    *) LPC_SPI1_BASE  )
#define LPC_USART0            ((LPC_USART_TypeDef   *) LPC_USART0_BASE )
#define LPC_USART1            ((LPC_USART_TypeDef   *) LPC_USART1_BASE )
#define LPC_USART2            ((LPC_USART_TypeDef   *) LPC_USART2_BASE )

#define LPC_CRC               ((LPC_CRC_TypeDef    *) LPC_CRC_BASE   )
#define LPC_SCT               ((LPC_SCT_TypeDef    *) LPC_SCT_BASE   )

#define LPC_GPIO_PORT         ((LPC_GPIO_PORT_TypeDef  *) LPC_GPIO_PORT_BASE  )
#define LPC_PIN_INT          ((LPC_PIN_INT_TypeDef   *) LPC_PIN_INT_BASE  )






/******************************************************************************/
/* LPC8xx DMA Controller driver
 */
/******************************************************************************/

/**
 * @brief DMA Controller shared registers structure
 */
typedef struct {                    /*!< DMA shared registers structure */
    __IO uint32_t  ENABLESET;       /*!< DMA Channel Enable read and Set for all DMA channels */
    __I  uint32_t  RESERVED0;
    __O  uint32_t  ENABLECLR;       /*!< DMA Channel Enable Clear for all DMA channels */
    __I  uint32_t  RESERVED1;
    __I  uint32_t  ACTIVE;          /*!< DMA Channel Active status for all DMA channels */
    __I  uint32_t  RESERVED2;
    __I  uint32_t  BUSY;            /*!< DMA Channel Busy status for all DMA channels */
    __I  uint32_t  RESERVED3;
    __IO uint32_t  ERRINT;          /*!< DMA Error Interrupt status for all DMA channels */
    __I  uint32_t  RESERVED4;
    __IO uint32_t  INTENSET;        /*!< DMA Interrupt Enable read and Set for all DMA channels */
    __I  uint32_t  RESERVED5;
    __O  uint32_t  INTENCLR;        /*!< DMA Interrupt Enable Clear for all DMA channels */
    __I  uint32_t  RESERVED6;
    __IO uint32_t  INTA;            /*!< DMA Interrupt A status for all DMA channels */
    __I  uint32_t  RESERVED7;
    __IO uint32_t  INTB;            /*!< DMA Interrupt B status for all DMA channels */
    __I  uint32_t  RESERVED8;
    __O  uint32_t  SETVALID;        /*!< DMA Set ValidPending control bits for all DMA channels */
    __I  uint32_t  RESERVED9;
    __O  uint32_t  SETTRIG;         /*!< DMA Set Trigger control bits for all DMA channels */
    __I  uint32_t  RESERVED10;
    __O  uint32_t  ABORT;           /*!< DMA Channel Abort control for all DMA channels */
} LPC_DMA_COMMON_T;

/**
 * @brief DMA Controller shared registers structure
 */
typedef struct {                    /*!< DMA channel register structure */
    __IO uint32_t  CFG;             /*!< DMA Configuration register */
    __I  uint32_t  CTLSTAT;         /*!< DMA Control and status register */
    __IO uint32_t  XFERCFG;         /*!< DMA Transfer configuration register */
    __I  uint32_t  RESERVED;
} LPC_DMA_CHANNEL_T;

/* Reserved bits masks... */
#define DMA_CFG_RESERVED            ((3<<2)|(1<<7)|(3<<12)|0xfffc0000)
#define DMA_CTLSTAT_RESERVED        (~(1|(1<<2)))
#define DMA_XFERCFG_RESERVED        ((3<<6)|(3<<10)|(0x3fu<<26))

/* DMA channel mapping - each channel is mapped to an individual peripheral
   and direction or a DMA imput mux trigger */
typedef enum {
    DMAREQ_USART0_RX,                   /*!< USART0 receive DMA channel */
    DMA_CH0 = DMAREQ_USART0_RX,
    DMAREQ_USART0_TX,                   /*!< USART0 transmit DMA channel */
    DMA_CH1 = DMAREQ_USART0_TX,
    DMAREQ_USART1_RX,                   /*!< USART1 receive DMA channel */
    DMA_CH2 = DMAREQ_USART1_RX,
    DMAREQ_USART1_TX,                   /*!< USART1 transmit DMA channel */
    DMA_CH3 = DMAREQ_USART1_TX,
    DMAREQ_USART2_RX,                   /*!< USART2 receive DMA channel */
    DMA_CH4 = DMAREQ_USART2_RX,
    DMAREQ_USART2_TX,                   /*!< USART2 transmit DMA channel */
    DMA_CH5 = DMAREQ_USART2_TX,
    DMAREQ_SPI0_RX,
    DMA_CH6 = DMAREQ_SPI0_RX,           /*!< SPI0 receive DMA channel */
    DMAREQ_SPI0_TX,
    DMA_CH7 = DMAREQ_SPI0_TX,           /*!< SPI0 transmit DMA channel */
    DMAREQ_SPI1_RX,
    DMA_CH8 = DMAREQ_SPI1_RX,           /*!< SPI1 receive DMA channel */
    DMAREQ_SPI1_TX,
    DMA_CH9 = DMAREQ_SPI1_TX,           /*!< SPI1 transmit DMA channel */
    DMAREQ_I2C0_MST,
    DMA_CH10 = DMAREQ_I2C0_MST,         /*!< I2C0 Master DMA channel */
    DMAREQ_I2C0_SLV,
    DMA_CH11 = DMAREQ_I2C0_SLV,         /*!< I2C0 Slave DMA channel */
    DMAREQ_I2C1_MST,
    DMA_CH12 = DMAREQ_I2C1_MST,         /*!< I2C1 Master DMA channel */
    DMAREQ_I2C1_SLV,
    DMA_CH13 = DMAREQ_I2C1_SLV,         /*!< I2C1 Slave DMA channel */
    DMAREQ_I2C2_MST,
    DMA_CH14 = DMAREQ_I2C2_MST,         /*!< I2C2 Master DMA channel */
    DMAREQ_I2C2_SLV,
    DMA_CH15 = DMAREQ_I2C2_SLV,         /*!< I2C2 Slave DMA channel */
    DMAREQ_I2C3_MST,
    DMA_CH16 = DMAREQ_I2C3_MST,         /*!< I2C2 Master DMA channel */
    DMAREQ_I2C3_SLV,
    DMA_CH17 = DMAREQ_I2C3_SLV,         /*!< I2C2 Slave DMA channel */
} DMA_CHID_T;

/* On LPC82x, Max DMA channel is 18 */
#define MAX_DMA_CHANNEL         (DMA_CH17 + 1)

/* Reserved bits masks... */
#define DMA_COMMON_RESERVED         (~(0UL) << MAX_DMA_CHANNEL)
#define DMA_ENABLESET_RESERVED      DMA_COMMON_RESERVED
#define DMA_ENABLECLR_RESERVED      DMA_COMMON_RESERVED
#define DMA_ACTIVE_RESERVED         DMA_COMMON_RESERVED
#define DMA_BUSY_RESERVED           DMA_COMMON_RESERVED
#define DMA_ERRINT_RESERVED         DMA_COMMON_RESERVED
#define DMA_INTENSET_RESERVED       DMA_COMMON_RESERVED
#define DMA_INTENCLR_RESERVED       DMA_COMMON_RESERVED
#define DMA_INTA_RESERVED           DMA_COMMON_RESERVED
#define DMA_INTB_RESERVED           DMA_COMMON_RESERVED
#define DMA_SETVALID_RESERVED       DMA_COMMON_RESERVED
#define DMA_SETTRIG_RESERVED        DMA_COMMON_RESERVED
#define DMA_ABORT_RESERVED          DMA_COMMON_RESERVED

/**
 * @brief DMA Controller register block structure
 */
typedef struct {                    /*!< DMA Structure */
    __IO uint32_t  CTRL;            /*!< DMA control register */
    __I  uint32_t  INTSTAT;         /*!< DMA Interrupt status register */
    __IO uint32_t  SRAMBASE;        /*!< DMA SRAM address of the channel configuration table */
    __I  uint32_t  RESERVED2[5];
    LPC_DMA_COMMON_T DMACOMMON[1];  /*!< DMA shared channel (common) registers */
    __I  uint32_t  RESERVED0[225];
    LPC_DMA_CHANNEL_T CHANNEL[MAX_DMA_CHANNEL];   /*!< DMA channel registers */
} LPC_DMA_T;

/* Reserved bits masks... */
#define DMA_CTRL_RESERVED           (~1)
#define DMA_INTSTAT_RESERVED        (~7)
#define DMA_SRAMBASE_RESERVED       (0xFF)

/* DMA interrupt status bits (common) */
#define DMA_INTSTAT_ACTIVEINT       0x2     /*!< Summarizes whether any enabled interrupts are pending */
#define DMA_INTSTAT_ACTIVEERRINT    0x4     /*!< Summarizes whether any error interrupts are pending */

#define LPC_DMA_BASE          (0x50008000UL)  /* Available only on LPC82x */
#define LPC_DMATIRGMUX_BASE   (0x40028000UL)  /* Available only on LPC82x */

#define LPC_DMA             ((LPC_DMA_T         *) LPC_DMA_BASE)
#define LPC_DMATRIGMUX      ((LPC_DMATRIGMUX_T  *) LPC_DMATIRGMUX_BASE)


/* DMA channel source/address/next descriptor */
typedef struct {
    uint32_t  xfercfg;      /*!< Transfer configuration (only used in linked lists and ping-pong configs) */
    uint32_t  source;       /*!< DMA transfer source end address */
    uint32_t  dest;         /*!< DMA transfer desintation end address */
    uint32_t  next;         /*!< Link to next DMA descriptor, must be 16 byte aligned */
} DMA_CHDESC_T;

/* DMA channel transfer configuration registers definitions */
#define DMA_XFERCFG_CFGVALID        (1 << 0)    /*!< Configuration Valid flag */
#define DMA_XFERCFG_RELOAD          (1 << 1)    /*!< Indicates whether the channels control structure will be reloaded when the current descriptor is exhausted */
#define DMA_XFERCFG_SWTRIG          (1 << 2)    /*!< Software Trigger */
#define DMA_XFERCFG_CLRTRIG         (1 << 3)    /*!< Clear Trigger */
#define DMA_XFERCFG_SETINTA         (1 << 4)    /*!< Set Interrupt flag A for this channel to fire when descriptor is complete */
#define DMA_XFERCFG_SETINTB         (1 << 5)    /*!< Set Interrupt flag B for this channel to fire when descriptor is complete */
#define DMA_XFERCFG_WIDTH_8         (0 << 8)    /*!< 8-bit transfers are performed */
#define DMA_XFERCFG_WIDTH_16        (1 << 8)    /*!< 16-bit transfers are performed */
#define DMA_XFERCFG_WIDTH_32        (2 << 8)    /*!< 32-bit transfers are performed */
#define DMA_XFERCFG_SRCINC_0        (0 << 12)   /*!< DMA source address is not incremented after a transfer */
#define DMA_XFERCFG_SRCINC_1        (1 << 12)   /*!< DMA source address is incremented by 1 (width) after a transfer */
#define DMA_XFERCFG_SRCINC_2        (2 << 12)   /*!< DMA source address is incremented by 2 (width) after a transfer */
#define DMA_XFERCFG_SRCINC_4        (3 << 12)   /*!< DMA source address is incremented by 4 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_0        (0 << 14)   /*!< DMA destination address is not incremented after a transfer */
#define DMA_XFERCFG_DSTINC_1        (1 << 14)   /*!< DMA destination address is incremented by 1 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_2        (2 << 14)   /*!< DMA destination address is incremented by 2 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_4        (3 << 14)   /*!< DMA destination address is incremented by 4 (width) after a transfer */
#define DMA_XFERCFG_XFERCOUNT(n)    ((n - 1) << 16) /*!< DMA transfer count in 'transfers', between (0)1 and (1023)1024 */


/* Support definitions for setting the configuration of a DMA channel. You
   will need to get more information on these options from the User manual. */
#define DMA_CFG_PERIPHREQEN     (1 << 0)    /*!< Enables Peripheral DMA requests */
#define DMA_CFG_HWTRIGEN        (1 << 1)    /*!< Use hardware triggering via imput mux */
#define DMA_CFG_TRIGPOL_LOW     (0 << 4)    /*!< Hardware trigger is active low or falling edge */
#define DMA_CFG_TRIGPOL_HIGH    (1 << 4)    /*!< Hardware trigger is active high or rising edge */
#define DMA_CFG_TRIGTYPE_EDGE   (0 << 5)    /*!< Hardware trigger is edge triggered */
#define DMA_CFG_TRIGTYPE_LEVEL  (1 << 5)    /*!< Hardware trigger is level triggered */
#define DMA_CFG_TRIGBURST_SNGL  (0 << 6)    /*!< Single transfer. Hardware trigger causes a single transfer */
#define DMA_CFG_TRIGBURST_BURST (1 << 6)    /*!< Burst transfer (see UM) */
#define DMA_CFG_BURSTPOWER_1    (0 << 8)    /*!< Set DMA burst size to 1 transfer */
#define DMA_CFG_BURSTPOWER_2    (1 << 8)    /*!< Set DMA burst size to 2 transfers */
#define DMA_CFG_BURSTPOWER_4    (2 << 8)    /*!< Set DMA burst size to 4 transfers */
#define DMA_CFG_BURSTPOWER_8    (3 << 8)    /*!< Set DMA burst size to 8 transfers */
#define DMA_CFG_BURSTPOWER_16   (4 << 8)    /*!< Set DMA burst size to 16 transfers */
#define DMA_CFG_BURSTPOWER_32   (5 << 8)    /*!< Set DMA burst size to 32 transfers */
#define DMA_CFG_BURSTPOWER_64   (6 << 8)    /*!< Set DMA burst size to 64 transfers */
#define DMA_CFG_BURSTPOWER_128  (7 << 8)    /*!< Set DMA burst size to 128 transfers */
#define DMA_CFG_BURSTPOWER_256  (8 << 8)    /*!< Set DMA burst size to 256 transfers */
#define DMA_CFG_BURSTPOWER_512  (9 << 8)    /*!< Set DMA burst size to 512 transfers */
#define DMA_CFG_BURSTPOWER_1024 (10 << 8)   /*!< Set DMA burst size to 1024 transfers */
#define DMA_CFG_BURSTPOWER(n)   ((n) << 8)  /*!< Set DMA burst size to 2^n transfers, max n=10 */
#define DMA_CFG_SRCBURSTWRAP    (1 << 14)   /*!< Source burst wrapping is enabled for this DMA channel */
#define DMA_CFG_DSTBURSTWRAP    (1 << 15)   /*!< Destination burst wrapping is enabled for this DMA channel */
#define DMA_CFG_CHPRIORITY(p)   ((p) << 16) /*!< Sets DMA channel priority, min 0 (highest), max 3 (lowest) */


/* Support macro for DMA_CHDESC_T */
#define DMA_ADDR(addr)      ((uint32_t) (addr))


#ifdef __cplusplus
}
#endif

#endif  /* __LPC8xx_H__ */
