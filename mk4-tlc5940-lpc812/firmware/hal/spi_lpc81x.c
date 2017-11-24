#include <stdint.h>

#include <LPC8xx.h>
#include <hal.h>


#define GPIO_BIT_SCK 2
#define GPIO_BIT_SIN 7
#define GPIO_BIT_XLAT 3

#define GPIO_XLAT LPC_GPIO_PORT->W0[GPIO_BIT_XLAT]
#define GPIO_SCK LPC_GPIO_PORT->W0[GPIO_BIT_SCK]
#define GPIO_SIN LPC_GPIO_PORT->W0[GPIO_BIT_SIN]


void hal_spi_init(void)
{
    GPIO_XLAT = 1;

    LPC_GPIO_PORT->DIR0 |= (1 << GPIO_BIT_SCK) |
                           (1 << GPIO_BIT_XLAT) |
                           (1 << GPIO_BIT_SIN);

    // Use 2 MHz SPI clock. 16 bytes take about 50 us to transmit.
    LPC_SPI0->DIV = (__SYSTEM_CLOCK / 2000000) - 1;

    LPC_SPI0->CFG = (1 << 0) |          // Enable SPI0
                    (1 << 2) |          // Master mode
                    (0 << 3) |          // LSB First mode disabled
                    (0 << 4) |          // CPHA = 0
                    (0 << 5) |          // CPOL = 0
                    (0 << 8);           // SPOL = 0

    LPC_SPI0->TXCTRL = (1 << 21) |      // set EOF
                       (1 << 22) |      // RXIGNORE, otherwise SPI hangs until
                                        //   we read the data register
                       ((6 - 1) << 24); // 6 bit frames

    // We use the SSEL function for XLAT: low during the transmission, high
    // during the idle periood.
    LPC_SWM->PINASSIGN3 = (GPIO_BIT_SCK << 24) |        // SCK
                          (0xff << 16) |
                          (0xff << 8) |
                          (0xff << 0);

    LPC_SWM->PINASSIGN4 = (0xff << 24) |
                          (GPIO_BIT_XLAT << 16) |       // XLAT (SSEL)
                          (0xff << 8) |
                          (GPIO_BIT_SIN << 0);          // SIN (MOSI)
}

void hal_spi_transaction(uint8_t *data, uint8_t count)
{
    volatile uint8_t i;

    // Wait for MSTIDLE, should be a no-op since we are waiting after
    // the transfer.
    while (!(LPC_SPI0->STAT & (1 << 8)));

    for (i = count; i >= 1; i--) {
        // Wait for TXRDY
        while (!(LPC_SPI0->STAT & (1 << 1)));

        LPC_SPI0->TXDAT = data[i - 1];
    }

    // Force END OF TRANSFER
    LPC_SPI0->STAT = (1 << 7);

    // Wait for the transfer to finish
    while (!(LPC_SPI0->STAT & (1 << 8)));
}