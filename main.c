#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "timer6.h"
// #include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
#include "dma.h"

int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();

    _delay_ms(1000000);

    eusart_init(U19200);

    eusartString("Welcome home");

    // eusart_close();

    while (1)
    {
        eusart_send(eusart_rd());
    }
    return 0;
}
