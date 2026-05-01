#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
#include "portRemaps.h"
// #include "tim1.h"
// #include "advTm1.h"
// #include "adc.h"
#include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "lcdI2c.h"
// #include "spi_Lcd.h"
// #include "i2c.h"
// #include "dma.h"
#include "sdcard.h"
// #include "changeover.h"

#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 1;
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

    clock_init_20mhz_apb();

    RCC->APB2ENR = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;

    RCC->APB1ENR = RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;

    spi2_init(BAUD_FCLK_64);
    eusart_init(U19200);
    confPinB(P_P50MHZ, 0); // cs

    sd_init();
    sd_card_cond_8();   // 00 00 01 AA
    read_opt_cond_41(); // 00 0001 0000
    // read_ocr_58();
    del = _3200HZ;
    // for (uint32_t i = 0; i < 0x7FF; i++)
    // {
    // sdWrite_pos(i, "", 0, 8);
    // }

    // sdRead(0);
    sdWrite_String(0X7FF,"");
    sdRead(0X7FF);

    while (1)
    {
    }
    return 0;
}
