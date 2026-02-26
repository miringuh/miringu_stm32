#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
// #include "tim1.h"
// #include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
#include "i2c.h"
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
    char *values = "0123456789ABCDEFGHIJKLMN";
    dma_i2cTx_init(values);
    // i2c1_send_address(SLA_W);
    for (uint8_t i = 0; i < strlen(values); i++)
    {
        eusart_send(i2c_buff[i]);
        _delay_ms(60000);
    }

    i2c1_stop();
    // i2c1_init();
    // for (uint8_t i = 255; i > 0; i--)
    // {
    //     i2c1_send_address(i);
    //     i2c1_stop();
    //     i2cStart();
    //     _delay_ms(100);
    // }
    // i2c1_stop();
    while (1)
    {
    }
    return 0;
}
