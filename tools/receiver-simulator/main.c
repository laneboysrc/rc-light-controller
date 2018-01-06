/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <hal.h>
#include <printf.h>

extern uint16_t HAL_adc_read(uint32_t mux);

// ****************************************************************************
static void service_systick(void)
{
    ++entropy;
}

// ****************************************************************************
static uint16_t adc_to_pulse_us(uint16_t adc_value)
{
    uint32_t pulse;
    const uint16_t MIN = 200;
    const uint16_t MAX = 65500 - MIN;

    if (adc_value > MIN) {
        adc_value -= MIN;
    }
    else {
        adc_value = 0;
    }

    pulse = adc_value;
    pulse = (pulse * 1000) / MAX;
    pulse = pulse + 1000;

    return pulse;
}

// ****************************************************************************
static void service_fake_receiver(void)
{
    static uint32_t next_tick_ms = 0;
    static uint16_t last_st = 0;
    static uint16_t last_th = 0;
    bool new_value = false;

    if (milliseconds >= next_tick_ms) {
        uint16_t adc_value;
        uint16_t pulse;

        next_tick_ms += 10;

        adc_value = HAL_adc_read(ADC_INPUTCTRL_MUXPOS_PIN10);
        pulse = adc_to_pulse_us(adc_value);
        if (pulse != last_st) {
            last_st = pulse;
            new_value = true;
            TC3->COUNT16.CC[0].reg = last_st * 2;
        }

        adc_value = HAL_adc_read(ADC_INPUTCTRL_MUXPOS_PIN11);
        pulse = adc_to_pulse_us(adc_value);
        if (pulse != last_th) {
            last_th = pulse;
            new_value = true;
            TC4->COUNT16.CC[0].reg = last_th * 2;
        }

        if (new_value) {
            printf("ST=%d  TH=%d\n", last_st, last_th);
        }
    }
}

// ****************************************************************************
int main(void)
{
    HAL_hardware_init(false, false, false);
    HAL_uart_init(115200);
    init_printf(STDOUT, HAL_putc);

    HAL_hardware_init_final();

    printf("\n\n**********\nReceiver simulator\n");

    while (1) {
        service_systick();
        service_fake_receiver();
        HAL_service();
    }
}
