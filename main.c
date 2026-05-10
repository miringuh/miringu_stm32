#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
#include "portRemaps.h"
#include "tim1.h"
// #include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "lcdI2c.h"
// #include "spi_Lcd.h"
// #include "i2c.h"
// #include "dma.h"
// #include "sdcard.h"
// #include "changeover.h"

#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 1;
// char buffx[20];
// char *getchar_2str(char a)
// {
//     char data[2] = {a, '\n'};
//     strcpy(buffx, data);
//     strcat(data, buffx);
//     return data;
// }
void reset_regs()
{
    // RCC->APB1ENR = 0;
    // RCC->APB2ENR = 0;
    GPIOC->CRH = 0;
    GPIOC->CRL = 0;
    GPIOA->CRH = 0;
    GPIOA->CRL = 0;
    GPIOB->CRH = 0;
    GPIOB->CRL = 0;
    GPIOC->ODR = 0;
    GPIOC->IDR = 0;
    ADC1->CR1 = 0;
    ADC1->CR2 = 0;
    ADC1->SQR1 = 0;
    ADC1->SQR2 = 0;
    ADC1->SQR3 = 0;
    ADC1->SMPR1 = 0;
    ADC1->SMPR2 = 0;
}
int main()
{
    reset_regs();
    clock_init_20mhz_apb();
    // RCC->APB2ENR = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;

    RCC->APB1ENR = RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPCEN;

    eusart_init(U19200);
    // eusartString ("welcome");
    confPinC(P_P10MHZ, 13);
    // timer4();
    while (1)
    {
        GPIOC->BSRR = GPIO_BSRR_BS13;
        timer1_delay(_5HZ);
        GPIOC->BSRR = GPIO_BSRR_BR13;
        timer1_delay(_5HZ);
    }
    return 0;
}
