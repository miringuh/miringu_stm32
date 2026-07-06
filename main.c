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
#include "sdcard.h"
// #include "changeover.h"
#include "timer_capture.h"

#define TX_BUFFSIZE 8
uint8_t tx_buffer[TX_BUFFSIZE];

char buffxn[20];
char *buffn = " ";
char data[2];

char *getchar_2str(char a)
{
    data[0] = a;
    data[1] = '/';
    strcpy(buffxn, data);
    strcat(data, buffxn);
    return data;
}
void reset_regs()
{
    // RCC->APB1ENR = 0;
    // RCC->APB2ENR = 0;
    CLEAR_REG(GPIOC->CRH);
    CLEAR_REG(GPIOC->CRL);
    CLEAR_REG(GPIOA->CRH);
    CLEAR_REG(GPIOA->CRL);
    CLEAR_REG(GPIOB->CRH);
    CLEAR_REG(GPIOB->CRL);
    CLEAR_REG(GPIOC->ODR);
    CLEAR_REG(GPIOC->IDR);
    CLEAR_REG(ADC1->CR1);
    CLEAR_REG(ADC1->CR2);
    CLEAR_REG(ADC1->SQR1);
    CLEAR_REG(ADC1->SQR2);
    CLEAR_REG(ADC1->SQR3);
    CLEAR_REG(ADC1->SMPR1);
    CLEAR_REG(ADC1->SMPR2);
}

int main()
{
    reset_regs();
    clock_init_20mhz_apb();
    SysTick_Init();
    RCC->AHBENR = RCC_AHBENR_DMA1EN;
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR = RCC_APB1ENR_TIM4EN; //| RCC_APB1ENR_BKPEN;

    // timer1_init();
    // timer1_delay(600);
    // timer2_ch1_init();
    // timer2_ch2_init();
    // char buf[11];
    // char *msg = "welcome home";
    // strcpy(buf, msg);
    // resend_data(U19200, buf, print);
    // msg = " hey welcome";
    // strcpy(buf, msg);
    // resend_data(U19200, buf, print);

    timer4();
    timer4_delay(8000);
    eusart_init(U19200);
    
    // sd_init();
    // sd_card_cond_8();
    // read_opt_cond_41();
    // spi2_init(BAUD_FCLK_16);

    // EraseCard(0);
    // EraseCard(1);
    // EraseCard(2);
    // EraseCard(3);

    // char buff[512];
    // char *fs = "testerdev1.txt";
    // strcpy(buff, fs);
    // make_file(buff);

    // fs = "testerdev2.txt";
    // strcpy(buff, fs);
    // make_file(buff);

    // fs = "testerdev3.txt";
    // strcpy(buff, fs);
    // make_file(buff, sd_buff);
    // sdWrite_pos_buff(1, fs, 16);
    // sdWrite_pos_buff(1, fs, 32);

    // sdWrite_pos_buff(1, "Welcome", 8);
    // sdWrite_pos_buff(2, "belcome", 8);
    // sdWrite_pos_buff(3, "velcome", 16);
    // sdWrite_pos_buff(1, "welcome3", 128);
    char *fname="algeria";
    print(U19200, fname);
    fname = "Aphabet";
    print(U19200, fname);
    // sdRead(0);
    // sdRead(1);
    // sdRead(2);

    while (1)
    {
        // eusart0_dma_listener();
        // GPIOA->ODR ^= GPIO_ODR_ODR4;
        // timer1_delay(300);
        // GPIOA->BSRR = GPIO_BSRR_BR1;
        // timer1_delay(1);
    }
    return 0;
}
