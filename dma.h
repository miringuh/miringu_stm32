#if !defined(__DMA)
#define __DMA
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "gpio.h"
// #include "eusart.h"
/*
usart1Tx-channel4(mem2mem)
usart1Rx-channel5(mem2mem)

DMA_CPARx peripheral address
DMA_CMARx memory address
DMA_CNDTRx (data size reg)
DMA_CCRx   configuration register
DMA_CCRx -modes(circular,direction,increment/decrement, interrupts etc)

    INTERRUPTS
Half Transfer(HTIF)   ==>HTIEN
Complete TX  (TCIF)   ==>TCIE
DMA Transfer Error

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
    < TEIEN >    TX  INTR enable
    < HEIE >    half TX INTR enable
    < TCIE >    complete INTR enable
    < EN >      Channel enable

DMA->CNDTRx (15:0) number of data to transfer

DMA->CPARx(31:0) peripheral addr reg
DMA->CMARx(31:0) memory addr reg
    */
//       DMA->ISR DMA interrupt status register
//[ TEIFx HTIFx TCIFx xGIF7 ]
#define CHANNEL_FLAGS(REG, POS) (READ_REG(REG)  << POS))
//
//      (DMA_IFCR) DMA interrupt flag clear register
//[ TEIFx HTIFx TCIFx xGIF7 ]
#define CHANNEL_FLAG_CLR(REG, BIT) SET_BIT(REG, BIT)
//
//      (DMA_CCRx) DMA channel x configuration register
#define MEM2MEM DMA_CCR_MEM2MEM
#define PL(REG, VAL) SET_BIT(REG, VAL)      // 00-low 01-mid 10-high 11-very high
#define MEMSIZE(REG, VAL) SET_BIT(REG, VAL) // 00-8bit 01-16bit 10-32bit
#define PSIZE(REG, VAL) SET_BIT(REG, VAL)   // 00-8bit 01-16bit 10-32bit
#define MINC DMA_CCR_MINC
#define PINC DMA_CCR_PINC
#define CIRC DMA_CCR_CIRC
#define DIR DMA_CCR_DIR // 0=peri READ 1=mem READ
#define TEIEN DMA_CCR_TEIE
#define HTIEN DMA_CCR_HTIE
#define TCIEN DMA_CCR_TCIE
#define DMAEN DMA_CCR_EN
//
// (DMA_CNDTRx)  DMA channel x number of data register
#define DATA_SIZE(REG, VAL) WRITE_REG(REG, VAL) // Number of data to transfer
//
// (DMA_CPARx)  DMA channel x peripheral address register
#define PERIPH_ADDR(REG, VAL) WRITE_REG(REG, VAL) // data register from/to which the data will be read/written.
//
// (DMA_CMARx) DMA channel x memory address register
#define MEM_ADDR(REG, VAL) WRITE_REG(REG, VAL) // memory area from/to which the data will be read/written.
//
// NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
// NVIC_EnableIRQ(DMA1_Channel1_IRQn);
//
/* DMA1 has 7 channels
channel 1== ADC1, TIM2_CH3, TIM4_CH1
channel 2== SPI1_RX, USART3_TX, TIM1_CH1, TIM2_UP, TIM3_CH3
channel 3== SPI1_TX, USART3_RX, TIM3_CH4, TIM3_UP,
channel 4== SPI2_RX, USART1_TX, I2S2_RX, I2C2_TX, TIM1_CH4, TIM1_TRIG, TIM1_COM TIM4_CH2
channel 5== SPI2_TX, I2SC2_TX, USART1_RX, I2C2_RX, TIM1_UP, TIM2_CH1 TIM4_CH3
channel 6== USART2_RX, I2C1_TX, TIM1_CH3, TIM3_CH1, TIM3_TRIG
channel 7== USART2_TX, I2C1_RX, TIM2_CH2, TIM2_CH4, TIM4_UP
*/
// DMA2 has 5 channels
//

// void DMA1_IRQHandler()
// {
// }

void channel1(uint32_t phaddr, uint32_t memaddr, uint16_t buffSize)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel1->CCR &= ~DMAEN;

    DMA1_Channel1->CPAR = (uint32_t)phaddr;
    DMA1_Channel1->CMAR = (uint32_t)memaddr;
    DMA1_Channel1->CNDTR = buffSize;
    DMA1_Channel1->CCR |= DMA_CCR_PL_0;    // priority
    DMA1_Channel1->CCR |= DIR;             // 0=peri READ 1=mem READ
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; // 8BIT
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; // 8BIT
    DMA1_Channel1->CCR |= MINC;            // mem incr
    DMA1_Channel1->CCR &= ~PINC;           // periph no incr
    DMA1_Channel1->CCR &= ~CIRC;           // 1-circ
    // DMA1_Channel1->CCR |= TEIEN | HTIEN | TCIEN;
    DMA1_Channel1->CCR |= DMAEN;
}
void dma1set()
{
    // DMA1->IFCR;
    // DMA1->ISR

    //     ;
    while ((DMA1->ISR & DMA_ISR_GIF5)) // TE/TC/HT occured
    {
        if ((DMA1->ISR & DMA_ISR_TEIF5)) // tx error
        {
            DMA1->IFCR &= ~(DMA_IFCR_CTEIF5);
        }
        if ((DMA1->ISR & DMA_ISR_HTIF5)) // half txed
        {
            DMA1->IFCR &= ~(DMA_IFCR_CHTIF5);
        }
        if ((DMA1->ISR & DMA_ISR_TCIF5)) // tx complete
        {
            DMA1->IFCR &= ~(DMA_IFCR_CTCIF5);
        }
    }
}
#endif // __DMA
