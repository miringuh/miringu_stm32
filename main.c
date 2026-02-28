#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "tim1.h"
#include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
#include "lcdI2c.h"
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
    eusart_init(U19200);

    i2c1_init();
    // i2c1_start();

    // i2c1_send_address(0xA0);
    for (uint8_t i = 1; i < 255; i++)
    {
        i2c1_start();
        i2c1_getAddress(i);
        _delay_ms(2000);
    }
    i2c1_send_address(SLA_R);
    for (uint8_t i = 0; i < 255; i++)
    {
        i2c1_read();
        _delay_ms(20000);
    }
    i2c1_send_address(SLA_W);
    for (uint8_t i = 0; i < 255; i++)
    {
        i2c1_write(i);
        _delay_ms(20000);
    }
    i2c1_stop();
    while (1)
    {

        // _delay_ms(200000);
        // i2c1_read();
        // _delay_ms(20000);
    }
    return 0;
}
