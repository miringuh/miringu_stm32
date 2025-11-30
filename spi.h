#if !defined(__SPI_)
#define __SPI_
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include "rcc_conf.h"
#include "gpio.h"

uint16_t spi_dummy = 0;
uint16_t spi_rd = 0;
// SPI EN
#define SPIEN SPI_CR1_SPE
// BAUD
#define BAUD_FCLK_2 SPI_CR1_BR_0 | SPI_CR1_BR_1;     //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_4 SPI_CR1_BR_0 | SPI_CR1_BR_1;     //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_8 SPI_CR1_BR_1 | SPI_CR1_BR_0;     //| SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_16 (SPI_CR1_BR_0 | SPI_CR1_BR_1);  //| ((SPI_CR1_BR_2) & 0X00)
#define BAUD_FCLK_32 SPI_CR1_BR_2 | SPI_CR1_BR_0;    //| SPI_CR1_BR_1) & 0X00)
#define BAUD_FCLK_64 (SPI_CR1_BR_0 | SPI_CR1_BR_2);  //| ((SPI_CR1_BR_1) & 0X00)
#define BAUD_FCLK_128 (SPI_CR1_BR_1 | SPI_CR1_BR_2); //| ((SPI_CR1_BR_0) & 0X00)
#define BAUD_FCLK_256 (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2)
//
/*
mosi PA_7
miso PA_6
sck  PA_5
ss   PA_4
*/
void SPI1_IRQHandler(void)
{
    if ((SPI1->SR & SPI_SR_MODF) == SPI_SR_MODF)
    {
        SPI1->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SPE);
    }
    if ((SPI1->SR & SPI_SR_TXE) == SPI_SR_TXE)
    {
        SPI1->SR &= ~SPI_SR_TXE;
    }
}
void latch()
{
    GPIOA->BSRR = GPIO_BSRR_BS3; // LATCH
    _delay_ms(100);
    GPIOA->BSRR = GPIO_BSRR_BR3; // LATCH
    _delay_ms(100);
}
void spiInit()
{
    RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN);
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    SPI1->SR = 0;
    AFIO->MAPR = ~AFIO_MAPR_SPI1_REMAP;
    GPIOA->CRL = (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1);  // MOSI
    GPIOA->CRL |= (GPIO_CRL_CNF6_1);                    // MISO INP-PP
    GPIOA->CRL |= (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_1); // SCK
    GPIOA->CRL |= (GPIO_CRL_CNF4_1 | GPIO_CRL_MODE4_1); // SS
    GPIOA->CRL |= GPIO_CRL_MODE0_1;                     // latch

    SPI1->CR1 |= BAUD_FCLK_128;
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 &= ~SPI_CR1_CPOL; // clk idle low
    SPI1->CR1 &= ~SPI_CR1_CPHA; // 1nd fall
    SPI1->CR2 |= SPI_CR2_ERRIE | SPI_CR2_TXEIE | SPI_CR2_RXNEIE;

    SPI1->CR1 |= (SPI_CR1_BIDIOE | SPI_CR1_LSBFIRST); //
    SPI1->CR1 |= SPI_CR1_SPE;
}
void spi_send(uint8_t val)
{
    SPI1->DR = val;
    while ((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY) // 0=nBUSY 1=BUSY
    {
    }
    while ((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
    {
        spi_dummy = SPI1->DR;
        SPI1->SR &= ~SPI_SR_RXNE;
    }
    GPIOA->BSRR = GPIO_BSRR_BR0;
    _delay_ms(1000);
    GPIOA->BSRR = GPIO_BSRR_BS0;
    _delay_ms(1000);
}
void spiExit()
{
    SPI1->CR1 &= ~SPI_CR1_SPE;
}
////// MANUAL ////////
#define del 1000
uint8_t mbuff[8];
uint8_t buff[8];
uint8_t *msb_send(uint8_t val)
{
    uint8_t cnt = 1;
    uint8_t value = (val);
    uint8_t buff[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        buff[i] = ((value & cnt) >> i);
        cnt = (cnt << 1);
    }
    mbuff[0] = buff[7];
    mbuff[1] = buff[6];
    mbuff[2] = buff[5];
    mbuff[3] = buff[4];
    mbuff[4] = buff[3];
    mbuff[5] = buff[2];
    mbuff[6] = buff[1];
    mbuff[7] = buff[0];
    return mbuff;
}
uint8_t *lsb_send(uint8_t val)
{
    uint8_t cnt = 1;
    uint8_t value = (val);
    for (uint8_t i = 0; i < 8; i++)
    {
        mbuff[i] = ((value & cnt) >> i);
        cnt = (cnt << 1);
    }
    return mbuff;
}
void spi_man(uint16_t val)
{
    msb_send(val);
    for (uint8_t i = 0; i < 8; i++)
    {
        if (mbuff[i] == 1)
        {
            GPIOA->BSRR = GPIO_BSRR_BS7;
        }
        if (mbuff[i] == 0)
        {
            GPIOA->BSRR = GPIO_BSRR_BR7;
        }
        _delay_ms(del);
        GPIOA->BSRR |= GPIO_BSRR_BS5;
        _delay_ms(del);
        GPIOA->BSRR = GPIO_BSRR_BR7 | GPIO_BSRR_BR5;
        _delay_ms(del);
    }
    GPIOA->BSRR = GPIO_BSRR_BS4; // LATCH
    _delay_ms(del);
    GPIOA->BSRR = GPIO_BSRR_BR4; // LATCH
    _delay_ms(del);
}
#endif // __SPI_;

/*
 // rcc_control_hsi();
    rcc_control();
    RCC->APB2ENR = RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;
    gpiob_set();
    gpioa_set();
    spi_init_tx();
    for (int i = 0; i < 129; i++)
    {
        spi_send(i);
        latch();
    }
    while (((SPI1->SR & SPI_SR_BSY) >> 7) == 1) // 0=nBUSY 1=BUSY
    {
    }
    GPIOB->ODR = 0;
    SPI1->CR1 &= !SPI_CR1_SPE;
    //// TIMER1 ////////////
    rcc_control();
    timer1_init();
    RCC->APB2ENR = RCC_APB2ENR_IOPCEN;
    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    //
    while (1)
    {
        WRITE_REG(GPIOC->BSRR, GPIO_BSRR_BS13);
        _delay_ms_us(100000);
        WRITE_REG(GPIOC->BSRR, GPIO_BSRR_BR13);
        _delay_ms_us(100000);
    }
*/