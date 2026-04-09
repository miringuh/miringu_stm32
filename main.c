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

int main()
{
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
    GPIOC->CRH = 0;
    GPIOC->CRL = 0;
    GPIOA->CRH = 0;
    GPIOA->CRL = 0;
    GPIOC->ODR = 0;
    GPIOC->IDR = 0;

    clock_init_20mhz_apb();
    SysTick_Init();

    timer1_init();
    timer1_del(_500ms);
    // ats_init();

    adc_lcd_init(GPIOB->CRL, 0x0000, 0);
    // lcd4_init(BAUD_FCLK_64);
    // lcd_4_init();
    write4Data("Welcome home", "welcome home");
    // lcd4_stop();
    // setPinC(P_P2MHZ, 13);
    // setPinA(P_P2MHZ, 0);
    // GPIOC->CRH |= (P_P2MHZ << 24);

    while (1)
    {
        // lcd_command(CLEAR_DISP);
        // write4Char(get_ADC());
        get_ADC();
        timer1_del(_500ms);       
        timer1_del(_500ms);
        // timer1_del(_500ms);
        // run_ats();    
        // timer1_del(_500ms);
    }
    return 0;
}
