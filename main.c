#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
#include "timer6.h"
// #include "adc.h"
// #include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"

int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();

    _delay_ms(1000000);
    eusart_init(U19200);
    timer6_init();


    while (1)
    {
        // test_tim6(2223); // 100HZ
        // test_tim6(4446); // 50HZ
        // test_tim6(8892); // 25HZ
        test_tim6(17784); // 12.5HZ
    }
    return 0;
}
