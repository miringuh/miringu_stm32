#if !defined(_SSPI)
#define _SSPI
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include "rcc_conf.h"
#include "eusart.h"
#include "gpio.h"
#include "tim1.h"
#include "portRemaps.h"
//
//  (SPI_CR1)
#define BIDIMODE SPI_CR1_BIDIMODE // 0: 2-line uni-DIR 1: 1-line BIDIR
#define BIDIOE SPI_CR1_BIDIOE     // 0 RX-only mode  1 TX only Mode
#define CRCEN SPI_CR1_CRCEN       // 1- ENabled
/*
This bit has to be written as soon as the last data wriTE into the SPI_DR register
*/
#define CRCNEXT SPI_CR1_CRCNEXT // 1- ENabled
#define DFF SPI_CR1_DFF         // 0-8BIT 1-16BITS
#define RXONLY SPI_CR1_RXONLY   // 1-full Duplex TX/RX  0-RX-only
/*
When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
SSM=1 SS-PIN==SSI bit
*/
#define SSM SPI_CR1_SSM // Software slave management
/*
The value of this bit is forced onto the NSS pin
and the I/O value of the NSS pin is ignored.
*/
#define SSI SPI_CR1_SSI
#define LSBFIRST SPI_CR1_LSBFIRST    // 0-MSB 1-LSB
#define SPE SPI_CR1_SPE              // 0-DISABLED  1-ENABLED
#define BR(REG, VAL) WRITE(REG, VAL) // Baud Rate Control
#define MSTR SPI_CR1_MSTR            // 0-SLAVE  1-MASTER
#define CPOL SPI_CR1_CPOL            // clk polarity
#define CPHA SPI_CR1_CPHA            // clk phase  0-(1st-clk) 1-(2nd-clk)
//

//******  (SPI_CR2)
#define TXEIE SPI_CR2_TXEIE   // Tx buffer empty interrupt enable
#define RXNEIE SPI_CR2_RXNEIE // RX buffer not empty interrupt enable
// This bit controls the gen of an interrupt when an error condition occurs (CRCERR, OVR, MODF
#define ERRIE SPI_CR2_ERRIE
// 0: SS output is disabled in master mode
#define SSOE SPI_CR2_SSOE       // SS Output Enable
#define TXDMAEN SPI_CR2_TXDMAEN // TX DMA Enable
#define RXDMAEN SPI_CR2_RXDMAEN // RX DMA Enable
//

//****** (SPI_SR)
// This flag is set and reset by hardware.
#define BSY SPI_SR_BSY // 1 SPI-BUSY  0 SPI-NOT BUSY
// This flag is set by hardware and reset by a software sequence
#define OVR SPI_SR_OVR // 0: No Overrun occur 1: Overrun occur
// This flag is set by hardware and reset by a software sequence
#define MODF SPI_SR_MODF // 1: Mode fault occurred
// CRC error flag This bit is only used in full-duplex mode.
#define CRCERR SPI_SR_CRCERR // 1: CRC value rXed does not match
#define TXE SPI_SR_TXE       // 1-EMPTY 0-FULL
#define RXNE SPI_SR_RXNE     // 1-FULL 0-EMPTY
//
//  (SPI_DR)
#define DATA_REG(REG, VAL) WRITE_REG(REG, VAL)
//  (SPI_CRCPR)
#define CRC_POLY(REG, VAL) WRITE_REG(REG, VAL)
//  (SPI_RXCRCR)
#define RXCRC(REG, VAL) WRITE_REG(REG, VAL)
//  (SPI_RXCRCR)
#define TXCRC(REG, VAL) WRITE_REG(REG, VAL)
//   BAUDS
#define BAUD_FCLK_2 0
#define BAUD_FCLK_4 (SPI_CR1_BR_0)
#define BAUD_FCLK_8 (SPI_CR1_BR_1)
#define BAUD_FCLK_16 (SPI_CR1_BR_0 | SPI_CR1_BR_1)
#define BAUD_FCLK_32 (SPI_CR1_BR_2)
#define BAUD_FCLK_64 (SPI_CR1_BR_0 | SPI_CR1_BR_2)
#define BAUD_FCLK_128 (SPI_CR1_BR_1 | SPI_CR1_BR_2)
#define BAUD_FCLK_256 (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2)
//
// mosi-PA7 miso-PA6  sck-PA5  ss-PA4 50MHZ
#define MOSI_0 ((GPIO_CRL_CNF7_1) | (GPIO_CRL_MODE7_Msk)) // AF-PP
#define MISO_0 ((GPIO_CRL_CNF6_1))                        // input P-P
#define SCKL_0 ((GPIO_CRL_CNF5_1) | (GPIO_CRL_MODE5_Msk)) // AF-PP
#define SS_0 ((GPIO_CRL_CNF4_1) | (GPIO_CRL_MODE4_Msk))   // AF-PP
//
// mosi pb5 miso pb4 sck-pb3 ss-pa15 50mhz
#define MOSI_1 (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_Msk) // AF PB5
#define MISO_1 (GPIO_CRL_CNF4_1)                      // PB4 or INPUT P-P
#define SCKL_1 (GPIO_CRL_CNF3_1 | GPIO_CRL_MODE3_Msk) // AF PB3
#define SS_1 (GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15_Msk) // AF PA15
//
// mosi pb15 miso pb14 sck-pb13 ss-pb12 50mhz
#define MOSI_2 ((GPIO_CRH_CNF15_1) | (GPIO_CRH_MODE15_Msk)) // AF-PP
#define MISO_2 ((GPIO_CRH_CNF14_1))                         // input P-P
#define SCKL_2 ((GPIO_CRH_CNF13_1) | (GPIO_CRH_MODE13_Msk)) // AF-PP
#define SS_2 ((GPIO_CRH_CNF12_1) | (GPIO_CRH_MODE12_Msk))   // AF-PP
//
volatile uint8_t spi_dummy = 0;
uint16_t spi_error = 0;
volatile uint8_t spi_rd = 0;
volatile uint8_t sdata;

void latch() // PC13
{
    // if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
    // {
    //     RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // }
    // if (!(GPIOC->CRH & GPIO_CRH_MODE13_1))
    // {
    //     GPIOC->CRH = GPIO_CRH_MODE13_1; // 2MHZ P_P
    // }
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
    timer4_delay(10);
    GPIOC->ODR |= GPIO_ODR_ODR13;
    timer4_delay(10);
}
////////////
void spi0_setup()
{ // mosi - PA7 miso - PA6 sck - PA5 ss - PA4 50MHZ
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
    AFIO->MAPR &= ~(AFIO_MAPR_SPI1_REMAP);
}
void spi1_setup()
{ // mosi pb5 miso pb4 sck-pb3 ss-pa15 50mhz
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_SPI1_REMAP;
}
void spi2_setup()
{ // mosi pb15 miso pb14 sck-pb13 ss-pb12 50mhz
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    AFIO->MAPR |= (AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
    AFIO->MAPR &= ~(AFIO_MAPR_SPI1_REMAP);
    GPIOB->CRH = (AF_P_P50MHZ << GPIO_CRH_MODE15_Pos);  // MOSI
    GPIOB->CRH |= (INP_PPULL << GPIO_CRH_MODE14_Pos);   // MIS0
    GPIOB->CRH |= (AF_P_P50MHZ << GPIO_CRH_MODE13_Pos); // MOSI
    GPIOB->CRH |= (AF_P_P50MHZ << GPIO_CRH_MODE12_Pos); // SS
}
/////////
/////////
void SPI1_IRQHandler(void)
{
    // eusart_send(SPI1->DR);

    if (SPI1->SR & SPI_SR_RXNE) // 0-empty 1-full
    {
        spi_rd = SPI1->DR;
    }
    if (SPI1->SR & SPI_SR_OVR) // 0-no ovr 1-ovr
    {
        SPI1->SR &= ~SPI_SR_OVR;
    }
    if (SPI1->SR & SPI_SR_MODF) // 0-no fault 1-fault
    {
        SPI1->SR &= ~SPI_SR_MODF;
    }
}
void SPI2_IRQHandler(void)
{
    // eusart_send(SPI1->DR);

    if (SPI2->SR & SPI_SR_RXNE) // 0-empty 1-full
    {
        spi_rd = SPI2->DR;
    }
    if (SPI2->SR & SPI_SR_OVR) // 0-no ovr 1-ovr
    {
        SPI2->SR &= ~SPI_SR_OVR;
    }
    if (SPI2->SR & SPI_SR_MODF) // 0-no fault 1-fault
    {
        SPI2->SR &= ~SPI_SR_MODF;
    }
}
/////////////////
////// SPI0 /////
void spi0_init(uint8_t baud)
{ // mosi-PA7  miso-PA6  sck-PA5 ss - PA4 50MHZ
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;

    spi0_setup();
    GPIOA->CRL |= (MOSI_0 | MISO_0 | SCKL_0 | SS_0);

    SPI1->CR1 = baud;
    SPI1->CR1 |= SSM | MSTR | SSI;
    // SPI1->CR1 |= SPI_CR1_LSBFIRST;
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
    SPI1->CR2 = RXNEIE | ERRIE | SSOE; // | TXEIE;

    NVIC_SetPriority(SPI1_IRQn, 2);
    NVIC_EnableIRQ(SPI1_IRQn);
    SPI1->CR1 |= SPE;
}
uint8_t spi0_send(uint8_t val)
{
    // while (!(SPI1->SR & SPI_SR_TXE)) // 0-Full 1-empty
    //     ;
    SPI1->DR = val;
    while ((SPI1->SR & SPI_SR_BSY)) // 0-free 1-bsy
        ;
    spi_rd = SPI1->DR;
    return (uint8_t)(spi_rd);
}
void spi0_buffer(char buff[], uint16_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        spi0_send(buff[i]);
    }
}
void spi0_stop()
{
    NVIC_DisableIRQ(SPI1_IRQn);
    SPI1->CR1 &= ~SPE;
}
/////////////////
////// SPI1 /////
void spi1_init(uint8_t baud)
{ // mosi pb5 miso pb4 sck-pb3 ss-pa15 50mhz
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    GPIOB->CRL = 0;
    GPIOA->CRH = 0;

    spi1_setup();
    GPIOB->CRL |= MOSI_1 | MISO_1 | SCKL_1;
    GPIOA->CRH |= SS_1;

    SPI1->CR1 |= baud;
    SPI1->CR1 |= SSM | MSTR | SSI;
    // SPI1->CR1 |= SPI_CR1_LSBFIRST;
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
    SPI1->CR2 = RXNEIE | ERRIE | SSOE; //| TXEIE;

    NVIC_SetPriority(SPI1_IRQn, 2);
    NVIC_EnableIRQ(SPI1_IRQn);
    SPI1->CR1 |= SPE;
}
uint8_t spi1_send(uint8_t val)
{
    while (!(SPI1->SR & SPI_SR_TXE)) // 0-Full 1-empty
        ;
    SPI1->DR = val;
    while ((SPI1->SR & SPI_SR_BSY)) // 0-free 1-bsy
        ;
    spi_rd = SPI1->DR;
    return (uint8_t)(spi_rd);
}
void spi1_buffer(char buff[], uint16_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        spi1_send(buff[i]);
    }
}
void spi1_stop()
{
    spi0_stop();
}
/////////////////
////// SPI2 /////
void spi2_init(uint8_t baud)
{ // mosi pb15 miso pb14 sck-pb13 ss-pb12 50mhz
    spi2_setup();
    SPI2->CR1 = 0;
    SPI2->CR2 = 0;
    SPI2->CR1 = baud;
    SPI2->CR1 |= SSM | MSTR | SSI;

    SPI2->CR2 = RXNEIE | ERRIE | SSOE;// | TXEIE;
    NVIC_SetPriority(SPI2_IRQn, 2);
    NVIC_EnableIRQ(SPI2_IRQn);
    SPI2->CR1 |= SPE;
}
uint8_t spi2_send(uint8_t val)
{
    while (!(SPI2->SR & SPI_SR_TXE)) // 0-Full 1-empty
        ;
    SPI2->DR = val;
    while ((SPI2->SR & SPI_SR_BSY)) // 0-free 1-bsy
        ;
    spi_rd = SPI2->DR;
    return (uint8_t)(spi_rd);
}
void spi2_buffer(char buff[], uint16_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        spi2_send(buff[i]);
    }
}
void spi2_stop()
{
    NVIC_DisableIRQ(SPI2_IRQn);
    SPI2->CR1 &= ~SPE;
}
//////////////
//// DMA TX//////////
/* DMA
SPI1-TX Channel2
SPI1-RX Channel3

SPI2-TX Channel5
SPI2-RX Channel4
*/
char spi_buff[7];

void spi2_dma_tx_init(uint8_t baud) // SPI1-TX
{
    // RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    spi2_setup();
    SPI2->CR1 = 0;
    SPI2->CR2 = 0;
    SPI2->CR1 = baud;
    SPI2->CR1 |= SSM | MSTR | SSI;
    SPI2->CR1 |= SPE;//******

    SPI2->CR2 |= SPI_CR2_TXDMAEN;
    // DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;
    // DMA1_Channel5->CMAR = (uint32_t)msg;
    // DMA1_Channel5->CNDTR = size;

    DMA1_Channel5->CCR &= ~MEM2MEM; // mem2mem
    DMA1_Channel5->CCR |= CIRC;     // 1-circ
    // DMA1_Channel5->CCR |= MEM2MEM; // mem2mem
    // DMA1_Channel5->CCR &= ~CIRC;   // 1-circ
    DMA1_Channel5->CCR |= MINC;  // mem incr
    DMA1_Channel5->CCR &= ~PINC; // per incr
    DMA1_Channel5->CCR |= DIR;   // 0=peri READ 1=mem READ

    DMA1_Channel5->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // peri/mem
    DMA1_Channel5->CCR |= DMA_CCR_PL_0;                             // high prioty

    DMA1_Channel5->CCR |= TCIEN | TEIEN | HTIEN;
    channel5_ready = 0;

    // NVIC_SetPriority(DMA1_Channel5_IRQn, 3);
    // NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    // DMA1_Channel5->CCR |= DMAEN;
}
void dma_spi_send(char msg[], uint16_t size)
{
    DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;
    DMA1_Channel5->CMAR = (uint32_t)msg;
    DMA1_Channel5->CNDTR = size;
    NVIC_SetPriority(DMA1_Channel5_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    DMA1_Channel5->CCR |= DMAEN;

    while (channel5_ready == 0)
    {
    }

    channel5_ready = 0;
}
void spi2_dma_rx_init(uint32_t baud, const char *msg, uint16_t size) // SPI1-RX
{
    // strcpy(spi_buff, msg);
    // RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // spi2_setup();
    // GPIOA->CRL = (MOSI_2 | MISO_2 | SCKL_2 | SS_2);
    // SPI2->CR1 = 0;
    // SPI2->CR2 = 0;
    SPI2->CR1 = baud;
    // SPI2->CR1 |= SSM | MSTR | SSI;
    // SPI2->CR1 |= SPI_CR1_LSBFIRST;
    DMA1_Channel4->CPAR = (uint32_t)&SPI2->DR;
    DMA1_Channel4->CMAR = (uint32_t)msg;
    DMA1_Channel4->CNDTR = size;
    DMA1_Channel4->CCR |= CIRC;                                 // 1-circ
    DMA1_Channel4->CCR |= MINC;                                 // mem incr
    DMA1_Channel4->CCR &= ~PINC;                                // periph no incr
    DMA1_Channel4->CCR &= ~DIR;                                 // 0=peri READ 1=mem READ
    DMA1_Channel4->CCR &= ~(DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0); // peri/mem size
    DMA1_Channel4->CCR |= DMA_CCR_PL_0;                         // high prioty
    SPI2->CR2 |= SPI_CR2_RXDMAEN;                               // spi1 Tx channel=2
    DMA1_Channel4->CCR |= TCIEN;
    NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    DMA1_Channel5->CCR = DMA_CCR_EN;
}

#endif // _SSPI
