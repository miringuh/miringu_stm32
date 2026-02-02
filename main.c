#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
// #include "timer6.h"
#include "eusart.h"
// #include "gpio.h"
// #include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"
int main()
{
    pllInit();
    // hseInit();
    SysTick_Init();
    RCC->APB2ENR = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    //| RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // GPIOB->CRL |= (AF_P_P2MHZ << mosi); // MOSI
    // GPIOB->CRL |= (INP_PPULL << miso);  // MISO INP-PP
    // GPIOB->CRL |= (AF_P_P2MHZ << sck);  // SCK
    // GPIOA->CRH = (AF_P_P2MHZ << ss);    // SS

    setAF_CRH();

    eusart_init(U19200);
    eusartString(" Welcome Again", 14);
    for (uint8_t i = 0; i < 255; i++)
    {
        eusart_io(i);
        _delay_ms(60);
    }
    eusart_close();
    while (1)
    {
        // eusart_io(eusart_rd());
        // _delay_ms(6000);
    }
    return 0;
}
