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
#include "spi_Lcd.h"
// #include "i2c.h"
#include "dma.h"
#include "sdcard.h"
// #include "changeover.h"
#include "timer_capture.h"

#define TX_BUFFSIZE 8
uint8_t tx_buffer[TX_BUFFSIZE];

char buffxn[120];
char *buffn = " ";
char data[512];

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
    clock_init_40mhz_apb();
    SysTick_Init();
    RCC->AHBENR = RCC_AHBENR_DMA1EN;
    RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR = RCC_APB1ENR_TIM4EN; //| RCC_APB1ENR_BKPEN;

    // timer1_init();
    // timer1_delay(600);
    // timer2_ch1_init();
    // timer2_ch2_init();

    timer4();
    timer4_delay(900);

    eusart_init(U19200);
    eusart0_dma_rx_init(U19200, 10, data);
    eusart0_dma_tx_init(U19200);

    sd_init(BAUD_FCLK_128);
    sd_card_cond_8();
    read_opt_cond_41();
    // spi2_init(BAUD_FCLK_16);

    // EraseCard(0);
    // EraseCard(1);
    // EraseCard(2);
    // EraseCard(1);
    // sdWrite_pos_buff(1, "first_file", 10, 0);
    // sdWrite_pos_buff(1, "Second_file", 20, 16);
    // sdWrite_pos_buff(1, "third_file", 30, 32);
    // sdWrite_pos_buff(1, "fourth_file", 40, 48);
    // sdWrite_pos_buff(1, "fifth_file", 50, 64);

    // sdRead(1);
    // dma_uart_send(sd_buff, 512);

    while (1)
    {
        eusart0_sdma_listener(data);
    }
    return 0;
}
