#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
#include "portRemaps.h"
#include "tim1.h"
// #include "advTm1.h"
//  #include "adc.h"
#include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "lcdI2c.h"
// #include "i2c.h"
// #include "dma.h"
#define PINC13(VAL) WRITE_REG(GPIOC->CRH, (VAL << 20))
#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 1;

int main()
{
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
    clock_init_20mhz_apb();
    SysTick_Init();
    timer1_init();
    timer1_del(10000);

    // pllInit();
    // eusart_init(U19200);

    // timer1_Init(0, 0xFFFF, 0XFFFE);
    // eusartString("welcome");
    while (1)
    {
        timer1_del(_50ms );
        test_tim6();
        // eusart_send(eusart_rd());
        // ADC_Start_Conversion();
        // _delay_ms(400000);
    }
    return 0;
}
