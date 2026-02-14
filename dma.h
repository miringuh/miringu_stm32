#if !defined(__DMA)
#define __DMA
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "gpio.h"

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
    < TEIE >    TX  INTR enable
    < HEIE >    half TX INTR enable
    < TCIE >    complete INTR enable
    < EN >      Channel enable

DMA->CNDTRx (15:0) number of data to transfer

DMA->CPARx(31:0) peripheral addr reg
DMA->CMARx(31:0) memory addr reg
    */
// DMA->ISR DMA interrupt status register
//   cleared by software writing 1 DMA_IFCR register.
//[ TEIFx HTIFx TCIFx xGIF7 ]
#define CHANNEL_FLAGS(REG, CHN, POS) ((READ_REG(REG) << CHN) << POS)
//
//  (DMA_IFCR) DMA interrupt flag clear register
//[ TEIFx HTIFx TCIFx xGIF7 ]
#define CHANNEL_FLAG_CLR(REG, BIT, POS) (SET_BIT(REG, BIT) << POS)
//
//(DMA_CCRx) DMA channel x configuration register
#define MEM2MEM DMA_CCR_MEM2MEM
#define PL_LOW DMA_CCR_PL_0
#define PL_MID DMA_CCR_PL_1
#define PL_HIGH DMA_CCR_PL_Msk
#define MEMSIZE_8BIT DMA_CCR_MSIZE
#define MEMSIZE_16BIT DMA_CCR_MSIZE_0
#define MEMSIZE_32BIT DMA_CCR_MSIZE_1
#define PSIZE_8BIT DMA_CCR_PSIZE
#define PSIZE_16BIT DMA_CCR_PSIZE_0
#define PSIZE_32BIT DMA_CCR_PSIZE_1
#define MINC DMA_CCR_MINC
#define PINC DMA_CCR_PINC
#define CIRC DMA_CCR_CIRC
#define DIR DMA_CCR_DIR // 0=READ 1=WRITE
#define TEIE DMA_CCR_TEIE
#define HTIE DMA_CCR_HTIE
#define TCIE DMA_CCR_TCIE
#define EN DMA_CCR_EN
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
//
#define readPeriph 0
#define readMem 1
uint32_t myvar = 0x10000000;
uint32_t mymem = (0x10000000 + 128);
struct setch1()
{
    uint32_t phaddr;
    uint32_t memaddr;
    uint16_t buffSize;
    uint8_t dir;
};
void DMA1_IRQHandler()
{
}
typedef setch1 ch1;
ch1 setChannel()
{
    struct setch1 ch1;
    // DMA->CCR1 = DMA_CCR_DIR;
    ch1.dir = 2;

    return ch1;
}

void channel1()
{
    // setChannel();

    // PERIPH_ADDR(DMA1->CPAR1, phaddr);
    // MEM_ADDR(DMA1->CMAR1, memaddr);
    // DATA_SIZE(DMA1->CNDTR1, buffSize);
    // DMA1->CCR1 = PL_MID;
}

#endif // __DMA
