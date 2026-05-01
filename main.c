#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
#include "portRemaps.h"
// #include "tim1.h"
// #include "advTm1.h"
#include "adc.h"
#include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "lcdI2c.h"
#include "spi_Lcd.h"
// #include "i2c.h"
// #include "dma.h"
#include "adc.h"
#include "changeover.h"

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

    RCC->APB2ENR = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    // reset_regs();

    eusart_init(U19200);
    GPIOC->CRH = (P_P50MHZ << GPIO_CRH_MODE13_Pos);
    // timer1_del(_500ms);

    // eusart_send(i);

    // timer1_del(_500ms);

    // ats_init();
    // adcInit();
    // lcd4_init(BAUD_FCLK_64);
    // lcd_4_init();
    // write4Data("Welcome home", "welcome home");
    // lcd4_stop();
    // setPinC(P_P2MHZ, 13);
    // setPinA(P_P2MHZ, 0);
    // GPIOC->CRH |= (P_P2MHZ << 24);

    // GPIOB->CRH = GPIO_CRH_MODE8;                // k1
    // GPIOB->CRH |= GPIO_CRH_MODE9;               // k2
    // setPinB(P_P10MHZ,8);
    // setPinB(P_P10MHZ, 9);
    // lcd_adc_config();

    // GPIOB->ODR = GPIO_ODR_ODR8 | GPIO_ODR_ODR9 | GPIO_ODR_ODR10 | GPIO_ODR_ODR11;
    // timer1_del(_500ms);
    // GPIOA->ODR =  GPIO_ODR_ODR0
    ;
    while (1)
    {
        // lcd_command(CLEAR_DISP);
        // write4Char(get_ADC());

        // // getAdc();
        // run_ats();
        // timer1_delay(_5HZ / 20);
        timer4_delay(_5HZ);
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
    return 0;
}
