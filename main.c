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
#include "dma.h"
// #include "sdcard.h"
// #include "changeover.h"
#include "timer_capture.h"

#define txBuffSize 12
char eusart_buff[txBuffSize];
// uint8_t cnt = 1;
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

    // RCC->APB1ENR = RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPCEN;
    RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    eusart_init(U19200);
    confPinC(P_P50MHZ, 13);
    timer1_init();
    //  eusartString("welcome");
    //  eusartString("welcome again");
    //  TIM1->CR1 &= ~CEN;
    timer1_delay(600);
    // timer2_ch1_init();
    //timer4();
    // timer2_ch2_init();
    // eusart_send()
    // char *msg = "welcome again and again";
    // print(U19200, msg);
    char *rxmem="hey ";
    while (1)
    {
        // rxmem = "send";
        eusart0_dma_rx_init(U19200,rxmem,strlen(rxmem));
        eusartString(rxmem);
        // print(U19200, rxmem);

        //  eusart_send(eusart_rd());
         // GPIOA->ODR ^= GPIO_ODR_ODR0;
         // GPIOA->BSRR = GPIO_BSRR_BS0;
         // timer1_delay(40);
         // GPIOA->BSRR = GPIO_BSRR_BR0;
         // timer1_delay(40);
    }
    return 0;
}
