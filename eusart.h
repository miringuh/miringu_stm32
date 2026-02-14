#if !defined(__EUSART)
#define __EUSART
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include "gpio.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include "gpio.h"
#include "rcc_conf.h"
#include "dma.h"

/*
STATUS -- USART_SR
DATA-REG --USART_DR
BAUDRATE --USART_BRR
Tx/ Rx baud =fCK/16*USARTDIV)
USARTDIV===USART_BRR register.
Input clock to the peripheral (PCLK1 for USART2, 3, 4, 5 or PCLK2 for USART1)

TX Procedure:
1.Enable the USART by writing the UE bit in USART_CR1 register to 1.
2.Program the M bit in USART_CR1 to define the word length.
3.Program the number of stop bits in USART_CR2.
4.Select DMA enable (DMAT) in USART_CR3 if Multi euBuffer Communication is to take
place. Configure the DMA register as explained in multi-euBuffer communication.
5.Set the TE bit in USART_CR1 to send an idle frame as first transmission.
6.Select the desired baud rate using the USART_BRR register.
7.Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this
for each data to be transmitted in case of single euBuffer.
//
TXIE-->TXE (a write to USART_DR-->>TXE)==set
TCIE-->TC transmission complete)==set can be clrd by tc= 0

RX Procedure:
1.Enable the USART by writing the UE bit in USART_CR1 register to 1.
2.Program the M bit in USART_CR1 to define the word length.
3.Program the number of stop bits in USART_CR2.
4.Select DMA enable (DMAT) in USART2_CR3 if Multi-euBuffer Communication is to take
place. Configure the DMA register as explained in multi-euBuffer communication. STEP 3
5.Select the desired baud rate using the baud rate register USART_BRR
6.Set the RE bit USART_CR1. This enables the receiver which begins searching for a
start bit.

baud=fclk/16*(USARTDIV)
USARTDIV=fclk/(baudx16)
*/
//
#define USART_FCLK 20000000
#define U9600 9600
#define U19200 19200
#define U38400 38400
#define U57600 57600
#define U115200 115200

//
#define readbuf 0
#define writebuf 1
uint8_t dummy;
volatile uint8_t rdVal;
void test_eusart();
//
// void USART1_IRQHandler()
// {
//     USART1->SR &= !USART_SR_TC;
//     dummy = USART1->DR;
// }
void channel1(uint32_t phaddr, uint32_t memaddr, uint16_t buffSize, uint8_t dir)
{

    DMA1_Channel1->CPAR = phaddr;
    DMA1_Channel1->CMAR = memaddr;
    DMA1_Channel1->CPAR = buffSize;

    DMA1_Channel1->CCR = PL_MID; //*****
    if (dir == 1)
    {
        DMA1_Channel1->CCR |= DIR; // 0=RD 1=WR
    }
    if (dir == 0)
    {
        DMA1_Channel1->CCR &= ~DIR; // 0=RD 1=WR
    }
    DMA1_Channel1->CCR = MEMSIZE_8BIT; //**** */
    DMA1_Channel1->CCR = PSIZE_8BIT;   //**** */
    DMA1_Channel1->CCR &= ~MINC;       //**** */
    DMA1_Channel1->CCR &= ~PINC;       //**** */
    DMA1_Channel1->CCR &= ~CIRC;       //**** */
    DMA1_Channel1->CCR = MEM2MEM;      //**** */

    DMA1_Channel1->CCR |= TEIE | HTIE | TCIE;
    DMA1_Channel1->CCR = DMAEN;
}
void test_eusart()
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
void u_baud(uint32_t baud)
{
    /*
    USARTDIV
           fraction = 16x0.nn
        div_mantisa = val //whole num
                BRR = (div_mantissa<<4|(16x0.nn) )
    */
    float div = ((USART_FCLK / (baud * 16.0)));
    float div1 = ((USART_FCLK / (baud * 16)));
    float div_frac = (16 * (div - div1));
    uint32_t mantissa = (uint32_t)div1;
    switch (baud)
    {
    case U9600:
        USART1->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U19200:
        USART1->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U38400:
        USART1->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U57600:
        USART1->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U115200:
        USART1->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    default:
        break;
    }
}
void usart_pins_init()
{
    AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP);
    GPIOA->CRH = (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0); // tx 10mhz AF_P_P
    GPIOA->CRH |= (GPIO_CRH_CNF10_0);                  // rx input FLOAT
}
//
void eusart_init_dma(uint32_t bauds)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    usart_pins_init(); /// PINS
    // NVIC_EnableIRQ(USART1_IRQn);
    // NVIC_SetPriority(USART1_IRQn, 4);
    u_baud(bauds);
    USART1->CR1 = (USART_CR1_TXEIE | USART_CR1_TCIE | USART_CR1_RXNEIE);
    USART1->CR1 |= (USART_CR1_RE | USART_CR1_UE);
    USART1->CR1 |= USART_CR1_TE;
    USART1->DR = 0;
}
void eusart_init(uint32_t bauds)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    usart_pins_init(); /// PINS
    // NVIC_EnableIRQ(USART1_IRQn);
    // NVIC_SetPriority(USART1_IRQn, 4);
    u_baud(bauds);
    USART1->CR1 = (USART_CR1_TXEIE | USART_CR1_TCIE | USART_CR1_RXNEIE);
    USART1->CR1 |= (USART_CR1_RE | USART_CR1_UE);
    USART1->CR1 |= USART_CR1_TE;
    USART1->DR = 0;
}
//
uint8_t eusart_io(uint8_t val)
{
    GPIOC->BSRR = GPIO_BSRR_BR13;

    USART1->DR = val;
    while (((USART1->SR & USART_SR_TC) != USART_SR_TC) && ((USART1->SR & USART_SR_TXE) != USART_SR_TXE)) // 0 not txed
        ;
    rdVal = USART1->DR;

    while ((USART1->SR & USART_SR_FE) == USART_SR_FE)
    {
        rdVal = USART1->DR;
    }
    while ((USART1->SR & USART_SR_IDLE) == USART_SR_IDLE)
    {
        rdVal = USART1->DR;
    }
    return rdVal;
}
void eusartString(char *mesg)
{
    char buff[20];
    strcpy(buff, mesg);
    eusart_io(' ');

    for (uint8_t i = 0; i < strlen(mesg) + 1; i++)
    {
        eusart_io(buff[i]);
    }
}

//
uint8_t eusart_rd()
{
    while ((USART1->SR & USART_SR_FE) != USART_SR_FE)
        ;
    if ((USART1->SR & USART_SR_NE) != USART_SR_NE)
    {
        dummy = USART1->DR;
    }
    while ((USART1->SR & USART_SR_RXNE) != USART_SR_RXNE)
        ;
    rdVal = USART1->DR;
    return rdVal;
}
uint8_t eusart_read()
{
    rdVal = USART1->DR;
    while ((USART1->SR & USART_SR_FE) == USART_SR_FE)
    {
        rdVal = USART1->DR;
    }
    while ((USART1->SR & USART_SR_IDLE) == USART_SR_IDLE)
    {
        rdVal = USART1->DR;
    }
    return rdVal;
}

void eusart_close()
{
    USART1->CR1 = 0;
}

/*                               \
pllInit();                              \
          // hseInit();                       \
          SysTick_Init();                     \
                                              \
                                              \
          eusart_init(U19200);                \
          eusartString(" Welcome Again", 14); \
          for (uint8_t i = 0; i < 255; i++)   \
          {                                   \
              eusart_io(i);                   \
                                              \
              _delay_ms(60);                  \
          }                                   \
          eusartString("End ", 4);            \
          eusart_close();                     \
              */
#endif //