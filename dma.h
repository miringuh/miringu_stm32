#if !defined(__DMA)
#define __DMA
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "gpio.h"

#define DMATXEN SET_BIT(USART->CR3, USART_CR3_DMAT)
#define DMARXEN SET_BIT(USART->CR3, USART_CR3_DMAR)
/*
usart1Tx-channel4(mem2mem)
usart1Rx-channel5(mem2mem)

DMA_CPARx peripheral address
DMA_CMARx memory address
DMA_CNDTRx (data size reg)
DMA_CCRx   configuration register
DMA_CCRx -modes(circular,direction,increment/decrement, interrupts etc)

    INTERRUPTS
Half Transfer(HTIF)   ==>HTIE
Complete TX  (TCIF)   ==>TCIE


DMA->ISR (intr flags) < 4:0 >
    < TEIFx >-(0x08) (transfer error flag)
    < HTIFx >-(0x04) (half transfer)
    < TCIFx >-(0x02) (transfer complete)
    < GIFx  >-(0x01) (TE,HT or TC) event occurred

DMA->IFCR (intr error clear ) < 4:0 >
    < CTIEFx >(0x08)
    < CHTIFx >(0x04)
    < CTCIFx >(0x02)
    < CGIFx  >(0x01)

DMA->CCRx (conf reg)
    < MEM2MEM > 1-enabled 0-disabled
    < PL >      [1:0] priority         0-low 1-medium 2-high 3-veryhigh
    < MSIZE >   [1:0] memory bits size     ( 0-8bit 1-16bit 2-32bit)
    < PSIZE >   [1:0] peripheral bits size ( 0-8bit 1-16bit 2-32bit)
    < MINC >    memory INC
    < PINC >    peripheral INC
    < CIRC >    circular Mode
    < DIR >     Direction from 1-memory 0-peripheral
    < TEIE >    TX  INTR enable
    < HEIE >    half TX INTR enable
    < TCIE >    complete INTR enable
    < EN >      Channel enable

DMA->CNDTRx (15:0) number of data to transfer

DMA->CPARx(31:0) peripheral addr reg
DMA->CMARx(31:0) memory addr reg
    */
#define readPeriph 0
#define readMem 1
uint32_t myvar =  0x10000000;
uint32_t mymem = (0x10000000 + 128);

void DMA1_IRQHandler()
{

    if ((DMA1->ISR & DMA_ISR_GIF1) == DMA_ISR_GIF1)
    {
        DMA1->IFCR = DMA_IFCR_CGIF1;
    }
}

void dmaRun(uint32_t phaddr, uint32_t memaddr, uint8_t dir, uint16_t buffSize)
{
    // NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    // NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
    // RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel1->CCR &= ~DMA_CCR_MINC;
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
    DMA1_Channel1->CCR |= (DMA_CCR_PL_1 | DMA_CCR_CIRC);

    DMA1_Channel1->CPAR = (uint32_t)&phaddr;
    DMA1_Channel1->CMAR = (uint32_t)&memaddr;
    DMA1_Channel1->CNDTR = buffSize;
    if (dir == readPeriph)
    {
        DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
    }
    if (dir == readMem)
    {
        DMA1_Channel1->CCR |= DMA_CCR_DIR;
    }
    DMA1_Channel1->CCR |= (DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_TEIE);
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    while ((DMA1->ISR & DMA_ISR_HTIF1) == DMA_ISR_HTIF1)
    {
        DMA1->IFCR = DMA_IFCR_CHTIF1;
    }
}
//
/*
void dmaUart(uint32_t phaddr, uint32_t memaddr, uint8_t dir, uint16_t buffSize)
{

    // RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // setAF_CRL();

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMA1_Channel1->CPAR = (uint32_t)&phaddr;
    DMA1_Channel1->CMAR = (uint32_t)&memaddr;
    DMA1_Channel1->CNDTR = buffSize;
    if (dir == readPeriph)
    {
        DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
    }
    if (dir == readMem)
    {
        DMA1_Channel1->CCR = DMA_CCR_DIR;
    }
    DMA1_Channel1->CCR &= ~DMA_CCR_MINC;
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1;
    DMA1_Channel1->CCR |= (DMA_CCR_PL_1 | DMA_CCR_CIRC);
    DMA1_Channel1->CCR |= (DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_TEIE);

    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    while ((DMA1_Channel1->CCR & DMA_CCR_EN) != DMA_CCR_EN)
    {
        DMA1_Channel1->CCR |= DMA_CCR_EN;
    }
}
*/
#endif // __DMA
