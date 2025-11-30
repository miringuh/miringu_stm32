#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

// #include "spi.h"
// #include "timer6.h"
// #include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
#include "dma.h"
int main()
{
    pllInit();
    // hseInit();
    rcc_init();
    SysTick_Init();
    // RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN;
    // RCC->AHBENR = RCC_AHBENR_DMA1EN;
    RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB1ENR_I2C1EN;

    // NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    // NVIC_SetPriority(DMA1_Channel1_IRQn, 3);

    GPIOA->CRL = PA_L_2MHZ;
    // GPIOA->CRH = PA_H_2MHZ;

    // setAF_CRL();
    // setAF_CRH();

    // GPIOB->CRL = PB_L_2MHZ;
    // GPIOB->CRH = PB_H_2MHZ;

    // GPIOB->CRL = PB_IN_L;
    // GPIOB->CRH = PB_IN_H;

    // setB_L_AF();
    // setB_H_AF();

    dmaRun(myvar, GPIOA->ODR, readPeriph, 1);

    // readReg(0X03, getRegA);
    // eusart_init(U115200);
    // for (int i = 0; i < 255; i++)
    // {
    //     eusart_io(i);
    //     _delay_ms(100000);
    // }
    // myvar ^= 0xAAAA;
    // readReg((uint32_t)&myvar, getRegA);

    while (1)
    {
        // GPIOA->ODR = eusart_read();
        myvar ^= 0xAAAA;
        // readReg(myvar, getRegA);
        for (volatile int i = 0; i < 5000; i++)
            ;
        // eusart_read();
    }

    return 0;
}
