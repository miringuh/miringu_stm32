#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include <string.h>
// #include "spi.h"
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

#define TX_BUFFSIZE 8
uint8_t tx_buffer[TX_BUFFSIZE];

char buffx[20];
char *buffn = " ";
char data[2];

char *getchar_2str(char a)
{
    data[0] = a;
    data[1] = '/';
    strcpy(buffx, data);
    strcat(data, buffx);
    return data;
}
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

    RCC->AHBENR = RCC_AHBENR_DMA1EN;
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR = RCC_APB1ENR_TIM2EN;

    eusart_init(U19200);
    confPinA(P_P50MHZ, 1);
    
    timer1_init();
    timer1_delay(600);

    // timer2_ch1_init();
    timer2_ch2_init();

    // char buf[11];
    // char *msg = "welcome home";
    // strcpy(buf, msg);
    // resend_data(U19200, buf, print);
    // msg = " hey welcome";
    // strcpy(buf, msg);
    // resend_data(U19200, buf, print);

    // eusart0_dma_rx_init(U19200, rx_buffer, 8);

    while (1)
    {
        // eusart0_dma_listener();
        // GPIOA->ODR ^= GPIO_ODR_ODR1;
        // timer1_delay(300);
        // GPIOA->BSRR = GPIO_BSRR_BR1;
        // timer1_delay(1);
    }
    return 0;
}
