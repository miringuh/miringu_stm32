#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "timer6.h"
#include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"
#define txBuffSize 128
uint32_t eusart_buff[txBuffSize];
uint8_t cnt = 0;
int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();
    _delay_ms(1000000);

    // eusart_dma_tx(U19200);

    eusart_dma_tx_init(U19200);
    uart_dma1set("welcome");


    while (1)
    {
    }
    return 0;
}
