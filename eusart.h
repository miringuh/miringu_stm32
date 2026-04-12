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
#include "tim1.h"

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
#define USART_TXEIE USART_CR1_TXEIE
#define TCIE USART_CR1_TCIE
#define USART_RXNEIE USART_CR1_RXNEIE
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
#define USART_CPOL USART_CR2_CPOL
#define USART_CPHA USART_CR2_CPHA
//
//  USART_CR3 Control register 3
// An interrupt is generated whenever CTS=1
#define CTSIE USART_CR3_CTSIE
// data is only transmitted when the CTS input is asserted (tied to 0).***
#define CTSE USART_CR3_CTSE // CTSE enable
// The RTS output is asserted (tied to 0) when a data can be received.
#define RTSE USART_CR3_RTSE // RTS Enable
//
#define EIE USART_CR3_EIE // Error Interrupt Enable
//
#define DMAT USART_CR3_DMAT // DMA Enable Transmitter
#define DMAR USART_CR3_DMAR // DMA Enable receiver
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
volatile uint8_t dummy;
volatile uint8_t rdVal;
volatile uint8_t wrVal;
uint8_t eusart_send(uint8_t val);
void test_eusart();
//
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
    // _delay_ms(600000);
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

void usart1_pins_remap0() // tx-PA9 rx-PA10
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP);
    GPIOA->CRH |= (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0); // tx 10mhz AF_P_P
    GPIOA->CRH |= (GPIO_CRH_CNF10_0);                   // rx input float
}
void usart1_pins_remap1() // tx-PB6  rx-PB7
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
    GPIOB->CRL |= (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1); // tx 10mhz AF_P_P
    GPIOB->CRL |= (GPIO_CRL_CNF7_0);                   // rx input FLOAT
}
/*
     (master)  TX----->RX (slave)
     (master)  RX<-----TX (slave)
     (master) RTS----->CTS (slave) if rts=0 send rts=1 read
     (master) CTS<-----RTS (slave) if cts=0 read cts=1 send
*/
void usart2_pins_remap2() // tx-PA2  rx-PA3 cts-PA0 rts-PA1
{

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    AFIO->MAPR &= ~AFIO_MAPR_USART2_REMAP;

    GPIOA->CRL |= (GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_Msk); // tx 50mhz AF_P_P
    GPIOA->CRL |= (GPIO_CRL_MODE1_Msk);                  // rts 50mhz P_P
    GPIOA->CRL |= (GPIO_CRL_CNF3_0);                     // rx input FLOAT
    GPIOA->CRL |= (GPIO_CRL_CNF0_0);                     // cts input FLOAT
}
void usart3_pins_remap3() // tx-PB10  rx-PB11 cts-PB13 rts-PB14
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    AFIO->MAPR &= ~AFIO_MAPR_USART2_REMAP;
    GPIOB->CRH = (GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_Msk); // tx 50mhz AF_P_P
    GPIOB->CRH |= (GPIO_CRH_MODE14_Msk);                   // rts 50mhz P_P
    GPIOB->CRH |= (GPIO_CRH_CNF11_0);                      // rx input FLOAT
    GPIOB->CRH |= (GPIO_CRH_CNF13_0);                      // cts input FLOAT
}

///////////////////////
void USART1_IRQHandler()
{
    if ((USART1->SR & TC_FLAG)) // 1- txed
    {
        USART1->SR &= ~TC_FLAG;
    }
    if ((USART1->SR & RXNE_FLAG)) // 1- txed
    {
        USART1->SR &= ~RXNE_FLAG;
    }
    // dummy = USART1->SR;
    // if ((USART1->SR & TXE_FLAG)) // 1- txed
    // {
    //     dummy = USART1->SR;
    // }
}
////// USART1_0 // tx-PA9 rx-PA10 //////
void eusart_init(uint32_t bauds)
{
    usart1_pins_remap0(); /// PINS
    USART1->CR1 = 0;
    u_baud(bauds);
    USART1->CR2 = STOP_1;

    // USART1->CR3 = EIE;
    USART1->CR1 = TCIE | USART_RXNEIE; //| USART_TXEIE;
    USART1->CR1 |= TXEN | RXEN;
    USART1->CR1 |= EU;

    // NVIC_SetPriority(USART1_IRQn, 2);
    // NVIC_EnableIRQ(USART1_IRQn);
}
uint8_t eusart_send(uint8_t val)
{
    // wrVal = val;
    USART1->DR = val;
    while (!(USART1->SR & TXE_FLAG)) // 1 DR-->>reg
    {
    }
    dummy = USART1->SR;
    return USART1->DR;
}
void eusartString(char *mesg)
{
    char buff[20];
    strcpy(buff, mesg);
    // eusart_send(' ');
    for (uint8_t i = 0; i < strlen(mesg) + 1; i++)
    {
        eusart_send(buff[i]);
    }
}
uint8_t eusart_rd()
{
    while (!(USART1->SR & RXNE_FLAG)) // 0=not recvd
        ;
    rdVal = USART1->DR;
    while ((USART1->SR & FE_FLAG)) // 1=error
        ;
    while ((USART1->SR & NE_FLAG)) // 1=noise
        ;
    dummy = USART1->SR;
    return rdVal;
}
//////////////////////////
//////// USART1_1 // tx-PB6  rx-PB7
void eusart_init_1(uint32_t bauds)
{
    usart1_pins_remap1(); /// PINS
    USART1->CR1 = 0;
    u_baud(bauds);
    USART1->CR2 = STOP_1;

    // USART1->CR3 = EIE;
    USART1->CR1 = TCIE | USART_RXNEIE; //| USART_TXEIE;
    USART1->CR1 |= TXEN | RXEN;
    USART1->CR1 |= EU;

    // NVIC_SetPriority(USART1_IRQn, 2);
    // NVIC_EnableIRQ(USART1_IRQn);
}
uint8_t eusart_send_1(uint8_t val)
{
    return eusart_send(val);
}
void eusartString_1(char *mesg)
{
    eusartString(mesg);
}
void eusart_rd1()
{
    eusart_rd();
}
//////////////////////////////////////////////////////
////// USART 2 //tx-PA2  rx-PA3 cts-PA0 rts-PA1
void u_baud2(uint32_t baud)
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
        USART2->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U19200:
        USART2->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U38400:
        USART2->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U57600:
        USART2->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U115200:
        USART2->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    default:
        break;
    }
}
void eusart_init_2(uint32_t bauds)
{
    usart2_pins_remap2(); /// PINS
    u_baud2(bauds);
    USART2->CR2 = STOP_1;
    USART2->CR1 = 0;
    USART2->CR3 &= ~(CTSIE | CTSE | RTSE);
    //
    USART2->CR1 |= TCIE; //| USART_TXEIE | USART_RXNEIE;
    //
    USART2->CR3 = CTSIE | CTSE | RTSE;
    //
    USART2->CR1 |= TXEN | RXEN;
    USART2->CR1 |= EU;
    // NVIC_SetPriority(USART1_IRQn, 3);
    // NVIC_EnableIRQ(USART1_IRQn);
}
uint8_t eusart_send_2(uint8_t val)
{
    // wrVal = val;
    USART2->DR = val;
    while (!(USART2->SR & TXE_FLAG)) // 1 DR-->>reg
    {
    }
    dummy = USART2->SR;
    return USART2->DR;
}
uint8_t eusart_rd_2()
{
    while (!(USART2->SR & RXNE_FLAG)) // 0=not recvd
        ;
    rdVal = USART2->DR;
    while ((USART2->SR & FE_FLAG)) // 1=error
        ;
    while ((USART2->SR & NE_FLAG)) // 1=noise
        ;
    dummy = USART2->SR;
    return rdVal;
}
void eusartString_2(char *mesg)
{
    char buff[20];
    strcpy(buff, mesg);
    eusart_send_2(' ');
    for (uint8_t i = 0; i < strlen(mesg) + 1; i++)
    {
        eusart_send_2(buff[i]);
    }
}
void eusart_cntrl_2(uint8_t val)
{
    if (!(USART2->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
    {
        goto read;
    }
    if ((USART2->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
    {
        goto send;
    }

read:
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
    eusart_send_2(eusart_rd_2());
    GPIOA->ODR = GPIO_ODR_ODR1; // rts=1 rd
send:
    GPIOA->ODR = GPIO_ODR_ODR1; // rts=1 rd
    eusart_send_2(val);
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
}
/////////////////////////
/////// USART 3 //tx-PB10  rx-PB11 cts-PB13 rts-PB14
void u_baud3(uint32_t baud)
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
        USART3->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U19200:
        USART3->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U38400:
        USART3->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U57600:
        USART3->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    case U115200:
        USART3->BRR = ((mantissa << 4) + (uint32_t)div_frac);
        break;
    default:
        break;
    }
}
void eusart_init_3(uint32_t bauds)
{
    usart3_pins_remap3(); /// PINS
    u_baud3(bauds);
    USART3->CR2 = STOP_1;
    USART3->CR1 &= ~(USART_TXEIE | TCIE | USART_RXNEIE | RXEN | TXEN | EU | M_SIZE);
    USART3->CR3 &= ~(CTSIE | CTSE | RTSE);
    //
    USART3->CR1 |= USART_TXEIE | TCIE | USART_RXNEIE;
    //
    USART3->CR3 = CTSIE | CTSE | RTSE;
    //
    USART3->CR1 |= TXEN | RXEN;
    USART3->CR1 |= EU;
    // NVIC_SetPriority(USART1_IRQn, 3);
    // NVIC_EnableIRQ(USART1_IRQn);
}
uint8_t eusart_send_3(uint8_t val)
{
    USART3->DR = val;
    while (!(USART3->SR & TXE_FLAG)) // 1 DR-->>reg
    {
    }
    dummy = USART3->SR;
    return USART3->DR;
}
uint8_t eusart_rd_3()
{
    while (!(USART3->SR & RXNE_FLAG)) // 0=not recvd
        ;
    rdVal = USART3->DR;
    while ((USART3->SR & FE_FLAG)) // 1=error
        ;
    while ((USART3->SR & NE_FLAG)) // 1=noise
        ;
    dummy = USART3->SR;
    return rdVal;
}
void eusartString_3(char *mesg)
{
    char buff[20];
    strcpy(buff, mesg);
    eusart_send_3(' ');
    for (uint8_t i = 0; i < strlen(mesg) + 1; i++)
    {
        eusart_send_3(buff[i]);
    }
}
//************
void eusart3_cntrl_send(uint8_t val)
{
    while (!(USART3->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
        ;
    GPIOA->ODR = GPIO_ODR_ODR1; // rts=1 rd
    eusart_send_3(val);
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
}
void eusart3_cntrl_read()
{
    while (!(USART3->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
        ;
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
    eusart_send_3(eusart_rd_3()); //***********
    GPIOA->ODR = GPIO_ODR_ODR1;   // rts=1 rd
}
void eusart_cntrl_3(uint8_t val)
{
    // GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
    // GPIOA->ODR = GPIO_ODR_IDR1;   // rts=1 rd
    if (!(USART3->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
    {
        goto read;
    }
    if ((USART3->SR & USART_SR_CTS)) // CTS if 1-send if 0-read
    {
        goto send;
    }
read:
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
    eusart_send_3(eusart_rd_3());
    GPIOA->ODR = GPIO_ODR_ODR1; // rts=1 rd
send:
    GPIOA->ODR = GPIO_ODR_ODR1; // rts=1 rd
    eusart_send_3(val);
    GPIOA->ODR &= ~GPIO_ODR_ODR1; // rts=0 wr
}
///////////
void eusart_close()
{
    USART1->CR1 = 0;
    RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN);
}
/////////////////
///// DMA ///////
/* DMA TX OR RX
usart1- TX=channel 4
usart1- RX=channel 5

usart2- TX=channel 7
usart2- RX=channel 6

usart3- TX=channel 2
usart3- RX=channel 3

*/
uint8_t valData;
char buff[255];
volatile uint8_t state;
//
void DMA1_Channel4_IRQHandler() // tx
{
    if ((DMA1->ISR & DMA_ISR_HTIF4)) // half txed
    {
        DMA1->IFCR |= DMA_IFCR_CHTIF4;
    }
    if ((DMA1->ISR & DMA_ISR_TCIF4)) // tx complete
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF4;
    }
    if ((DMA1->ISR & DMA_ISR_TEIF4)) // tx error
    {
        DMA1->IFCR |= DMA_IFCR_CTEIF4;
    }
    DMA1_Channel4->CCR &= ~DMAEN;
}
void DMA1_Channel5_IRQHandler() // rx
{
    if ((DMA1->ISR & DMA_ISR_TCIF5)) // tx complete
    {
        state = 1;
        DMA1->IFCR |= DMA_IFCR_CTCIF5;
    }
}
//
void eusart_dma_tx_init(uint32_t baud, const char *msg)
{
    strcpy(buff, msg);
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    usart1_pins_remap0();
    u_baud(baud);
    USART1->CR1 = 0;
    USART1->CR2 = STOP_1;
    DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel4->CMAR = (uint32_t)&buff;
    DMA1_Channel4->CNDTR = (strlen(msg) * 2);
    DMA1_Channel4->CCR |= CIRC;                                     // 1-circ
    DMA1_Channel4->CCR |= MINC;                                     // mem incr
    DMA1_Channel4->CCR &= ~PINC;                                    // periph no incr
    DMA1_Channel4->CCR |= DIR;                                      // 0=peri READ 1=mem READ
    DMA1_Channel4->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // peri/mem size
    DMA1_Channel4->CCR |= DMA_CCR_PL_0;                             // high prioty
    USART1->CR3 |= DMAT;                                            // usart1 Tx channel=4

    DMA1_Channel4->CCR |= TEIEN | HTIEN | TCIEN;
    NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    USART1->CR1 |= TXEN | EU;
    DMA1_Channel4->CCR |= DMAEN;
}
// eusart_buff[i] = i | 0x30;
void eusart_dma_tx2_init(uint32_t baud, char msg[])
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    usart1_pins_remap1();
    u_baud(baud);
    USART1->CR1 = 0;
    USART1->CR2 = STOP_1;
    DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel4->CMAR = (uint32_t)msg;
    DMA1_Channel4->CNDTR = (strlen(msg) * 2);
    DMA1_Channel4->CCR |= CIRC;                                     // 1-circ
    DMA1_Channel4->CCR |= MINC;                                     // mem incr
    DMA1_Channel4->CCR &= ~PINC;                                    // periph no incr
    DMA1_Channel4->CCR |= DIR;                                      // 0=peri READ 1=mem READ
    DMA1_Channel4->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // peri/mem size
    DMA1_Channel4->CCR |= DMA_CCR_PL_0;                             // high prioty
    USART1->CR3 |= DMAT;                                            // usart1 Tx channel=4

    DMA1_Channel4->CCR |= TEIEN | HTIEN | TCIEN;
    NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    USART1->CR1 |= TXEN | EU;
    DMA1_Channel4->CCR |= DMAEN;
}
/////////////////
void eusart_dma_rx_init(uint32_t bauds, char msg[], uint16_t size)
{
    usart1_pins_remap1(); /// PINS
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    u_baud(bauds);
    USART1->CR2 = STOP_1;
    USART1->CR1 = 0;
    // USART1->CR1 |= USART_TXEIE | TCIE | USART_RXNEIE;
    USART1->CR1 |= RXEN | TXEN | USART_CR1_UE;
    USART1->CR3 |= DMAR; // usart1 Rx channel=5

    DMA1_Channel5->CCR = 0;

    DMA1_Channel5->CCR &= ~DMAEN;
    DMA1_Channel5->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR = (uint32_t)&msg;
    DMA1_Channel5->CNDTR = size;
    DMA1_Channel5->CCR |= DMA_CCR_PL_1; // priority HIGH
    DMA1_Channel5->CCR &= ~DIR;         // 0=peri READ 1=mem READ
    // DMA1_Channel5->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // 8BIT
    DMA1_Channel5->CCR |= MINC;  // mem incr
    DMA1_Channel5->CCR &= ~PINC; // periph no incr
    DMA1_Channel5->CCR |= CIRC;  // 1-circ
    DMA1_Channel5->CCR |= TCIEN;
    state = 0;
    DMA1_Channel5->CCR |= DMAEN;
    NVIC_SetPriority(DMA1_Channel5_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}
//
#endif

/*
#define txBuffSize 12
char eusart_buff[txBuffSize];
uint8_t cnt = 0;
int main()
{
    clock_init_20mhz_apb();
    // pllInit();
    SysTick_Init();
    _delay_ms(1000000);
    // eusart_init(U19200);
    eusart_dma_rx_init(U19200, eusart_buff, 12);
    eusart_dma_tx_init(U19200, "welcome Home");

    while (1)
    {
        if (state)
        {
            USART1->CR1 |= USART_CR1_UE;
            state = 0;
            for (int i = 0; i < txBuffSize; i++)
            {
                eusart_send(eusart_buff[i]);
            }
        }

    }
    return 0;
}

*/