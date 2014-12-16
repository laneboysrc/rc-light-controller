/****************************************************************************
 *
 * ROM API header file for NXP LPC800 Device Series
 *
 ***************************************************************************/
 #ifndef __LPC8xx_ROM_API_H__
#define __LPC8xx_ROM_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 * UART ROM API
 ***************************************************************************/
typedef void (* UART_CALLBK_T)(void);
typedef void *UART_HANDLE_T;

typedef struct UART_CONFIG {
    uint32_t sys_clk_in_hz;     // Sytem clock in hz.
    uint32_t baudrate_in_hz;    // Baudrate in hz
    uint8_t config;             // bit 1:0
                                //   00: 7 bits length
                                //   01: 8 bits length
                                //   others: reserved
                                // bit 3:2
                                //   00: no parity
                                //   01: reserved
                                //   10: even parity
                                //   11: odd parity
                                // bit 4
                                //   0: 1 stop bit, 1: 2 stop bits
    uint8_t sync_mod;           // bit 0:
                                //   0: async mode, 1: sync mode
                                // bit 1:
                                //   0: Un_RXD is sampled on the falling edge of SCLK
                                //   1: Un_RXD is sampled on the rising edge of SCLK
                                // bit 2:
                                //   0: Start and stop bits are transmitted as in asynchronous mode
                                //   1: Start and stop bits are not transmitted
                                // bit 3:
                                //   0: The UART is a slave on Sync mode
                                //   1: Tthe UART is a master on Sync mode
    uint16_t error_en;          // bit 0: OverrunEn
                                // bit 1: UnderrunEn
                                // bit 2: FrameErrEn
                                // bit 3: ParityErrEn
                                // bit4: RxNoiseEn
} UART_CONFIG_T;

typedef struct uart_A {
    // uart_get_line: pointer to buffer for receiving data.
    // uart_put_line: pointer to buffer for transmitting data.
    uint8_t * buffer;

    // [IN]: Size of buffer.
    // [OUT]: Number of bytes transmitted/received.
    uint32_t size;

    // 0x00:
    //  Transfer without termination.
    // 0x01:
    //  uart_get_line: Stop transfer when <CR><LF> are received.
    //  uart_put_line: Transfer is stopped after reaching \0. <CR><LF> is sent
    //      out after that.
    // 0x02:
    //  uart_get_line: Stop transfer when <LF> is received.
    //  uart_put_line: Transfer is stopped after reaching \0. <LF> is sent out
    //      after that.
    // 0x03:
    //  uart_get_line: RESERVED.
    //  uart_put_line: Transfer is stopped after reaching \0.
    uint16_t transfer_mode;

    // 0x00: Polling mode; function is blocked until transfer is finished.
    // 0x01: Interrupt mode; function exits immediately, callback function
    //   is invoked when transfer is finished.
    uint16_t driver_mode;

    // Pointer to callback function
    UART_CALLBK_T callback_func_pt;
} UART_PARAM_T;

typedef struct UARTD_API {
    uint32_t (* uart_get_mem_size)(void);
    UART_HANDLE_T (* uart_setup)(uint32_t base_addr, uint8_t *ram);
    uint32_t (* uart_init)(UART_HANDLE_T handle, UART_CONFIG_T *set);
    uint8_t (* uart_get_char)(UART_HANDLE_T handle);
    void (* uart_put_char)(UART_HANDLE_T handle, uint8_t data);
    uint32_t (* uart_get_line)(UART_HANDLE_T handle, UART_PARAM_T *param);
    uint32_t (* uart_put_line)(UART_HANDLE_T handle, UART_PARAM_T *param);
    void (* uart_isr)(UART_HANDLE_T handle);
} UARTD_API_T;


/****************************************************************************
 * Power profile ROM API
 ***************************************************************************/
typedef struct _PWRD {
    void (* set_pll)(unsigned int cmd[], unsigned int resp[]);
    void (* set_power)(unsigned int cmd[], unsigned int resp[]);
} PWRD_API_T ;


/****************************************************************************
 * I2C bus ROM API
 ***************************************************************************/
typedef void *I2C_HANDLE_T;
typedef void (* I2C_CALLBK_T) (uint32_t err_code, uint32_t n);

typedef enum {
    LPC_OK = 0,     // Value returned on success
    ERROR,
    ERR_I2C_BASE = 0x00060000,
    ERR_I2C_NAK,
    ERR_I2C_BUFFER_OVERFLOW,
    ERR_I2C_BYTE_COUNT_ERR,
    ERR_I2C_LOSS_OF_ARBRITRATION,
    ERR_I2C_SLAVE_NOT_ADDRESSED,
    ERR_I2C_LOSS_OF_ARBRITRATION_NAK_BIT,
    ERR_I2C_GENERAL_FAILURE,
    ERR_I2C_REGS_SET_TO_DEFAULT
} ErrorCode_t;

typedef enum I2C_mode {
    IDLE,
    MASTER_SEND,
    MASTER_RECEIVE,
    SLAVE_SEND,
    SLAVE_RECEIVE
} I2C_MODE_T ;

typedef struct i2c_R {
    uint32_t n_bytes_sent;
    uint32_t n_bytes_recieved;
} I2C_RESULT;

typedef struct i2c_A {
    uint32_t num_bytes_send;
    uint32_t num_bytes_rec;
    uint8_t *buffer_ptr_send;
    uint8_t *buffer_ptr_rec;
    I2C_CALLBK_T func_pt;
    uint8_t stop_flag;
    uint8_t dummy[3];       // required for word alignment
} I2C_PARAM;

typedef struct I2CD_API {
    void (* i2c_isr_handler) (I2C_HANDLE_T *h_i2c);

    ErrorCode_t (* i2c_master_transmit_poll)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_master_receive_poll)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_master_tx_rx_poll)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_master_transmit_intr)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_master_receive_intr)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_master_tx_rx_intr)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr) ;

    ErrorCode_t (* i2c_slave_receive_poll)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_slave_transmit_poll)(I2C_HANDLE_T  h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_slave_receive_intr)(I2C_HANDLE_T *h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_slave_transmit_intr)(I2C_HANDLE_T * h_i2c,
        I2C_PARAM *ptp, I2C_RESULT *ptr);
    ErrorCode_t (* i2c_set_slave_addr)(I2C_HANDLE_T *h_i2c,
        uint32_t slave_addr_0_3, uint32_t slave_mask_0_3);

    uint32_t (* i2c_get_mem_size)(void);
    I2C_HANDLE_T *(* i2c_setup)(uint32_t i2c_base_addr, uint32_t *start_of_ram);
    ErrorCode_t (* i2c_set_bitrate)(I2C_HANDLE_T *h_i2c, uint32_t P_clk_in_hz,
        uint32_t bitrate_in_bps);
    uint32_t (* i2c_get_firmware_version)();
    I2C_MODE_T (* i2c_get_status)(I2C_HANDLE_T *h_i2c);
} I2CD_API_T ;


/****************************************************************************
 * ROM API table
 ***************************************************************************/
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
#define LPC_PWRD_API ((PWRD_API_T *) ((*(ROM_API_T * *) (ROM_DRIVER_BASE))->pPWRD))
#define LPC_I2CD_API ((I2CD_API_T *) ((*(ROM_API_T * *) (ROM_DRIVER_BASE))->pI2CD))
#define LPC_UART_API ((UARTD_API_T *) ((*(ROM_API_T * *) (ROM_DRIVER_BASE))->pUARTD))


/****************************************************************************
 * In-Application Programming of the Flash memory
 ***************************************************************************/
#define IAP_LOCATION 0x1FFF1FF1
typedef void (* IAP)(unsigned int [], unsigned int[]);
const IAP iap_entry = (IAP) IAP_LOCATION;


#ifdef __cplusplus
}
#endif

#endif  /* __LPC8xx_ROM_API_H__ */
