#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "tim1.h"
// #include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
// #include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"
#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 0;
int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();
    _delay_ms(1000000);

    // eusart_init(U19200);
    // eusart_dma_rx_init(U19200, eusart_buff, 12);
    for (uint8_t i = 75; i < (75+12); i++)
    {
        eusart_buff[i] = i | 0x30;
    }
    eusart_dma_tx2_init(U19200, eusart_buff);

    while (1)
    {
        // if (state)
        // {
        //     USART1->CR1 |= USART_CR1_UE;
        //     state = 0;
        //     for (int i = 0; i < txBuffSize; i++)
        //     {
        //         eusart_send(eusart_buff[i]);
        //     }
        // }
    }
    return 0;
}
