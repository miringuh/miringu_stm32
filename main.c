#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
// #include "timer6.h"
// #include "eusart.h"
// #include "gpio.h"
// #include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"


int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();

    // RCC->APB2RSTR = RCC_APB2RSTR_IOPARST | RCC_APB2RSTR_SPI1RST;
    // RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN; // | RCC_APB2ENR_IOPBEN;

    _delay_ms(100000);

    spiInitA(BAUD_FCLK_32);

    for (uint8_t i = 0; i < 255; i++)
    {

        spi_send(i);
        latch();
;
    }

    spiStop();

    while (1)
    {
    }
    return 0;
}
