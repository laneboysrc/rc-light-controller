#include <stdint.h>

#include <LPC8xx.h>

#include <uart0.h>

#ifndef UART0_BAUDRATE
    #define UART0_BAUDRATE 115200
#endif

// INT32_MIN  is -2147483648 (decimal needs 12 characters, incl. terminating 0)
// INT32_MAX  is 2147483647
// UINT32_MIN is 0
// UINT32_MAX is 4294967295

#define NO_LEADING_ZEROS 0

#define UART_CFG_ENABLE (1 << 0)
#define UART_CFG_DATALEN(d) (((d) - 7) << 2)
#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)

typedef void (* UART_CALLBK_T)(void);

typedef struct UART_CONFIG {
    uint32_t sys_clk_in_hz; // Sytem clock in hz.
    uint32_t baudrate_in_hz; // Baudrate in hz
    uint8_t config; //bit 1:0
                    // 00: 7 bits length, 01: 8 bits lenght, others: reserved
                    //bit3:2
                    // 00: No Parity, 01: reserved, 10: Even, 11: Odd
                    //bit4
                    // 0: 1 Stop bit, 1: 2 Stop bits
    uint8_t sync_mod; //bit0: 0(Async mode), 1(Sync mode)
                      //bit1: 0(Un_RXD is sampled on the falling edge of SCLK)
                      //    1(Un_RXD is sampled on the rising edge of SCLK)
                      //bit2: 0(Start and stop bits are transmitted as in asynchronous mode)
                      //    1(Start and stop bits are not transmitted)
                      //bit3: 0(the UART is a slave on Sync mode)
                      //    1(the UART is a master on Sync mode)
    uint16_t error_en; //Bit0: OverrunEn, bit1: UnderrunEn, bit2: FrameErrEn,
                       // bit3: ParityErrEn, bit4: RxNoiseEn
} UART_CONFIG_T ;

typedef struct uart_A { // parms passed to uart driver function
    uint8_t * buffer ; // The pointer of buffer.
    // For uart_get_line function, buffer for receiving data.
    // For uart_put_line function, buffer for transmitting data.
    uint32_t size; // [IN] The size of buffer.
    //[OUT] The number of bytes transmitted/received.
    uint16_t transfer_mode ;
    // 0x00: For uart_get_line function, transfer without
    // termination.
    // For uart_put_line function, transfer without termination.
    // 0x01: For uart_get_line function, stop transfer when
    // <CR><LF> are received.
    // For uart_put_line function, transfer is stopped after
    // reaching \0. <CR><LF> characters are sent out after that.
    // 0x02: For uart_get_line function, stop transfer when <LF>
    // is received.
    // For uart_put_line function, transfer is stopped after
    // reaching \0. A <LF> character is sent out after that.
    //0x03: For uart_get_line function, RESERVED.
    // For uart_put_line function, transfer is stopped after
    // reaching \0.
    uint16_t driver_mode;
    //0x00: Polling mode, function is blocked until transfer is
    // finished.
    // 0x01: Intr mode, function exit immediately, callback function
    // is invoked when transfer is finished.
    //0x02: RESERVED
    UART_CALLBK_T callback_func_pt; // callback function
} UART_PARAM_T ;

typedef void *UART_HANDLE_T ; // define TYPE for uart handle pointer

typedef struct UARTD_API {
    // index of all the uart driver functions
    uint32_t (*uart_get_mem_size)(void);
    UART_HANDLE_T (*uart_setup)(uint32_t base_addr, uint8_t *ram);
    uint32_t (*uart_init)(UART_HANDLE_T handle, UART_CONFIG_T *set);
    //--polling functions--//
    uint8_t (*uart_get_char)(UART_HANDLE_T handle);
    void (*uart_put_char)(UART_HANDLE_T handle, uint8_t data);
    uint32_t (*uart_get_line)(UART_HANDLE_T handle, UART_PARAM_T * param);
    uint32_t (*uart_put_line)(UART_HANDLE_T handle, UART_PARAM_T * param);
    //--interrupt functions--//
    void (*uart_isr)(UART_HANDLE_T handle);
} UARTD_API_T ;

typedef struct _PWRD {
    void (*set_pll)(unsigned int cmd[], unsigned int resp[]);
    void (*set_power)(unsigned int cmd[], unsigned int resp[]);
} PWRD_API_T ;

typedef enum
{
    LPC_OK=0, /**< enum value returned on Success */
    ERROR,
    ERR_I2C_BASE = 0x00060000,
    /*0x00060001*/ ERR_I2C_NAK=ERR_I2C_BASE+1,
    /*0x00060002*/ ERR_I2C_BUFFER_OVERFLOW,
    /*0x00060003*/ ERR_I2C_BYTE_COUNT_ERR,
    /*0x00060004*/ ERR_I2C_LOSS_OF_ARBRITRATION,
    /*0x00060005*/ ERR_I2C_SLAVE_NOT_ADDRESSED,
    /*0x00060006*/ ERR_I2C_LOSS_OF_ARBRITRATION_NAK_BIT,
    /*0x00060007*/ ERR_I2C_GENERAL_FAILURE,
    /*0x00060008*/ ERR_I2C_REGS_SET_TO_DEFAULT
} ErrorCode_t;

typedef struct i2c_R {
    // RESULTs struct--results are here when returned
    uint32_t n_bytes_sent ;
    uint32_t n_bytes_recd ;
} I2C_RESULT ;

typedef void* I2C_HANDLE_T;

typedef void (*I2C_CALLBK_T) (uint32_t err_code, uint32_t n);

typedef struct i2c_A { //parameters passed to ROM function
    uint32_t num_bytes_send ;
    uint32_t num_bytes_rec ;
    uint8_t *buffer_ptr_send ;
    uint8_t *buffer_ptr_rec ;
    I2C_CALLBK_T func_pt; // callback function pointer
    uint8_t stop_flag;
    uint8_t dummy[3] ;
    // required for word alignment
} I2C_PARAM ;

typedef enum I2C_mode {
    IDLE,
    MASTER_SEND,
    MASTER_RECEIVE,
    SLAVE_SEND,
    SLAVE_RECEIVE
} I2C_MODE_T ;

typedef struct I2CD_API {
    // index of all the i2c driver functions
    void (*i2c_isr_handler) (I2C_HANDLE_T* h_i2c) ; // ISR interrupt service request
    // MASTER functions ***
    ErrorCode_t (*i2c_master_transmit_poll)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr );
    ErrorCode_t (*i2c_master_receive_poll)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr );
    ErrorCode_t (*i2c_master_tx_rx_poll)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr ) ;
    ErrorCode_t (*i2c_master_transmit_intr)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr ) ;
    ErrorCode_t (*i2c_master_receive_intr)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr ) ;
    ErrorCode_t (*i2c_master_tx_rx_intr)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp, I2C_RESULT*
    ptr ) ;
    // SLAVE functions ***
    ErrorCode_t (*i2c_slave_receive_poll)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp, I2C_RESULT*
    ptr ) ;
    ErrorCode_t (*i2c_slave_transmit_poll)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr ) ;
    ErrorCode_t (*i2c_slave_receive_intr)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp, I2C_RESULT*
    ptr ) ;
    ErrorCode_t (*i2c_slave_transmit_intr)(I2C_HANDLE_T* h_i2c, I2C_PARAM* ptp,
    I2C_RESULT* ptr ) ;
    ErrorCode_t (*i2c_set_slave_addr)(I2C_HANDLE_T* h_i2c,
    uint32_t slave_addr_0_3, uint32_t slave_mask_0_3) ;
    // OTHER functions
    uint32_t (*i2c_get_mem_size)(void) ; //ramsize_in_bytes memory needed by I2C drivers
    I2C_HANDLE_T* (*i2c_setup)(uint32_t i2c_base_addr, uint32_t *start_of_ram ) ;
    ErrorCode_t (*i2c_set_bitrate)(I2C_HANDLE_T* h_i2c, uint32_t P_clk_in_hz,
    uint32_t bitrate_in_bps) ;
    uint32_t (*i2c_get_firmware_version)() ;
    I2C_MODE_T (*i2c_get_status)(I2C_HANDLE_T* h_i2c ) ;
} I2CD_API_T ;

typedef struct _ROM_API {
    const uint32_t unused[3];
    const PWRD_API_T *pPWRD;
    const uint32_t p_dev1;
    const I2CD_API_T *pI2CD;
    const uint32_t p_dev3;
    const uint32_t p_dev4;
    const uint32_t p_dev5;
    const UARTD_API_T *pUARTD;
} ROM_API_T;

#define ROM_DRIVER_BASE (0x1FFF1FF8UL)
#define LPC_UART_API ((UARTD_API_T *) ((*(ROM_API_T * *) (ROM_DRIVER_BASE))->pUARTD))

static uint8_t ram[40];
static UART_HANDLE_T *handle;
static UART_PARAM_T uart_param;

// ****************************************************************************
static void uint32_to_cstring(uint32_t value, char *result, int radix, int number_of_leading_zeros)
{
    // Worst case (base 2) we have to write 32 characters. However, since we
    // only support base 2 for uint8_t we can make due with 12 bytes,
    // which is the maximum needed for decimal.
    char temp[12];
    char *tp = temp;
    int digit;

    // Process the digits in reverse order, i.e. fill temp[] with the least
    // significant digit first. We stop as soon as the higher most remaining
    // digits are 0 (leading zero supression).
    do {
        digit = value % radix;
        *tp++ = (digit < 10) ? (digit + '0') : (digit + 'a' - 10);
        value /= radix;
        --number_of_leading_zeros;
    } while (value || number_of_leading_zeros > 0);

    // We write the digits to "result" in reverse order, i.e. most significant
    // digit first.
    while (tp > temp) {
        *result++ = *--tp;
    }
    *result = '\0';
}


// ****************************************************************************
static void int32_to_cstring(int32_t value, char *result, int radix)
{
    if (radix == 10  &&  value < 0) {
        *result++ = '-';
        value = -value;
    }

    return uint32_to_cstring((uint32_t)value, result, radix, NO_LEADING_ZEROS);
}


// ****************************************************************************
void init_uart0(void)
{
    UART_CONFIG_T uart_config;

    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;

    handle = LPC_UART_API->uart_setup(LPC_USART0_BASE, ram);

    uart_config.sys_clk_in_hz = __SYSTEM_CLOCK;
    uart_config.baudrate_in_hz = UART0_BAUDRATE;
    uart_config.config = (0x01 << 0) | (0x00 << 2) | (0x0 << 4);    // 8n1
    uart_config.sync_mod = (0x0 << 0);      // Async mode
    uart_config.error_en = 0;               // Ignore all errors

    LPC_SYSCON->UARTFRGMULT = LPC_UART_API->uart_init(handle, &uart_config);

    uart_param.size = 0;                // Ignore, should be \0-terminated
    uart_param.transfer_mode = 0x03;    // Stop after \0, no linefeed
    uart_param.driver_mode = 0x00;      // Polling mode
    uart_param.callback_func_pt = 0;
}


// ****************************************************************************
inline void uart0_send_char(const char c)
{
    LPC_UART_API->uart_put_char(handle, c);
}


// ****************************************************************************
void uart0_send_cstring(const char *cstring)
{
    uart_param.buffer = (uint8_t *)cstring;
    uart_param.size = 0;                // Ignore, should be \0-terminated
    LPC_UART_API->uart_put_line(handle, &uart_param);
}


// ****************************************************************************
void uart0_send_int32(int32_t number)
{
    char buf[12];
    int32_to_cstring(number, buf, 10);
    uart0_send_cstring(buf);
}

// ****************************************************************************
void uart0_send_uint32(uint32_t number)
{
    char buf[12];
    uint32_to_cstring(number, buf, 10, NO_LEADING_ZEROS);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint32_hex(uint32_t number)
{
    char buf[9];
    uint32_to_cstring(number, buf, 16, 8);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint16_hex(uint16_t number)
{
    char buf[5];
    uint32_to_cstring(number, buf, 16, 4);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint8_hex(uint8_t number)
{
    char buf[3];
    uint32_to_cstring(number, buf, 16, 4);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint8_binary(uint8_t number)
{
    char buf[9];
    uint32_to_cstring(number, buf, 2, 8);
    uart0_send_cstring(buf);
}


// ****************************************************************************
inline void uart0_send_linefeed(void)
{
    uart0_send_char('\n');
}


// ****************************************************************************
int uart0_read_is_byte_pending(void)
{
    return (LPC_USART0->STAT & UART_STAT_RXRDY) ? 1 : 0;
}


// ****************************************************************************
inline uint8_t uart0_read_byte(void)
{
    return LPC_UART_API->uart_get_char(handle);
}
