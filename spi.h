#if !defined(__SPI_)
#define __SPI_
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include "rcc_conf.h"
#include "eusart.h"
#include "gpio.h"

volatile uint8_t spi_dummy = 0;
uint16_t spi_error = 0;
volatile uint8_t spi_rd = 0;
// SPI EN
#define SPIEN SPI_CR1_SPE
// BAUD
#define BAUD_FCLK_2 0
#define BAUD_FCLK_4 (SPI_CR1_BR_0)
#define BAUD_FCLK_8 (SPI_CR1_BR_1)
#define BAUD_FCLK_16 (SPI_CR1_BR_0 | SPI_CR1_BR_1)
#define BAUD_FCLK_32 (SPI_CR1_BR_2)
#define BAUD_FCLK_64 (SPI_CR1_BR_0 | SPI_CR1_BR_2)
#define BAUD_FCLK_128 (SPI_CR1_BR_1 | SPI_CR1_BR_2)
#define BAUD_FCLK_256 (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2)
// PB5
// 50mhz
#define MOSI_A (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_Msk) // PA7
#define MISO_A (GPIO_CRL_CNF6_0)                      // PA6 or FLOAT_INP
#define SCK_A (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_Msk)  // PA5
#define SS_A (GPIO_CRL_CNF4_1 | GPIO_CRL_MODE4_Msk)   // PA4

#define LATCH (GPIO_CRL_MODE3_1) // PA3
// 50mhz
#define MOSI_B (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_Msk) // AF PB5
#define MISO_B (GPIO_CRL_CNF4_0)                      // PB4 or FLOAT_INP
#define SCK_B (GPIO_CRL_CNF3_1 | GPIO_CRL_MODE3_Msk)  // AF PB3
#define SS_B (GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15_Msk) // AF PA15
/*
SPI1->CR2 |= SPI_CR2_SSOE;//ss-out 1-en 0-noEnable
SPI1->CR1 |= SPI_CR1_SSM;//1-software 0-hardware (management)

NSS-OUT_EN (ssm=0,ssoe=1) >(ONLY ON MASTER MODE)
NSS-OUT NOT_EN (ssm=0,ssoe=0)>(multiMstr Mode // if slave (NSS acts input)if NSS-low the slave is activates )
*/
volatile uint8_t *buff;
void spiStop();
void test();
void latch();

void SPI1_IRQHandler(void)
{
    if (SPI1->SR & SPI_SR_TXE)
    {
        spi_dummy = SPI1->SR;
        spi_dummy = SPI1->DR;
        SPI1->SR &= ~SPI_SR_TXE;
    }
    if (SPI1->SR & SPI_SR_RXNE)
    {
        spi_dummy = SPI1->SR;
        spi_rd = SPI1->DR;
        SPI1->SR &= ~SPI_SR_RXNE;
    }
    if (SPI1->SR & SPI_SR_OVR)
    {
        spi_rd = SPI1->DR;
        spi_dummy = SPI1->SR;
        SPI1->SR &= ~(SPI_SR_OVR | SPI_SR_MODF | SPI_SR_CRCERR);
    }
    // test();
    NVIC_DisableIRQ(SPI1_IRQn);
}
void test()
{
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }
    if (!(GPIOC->CRH & GPIO_CRH_MODE13_1))
    {
        GPIOC->CRH = GPIO_CRH_MODE13_1; // 2MHZ P_P
    }
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    _delay_ms(60000);
}
void spiInitB(uint8_t baud)
{
    /* Enable clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_SPI1_REMAP;

    GPIOB->CRL &= ~(
        GPIO_CRL_MODE3 | GPIO_CRL_CNF3 |
        GPIO_CRL_MODE4 | GPIO_CRL_CNF4 |
        GPIO_CRL_MODE5 | GPIO_CRL_CNF5);

    GPIOB->CRL |= MOSI_B | MISO_B | SCK_B;
    GPIOA->CRH = SS_A;
    GPIOA->CRL = LATCH;

    // SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI;

    SPI1->CR1 = SPI_CR1_MSTR | baud;
    SPI1->CR1 &= ~SPI_CR1_CPHA;     // 0-lead 1-lag
    SPI1->CR1 &= ~SPI_CR1_CPOL;     // 0-low->high 1-high->low
    SPI1->CR1 &= ~SPI_CR1_DFF;      // 0-8bit 1-16bit
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST; // 0-msbFirst 1-lsbFirst
    // SPI1->CR1 |= SPI_CR1_BIDIMODE;  // 0-miso-only 1-mosi,miso,clk
    // SPI1->CR1 |= SPI_CR1_BIDIOE;    // 0-rx mode 1-tx mode
    SPI1->CR2 |= SPI_CR2_SSOE; // SS output 1==multi mst Enable

    SPI1->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE;
    // NVIC_EnableIRQ(SPI1_IRQn);
    // NVIC_SetPriority(SPI1_IRQn, 2);
    SPI1->CR1 |= SPI_CR1_SPE;
}
//
void pinSetup()
{
    GPIOA->CRL = 0;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP; // 1=PINB 0=PINA
    GPIOB->CRL &= ~(
        GPIO_CRL_MODE3 | GPIO_CRL_CNF3 |
        GPIO_CRL_MODE4 | GPIO_CRL_CNF4 |
        GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= SS_A; //|SCK | MOSI | MISO | LATCH;
}

void spiInitA(uint8_t baud)
{
    /* Enable clocks */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    GPIOA->CRL = 0;
    GPIOA->CRH = 0;
    GPIOA->CRL |= MOSI_A | MISO_A | SCK_A | SS_A | LATCH;

    SPI1->CR1 |= SPI_CR1_MSTR | baud;
    SPI1->CR1 |= SPI_CR1_CPHA;    // 0-lead 1-lag
    SPI1->CR1 &= ~SPI_CR1_CPOL;    // 0-low->high 1-high->low
    SPI1->CR1 &= ~SPI_CR1_DFF;     // 0-8bit 1-16bit
    SPI1->CR1 |= SPI_CR1_LSBFIRST; // 0-msbFirst 1-lsbFirst

    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    // SPI1->CR1 |= SPI_CR1_BIDIMODE;  // 0-miso-only 1-mosi,miso,clk
    // SPI1->CR1 |= SPI_CR1_BIDIOE;    // 0-rx mode 1-tx mode

    SPI1->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
    SPI1->CR2 |= SPI_CR2_SSOE; // SS output 1==multi mst Enable
    SPI1->CR1 |= SPI_CR1_SPE;
    NVIC_SetPriority(SPI1_IRQn, 3);
    NVIC_EnableIRQ(SPI1_IRQn);
}
////////////////
uint8_t spi_send(uint8_t val)
{

    while (!(SPI1->SR & SPI_SR_TXE)) // 1=empty 0=Full
        ;
    SPI1->DR = val;
    while (!(SPI1->SR & SPI_SR_RXNE)) // rxbuff 1=FULL 0=EMPTY
        ;
    return SPI1->DR;
}
uint8_t spi1_transfer(uint8_t data)
{
    /* Wait until TX buffer empty */
    while (!(SPI1->SR & SPI_SR_TXE))
        ;

    /* Send byte */
    *(volatile uint8_t *)&SPI1->DR = data;

    /* Wait until RX buffer full */
    while (!(SPI1->SR & SPI_SR_RXNE))
        ;

    /* Read received byte */
    return *(volatile uint8_t *)&SPI1->DR;
}
////////////
void latch()
{
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    }
    if (!(GPIOA->CRL & GPIO_CRL_MODE3_1))
    {
        GPIOA->CRL = GPIO_CRL_MODE3_1; // PA3
    }

    GPIOA->ODR &= ~GPIO_ODR_ODR3;
    _delay_ms(60000);
    GPIOA->ODR = GPIO_ODR_ODR3;
    _delay_ms(60000);
}
void spiStop()
{
    // NVIC_DisableIRQ(SPI1_IRQn);
;
    GPIOC->BSRR &= ~GPIO_BSRR_BR0;
    GPIOA->ODR = 0;
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
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