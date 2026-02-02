#if !defined(__SPI_)
#define __SPI_
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include "rcc_conf.h"
#include "gpio.h"

uint16_t spi_dummy = 0;
uint16_t spi_error = 0;
uint16_t spi_rd = 0;
// SPI EN
#define SPIEN SPI_CR1_SPE
// BAUD
#define BAUD_FCLK_2 (SPI_CR1_BR_0 | SPI_CR1_BR_1)   //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_4 (SPI_CR1_BR_0 | SPI_CR1_BR_1)   //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_8 (SPI_CR1_BR_1 | SPI_CR1_BR_0)   //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_16 (SPI_CR1_BR_0 | SPI_CR1_BR_1)  //| ((SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_32 (SPI_CR1_BR_2 | SPI_CR1_BR_0)  //| SPI_CR1_BR_1) & 0X00)
#define BAUD_FCLK_64 (SPI_CR1_BR_0 | SPI_CR1_BR_2)  //| ((SPI_CR1_BR_1) & 0X00)
#define BAUD_FCLK_128 (SPI_CR1_BR_1 | SPI_CR1_BR_2) //| ((SPI_CR1_BR_0) & 0X00)
#define BAUD_FCLK_256 (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2)
// PB5
#define mosi 5 // B5
#define miso 4 // B4
#define sck 3  // B3
#define ss 15  // A15
#define latch 0
/*
SPI1->CR2 |= SPI_CR2_SSOE;//ss-out 1-en 0-noEnable
SPI1->CR1 |= SPI_CR1_SSM;//1-software 0-hardware (management)

NSS-OUT_EN (ssm=0,ssoe=1) >(ONLY ON MASTER MODE)
NSS-OUT NOT_EN (ssm=0,ssoe=0)>(multiMstr Mode // if slave (NSS acts input)if NSS-low the slave is activates )

    RCC->APB2ENR |=RCC_APB2ENR_SPI1EN
    GPIOB->CRL = (AF_P_P2MHZ << mosi);  // MOSI
    GPIOB->CRL |= (AF_P_P2MHZ << miso); // MISO INP-PP
    GPIOB->CRL |= (AF_P_P2MHZ << sck);  // SCK
    GPIOA->CRH = (AF_P_P2MHZ << ss);    // SS
    GPIOA->CRL = (PA0_OUT_2MHZ);        // latch

    spiInit();

*/
volatile uint8_t *buff;
void SPI1_IRQHandler(void)
{
    switch (SPI1->SR)
    {

    case (SPI_SR_TXE):
        spi_dummy = SPI1->DR;
        break;
    case (SPI_SR_RXNE):
        spi_dummy = SPI1->DR;
        break;
    case (SPI_SR_OVR):
        spi_dummy = SPI1->DR;
        spi_error = SPI1->SR;
        break;
    default:
        break;
    }
}
void latch1()
{
    GPIOB->ODR = GPIO_ODR_ODR6;
    _delay_ms(100);
    GPIOB->ODR &= ~GPIO_ODR_ODR6;
    _delay_ms(100);
}

void spiInit(uint16_t baud)
{
    // RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN);
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    SPI1->SR = 0;

    // NVIC_EnableIRQ(SPI1_IRQn);
    SPI1->CR1 |= baud;
    // SPI1->CR1 &= ~SPI_CR1_CPHA;    // 0-lead 1-lag
    // SPI1->CR1 &= ~SPI_CR1_CPOL;    // 0-low->high 1-high->low
    SPI1->CR1 &= ~SPI_CR1_DFF;     // 0-8bit 1-16bit
    SPI1->CR1 |= SPI_CR1_LSBFIRST; // 0-msbFirst 1-lsbFirst

    SPI1->CR1 |= SPI_CR1_BIDIMODE; // 0- 2-line(UNIDIR) 1 1-line(BIDIR)
    SPI1->CR1 |= SPI_CR1_BIDIOE;   // 0-rx mode 1-tx mode

    SPI1->CR2 = SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
    SPI1->CR2 |= SPI_CR2_SSOE; //
    SPI1->CR1 |= SPI_CR1_MSTR; // 0-slv 1-mstr
    SPI1->CR1 |= SPI_CR1_SPE;
    // mosi-->>   miso<<--
}
void spi_send(uint8_t val)
{
    SPI1->DR = val;
    while ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE) // 0=empty 1=Full
    {
    }
    spi_dummy = SPI1->DR;
}

void spiStop()
{
    SPI1->CR1 &= ~SPI_CR1_SPE;
}
//
#endif // __SPI_;
       /*
        // GPIOB->CRH = PB_H_2MHZ;
           // AFIO->MAPR = ~AFIO_MAPR_SPI1_REMAP;
           GPIOB->CRL = (AF_P_P2MHZ << mosi); // MOSI
           GPIOB->CRL |= (AF_P_P2MHZ << miso); // MISO INP-PP
           GPIOB->CRL |= (AF_P_P2MHZ << sck); // SCK
           GPIOA->CRH = (AF_P_P2MHZ << ss);     // SS
           GPIOA->CRL = (PA0_OUT_2MHZ);         // latch
           spiInit();
       */