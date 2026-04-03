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

#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 1;

int main()
{
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
    clock_init_20mhz_apb();
    SysTick_Init();
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;
    _delay_ms(900000);

    // ADC_Init(GPIOA->CRL, 0x0000);
    // setPinC(P_P50MHZ, 13);
    // setPinC(P_P50MHZ, 14);

    lcd4_init(BAUD_FCLK_64);
    lcd_4_init();
    write4Data("Welcome", "WELCOMEs");
    lcd4_stop();
    while (1)
    {
    }
    return 0;
}
