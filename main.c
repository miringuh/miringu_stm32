#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#include "spi.h"
// #include "timer6.h"
// #include "eusart.h"
#include "gpio.h"
#include "rcc_conf.h"
// #include "i2c.h"
// #include "dma.h"
int main()
{
    pllInit();
    // hseInit();
    rcc_init();
    SysTick_Init();
    RCC->APB2ENR = (RCC_APB2ENR_IOPBEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN);
    // GPIOB->CRH = PB_H_2MHZ;
    // AFIO->MAPR = ~AFIO_MAPR_SPI1_REMAP;
    GPIOB->CRL = (AF_P_P2MHZ << mosi); // MOSI
    GPIOB->CRL |= (AF_P_P2MHZ << miso); // MISO INP-PP
    GPIOB->CRL |= (AF_P_P2MHZ << sck); // SCK
    GPIOA->CRH = (AF_P_P2MHZ << ss);     // SS
    GPIOA->CRL = (PA0_OUT_2MHZ);         // latch
    spiInit();
    
    for (int i = 1; i < 255; i++)
    {
        spi_send(i);
        // getRegB(i);
    }
    spiExit();
    while (1)
    {
    }

    return 0;
}
