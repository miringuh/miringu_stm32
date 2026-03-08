#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
#include "portRemaps.h"
#include "tim1.h"
// #include "advTm1.h"
// #include "adc.h"
// #include "eusart.h"
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
    // pllInit();
    SysTick_Init();
    _delay_ms(10000);
    // eusart_init(U19200);

    RCC->APB2ENR = RCC_APB2ENR_IOPCEN;
    // GPIOC->CRH = GPIO_CRH_MODE13_1;
    // GPIOC->CRH = (P_P10MHZ<<20);
    PIN_MODE(GPIOC->CRH, (P_P10MHZ << 20));

    // timer1_init();
    while (1)
    {
        // GPIOC->ODR ^= GPIO_ODR_ODR13;
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        _delay_ms(20000);
        // tim_del(_12800HZ);
    }
    return 0;
}
