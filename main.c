#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "tim1.h"
#include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"
#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 1;
int main()
{
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;

    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();
    _delay_ms(1000000);

    eusart_init_1(U19200);

    
    while (1)
    {

    }
    return 0;
}
