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
// USART_SR Status register
//
// if cts is set softw clr bit=0
#define CTS_FLAG USART_SR_CTS
// data txed to reg.**** single buffer only
#define TXE_FLAG USART_SR_TXE
#define TC_FLAG USART_SR_TC     // tx complete 1=txed 0=not txed
#define RXNE_FLAG USART_SR_RXNE // val @ RDR -> usart_dr
#define ORE_FLAG USART_SR_ORE   // RDR reg is holding prevoius  data
//** noise error crd by rd--SR and rd--DR NB/ doe not raise flag
#define NE_FLAG USART_SR_NE
//**  crd by rd--SR and rd--DR doe not raise flag
#define FE_FLAG USART_SR_FE
//
// USART_CR1  Control register
//
#define EU USART_CR1_UE     // usart Enable
#define M_SIZE USART_CR1_M  // Word length 0==8 1==9
#define WAKE USART_CR1_WAKE // Wake 0==8 1==9
#define PEIE USART_CR1_PEIE
#define TXEIE USART_CR1_TXEIE
#define TCIE USART_CR1_TCIE
#define RXNEIE USART_CR1_RXNEIE
#define TXEN USART_CR1_TE
#define RXEN USART_CR1_RE
#define SBKEN USART_CR1_SBK // send break
//
//  USART_CR2 Control register 2
// enables the capability to send LIN Synch Breaks (13 low bits) using the SBK
#define LINEN USART_CR2_LINEN
#define STOP_1 USART_CR2_STOP
#define STOP_05 USART_CR2_STOP_0
#define STOP_2 USART_CR2_STOP_1
#define STOP_1_5 USART_CR2_STOP_Msk
#define CLKEN USART_CR2_CLKEN
#define CPOL USART_CR2_CPOL
#define CPHA USART_CR2_CPHA
//
//  USART_CR3 Control register 3
// An interrupt is generated whenever CTS=1
#define CTSIE USART_CR3_CTSIE
// data is only transmitted when the CTS input is asserted (tied to 0).***
#define CTSE USART_CR3_CTSE // CTSE
// The RTS output is asserted (tied to 0) when a data can be received.
#define RTSE USART_CR3_RTSE
//
#define DMAEN_TX USART_CR3_DMAT
#define DMAEN_RX USART_CR3_DMAR
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
uint8_t eusart_send(uint8_t val);
void test_eusart();
//
void USART1_IRQHandler()
{
    if (((USART1->SR & ORE_FLAG)))
    {
        rdVal = USART1->SR;
        rdVal = USART1->DR;
    }
    if (((USART1->SR & TC_FLAG))) // 0 not txed
    {
        rdVal = USART1->SR;
        rdVal = USART1->DR;
    }
    if ((USART1->SR & RXNE_FLAG))
    {
        // rdVal = USART1->SR;
        // rdVal = USART1->DR;
    }
    if ((USART1->SR & FE_FLAG))
    {
        rdVal = USART1->SR;
        rdVal = USART1->DR;
    }
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
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP);
    GPIOA->CRH = (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_Msk); // tx 50mhz AF_P_P
    GPIOA->CRH |= (GPIO_CRH_CNF10_0);                    // rx input FLOAT
}
///// DMA ///////
void eusart_dma_init(uint32_t bauds)
{
    usart_pins_init(); /// PINS
    u_baud(bauds);
    USART1->CR2 = STOP_1;
    USART1->CR1 &= ~M_SIZE;
    USART1->CR1 &= ~(TXEIE | TCIE | RXNEIE | RXEN | USART_CR1_UE | TXEN | EU);
    USART1->CR1 |= TXEIE | TCIE | RXNEIE;
    USART1->CR1 |= TXEN | USART_CR1_UE | RXEN;
    DMA1_Channel5->CCR |= DMAEN;
    USART1->CR1 |= EU;
}
void uart_dma1set()
{
    // DMA1->IFCR;
    // DMA1->ISR;

    while ((DMA1->ISR & DMA_ISR_GIF1)) // TE/TC/HT occured
    {
        if ((DMA1->ISR & DMA_ISR_TEIF1)) // tx error
        {
            DMA1->IFCR &= ~(DMA_IFCR_CTEIF1);
        }
        if ((DMA1->ISR & DMA_ISR_HTIF1)) // half txed
        {
            DMA1->IFCR &= ~(DMA_IFCR_CHTIF1);
        }
        if ((DMA1->ISR & DMA_ISR_TCIF1)) // tx complete
        {
            DMA1->IFCR &= ~(DMA_IFCR_CTCIF1);
        }
    }
}
void dma_Rx_ch5(uint32_t phaddr, uint32_t memaddr, uint16_t buffSize)
{
    RCC->APB2ENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CPAR = phaddr;
    DMA1_Channel5->CMAR = memaddr;
    DMA1_Channel5->CPAR = buffSize;
    USART1->CR3 |= DMAEN_RX; // usart1 Tx channel=5

    DMA1_Channel5->CCR = DIR;          // 0=RD-peri 1=Rd mem
    DMA1_Channel5->CCR |= MEMSIZE_8BIT; //**** */
    DMA1_Channel5->CCR |= PSIZE_8BIT;   //**** */
    DMA1_Channel5->CCR |= MINC;         //**** */
    DMA1_Channel5->CCR &= ~PINC;        //**** */
    DMA1_Channel5->CCR |= CIRC;         //**** */
    // DMA1_Channel5->CCR |= MEM2MEM;     //**** */

    DMA1_Channel5->CCR |= TEIEN | HTIEN | TCIE;
    //
    // USART1->CR1 |= TXEIE | TCIE | RXNEIE;
    // USART1->CR1 |= TXEN | USART_CR1_UE | RXEN;
    // DMA1_Channel5->CCR |= DMAEN;
    // USART1->CR1 |= EU;
}
//////////////
//////////////
void eusart_init(uint32_t bauds)
{
    usart_pins_init(); /// PINS
    u_baud(bauds);
    USART1->CR2 = STOP_1;
    USART1->CR1 &= ~M_SIZE;
    USART1->CR1 &= ~(TXEIE | TCIE | RXNEIE | RXEN | USART_CR1_UE | TXEN | EU);

    USART1->CR1 |= TXEIE | TCIE | RXNEIE;
    USART1->CR1 |= TXEN | USART_CR1_UE | RXEN;
    USART1->CR1 |= EU;
    // NVIC_SetPriority(USART1_IRQn, 3);
    // NVIC_EnableIRQ(USART1_IRQn);
}
uint8_t eusart_send(uint8_t val)
{
    USART1->DR = val;
    while ((!(USART1->SR & TC_FLAG))) // 0 not txed
        ;
    rdVal = USART1->DR;
    return rdVal;
}
void eusartString(char *mesg)
{
    char buff[20];
    strcpy(buff, mesg);
    eusart_send(' ');

    for (uint8_t i = 0; i < strlen(mesg) + 1; i++)
    {
        eusart_send(buff[i]);
    }
}
uint8_t eusart_rd()
{
    dummy = USART1->DR;
    dummy = USART1->SR;
    while (!(USART1->SR & RXNE_FLAG))
        ;
    while ((USART1->SR & FE_FLAG))
        ;
    rdVal = USART1->DR;
    return rdVal;
}
//////////////////////
//////////////////////
void eusart_cntrl_init(uint32_t bauds)
{
    usart_pins_init(); /// PINS
    u_baud(bauds);
    USART1->CR2 = STOP_1;
    USART1->CR1 &= ~M_SIZE;
    USART1->CR1 &= ~(TXEIE | TCIE | RXNEIE | RXEN | USART_CR1_UE | TXEN | EU);
    USART1->CR3 |= CTSIE | CTSE | RTSE; //
    USART1->CR1 |= TXEIE | TCIE | RXNEIE;
    USART1->CR1 |= TXEN | USART_CR1_UE | RXEN;
    USART1->CR1 |= EU;
    // NVIC_SetPriority(USART1_IRQn, 3);
    // NVIC_EnableIRQ(USART1_IRQn);
}
void eusart_cntrl_send(uint8_t val)
{
    while (!(USART1->SR & CTS_FLAG)) // 1==change 0==no change
        ;

    USART1->DR = val;
    while ((!(USART1->SR & TC_FLAG))) // 0 not txed
        ;
    rdVal = USART1->DR;
}
uint8_t eusart_cntrl_rd()
{
    USART1->CR3 = RTSE; // 1==>RTS=0
    dummy = USART1->DR;
    dummy = USART1->SR;
    while ((USART1->SR & ORE_FLAG))
        ;
    while (!(USART1->SR & RXNE_FLAG))
        ;
    while ((USART1->SR & FE_FLAG))
        ;
    rdVal = USART1->DR;

    USART1->CR3 &= ~RTSE; // 0==>RTS=1

    return rdVal;
}
//////////////////////
/////////////

void eusart_close()
{
    USART1->CR1 = 0;
    RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN);
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
              eusart_send(i);                   \
                                              \
              _delay_ms(60);                  \
          }                                   \
          eusartString("End ", 4);            \
          eusart_close();                     \
              */
#endif //