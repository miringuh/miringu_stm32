#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
#include "tim1.h"
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

    // eusart_init(U19200);
    eusart_dma_tx_init(U19200, "Welcome");
    // timer1_init();
    // eusartString("welcome");
 
    while (1)
    {
        // eusart_read
        // test_tim6();
        // tim_del(_100HZ);
    }
    return 0;
}
