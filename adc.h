#if !defined(__ADC)
#define __ADC
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include "eusart.h"
//
// (RCC_CFGR) ADPREPRESC
#define ADPRE_PRESC2 RCC_CFGR_ADCPRE_DIV2
#define ADPRE_PRESC4 RCC_CFGR_ADCPRE_DIV4
#define ADPRE_PRESC6 RCC_CFGR_ADCPRE_DIV6
#define ADPRE_PRESC8 RCC_CFGR_ADCPRE_DIV8
//
// ADC->SR
// regular ch clrd by softw 1==started
#define STRT_FLAG ADC_SR_STRT
// reg ch end conv 1==comp
#define EOC_FLAG ADC_SR_EOC
//      ADC->CR1
/*
–These bits are reserved in ADC2.
–In dual mode, a change of channel configuration generates a restart that can
produce a loss of synchronization. It is recommended to disable dual mode
before any configuration change.
*/
//      dual-modes  0110: Regular simultaneous mode only
#define DUALMOD(REG, VAL) WRITE_REG(REG, VAL) // xxx

/*DISCNUM define the num of regular channels to be converted
 in discontinuous mode, after receiving an external trigger*/
#define DISCNUM(REG, BIT) WRITE_REG(REG, BIT) // DISCNUM[2:0] 0...0x07
#define DISCEN ADC_CR1_DISCEN                 // enable/disable Discontinuous mode

#define SCAN_MODE ADC_CR1_SCAN // enable/disable Scan mode
#define EOCIE ADC_CR1_EOCIE    // enable/disable the End of Conversion interrupt
//
//         ADC->CR2
// temp sensor vref channel Enable
#define TEMP_SEN_VREF_EN ADC_CR2_TSVREFE
/* start conversion and cleared by hardware as soon as conversion starts.
if SWSTART is selected as trigger event by the EXTSEL[2:0] bits
*/
#define SWSTART ADC_CR2_SWSTART
// enable/disable the external trigger used to start conversion
#define EXTTRIG ADC_CR2_EXTTRIG
// External event select for regular group
#define EXTSEL(REG, BIT) SET_BIT(REG, BIT) // 111: SWSTART
// Data alignment
#define ALIGN ADC_CR2_ALIGN // 1-LEFT  0-RIGHT
// Reset calibration
#define RSTCAL ADC_CR2_RSTCAL
// set by software to start the calibration.
#define CAL ADC_CR2_CAL
// Continuous conversion
#define CONT ADC_CR2_CONT // 0=single conv mode 1==continous conv
// A/D converter ON / OFF
// bit holds a value of zero and a 1 is written to it then it wakes up the ADC
#define ADON ADC_CR2_ADON
//
//      ADC_SMPR1   ADC sample time register
// Channel x Sample time selection
#define SMP1(REG, BIT, CHANNEL) (SET_BIT(REG, BIT) << CHANNEL)
//
//     ADC_SMPR2   ADC sample time register 2
// Channel x Sample time selection
#define SMP2(REG, BIT, CHANNEL) (SET_BIT(REG, BIT) << CHANNEL)
//
//      ADC_SQR1  regular sequence register 1
#define SEQ_LEN(REG, VAL) WRITE_REG(REG, VAL) // 0000 define the total num of conv.
#define SQ16(REG, VAL) WRITE_REG(REG, VAL)    // 16th conv. in reg. seq.
#define SQ15(REG, VAL) WRITE_REG(REG, VAL)    // 15th conv. in reg. seq.
#define SQ14(REG, VAL) WRITE_REG(REG, VAL)    // 14th conv. in reg. seq.
#define SQ13(REG, VAL) WRITE_REG(REG, VAL)    // 13th conv. in reg. seq.
//
//      ADC_SQR2  regular sequence register 2
#define SQ12(REG, VAL) WRITE_REG(REG, VAL) // 12th conv. in reg. seq.
#define SQ11(REG, VAL) WRITE_REG(REG, VAL) // 11th conv. in reg. seq.
#define SQ10(REG, VAL) WRITE_REG(REG, VAL) // 10th conv. in reg. seq.
#define SQ9(REG, VAL) WRITE_REG(REG, VAL)  // 9th conv. in reg. seq.
#define SQ8(REG, VAL) WRITE_REG(REG, VAL)  // 8th conv. in reg. seq.
#define SQ7(REG, VAL) WRITE_REG(REG, VAL)  // 7th conv. in reg. seq.
//
//      ADC_SQR3  regular sequence register 3
#define SQ6(REG, VAL) WRITE_REG(REG, VAL) // 6th conv. in reg. seq.
#define SQ5(REG, VAL) WRITE_REG(REG, VAL) // 5th conv. in reg. seq.
#define SQ4(REG, VAL) WRITE_REG(REG, VAL) // 4th conv. in reg. seq.
#define SQ3(REG, VAL) WRITE_REG(REG, VAL) // 3th conv. in reg. seq.
#define SQ2(REG, VAL) WRITE_REG(REG, VAL) // 2th conv. in reg. seq.
#define SQ1(REG, VAL) WRITE_REG(REG, VAL) // 1th conv. in reg. seq.
//
//      ADC_DR  ADC regular data register
#define DUAL_DR(REG) READ_REG(REG)
#define DATA_DR(REG) READ_REG(REG)
//
uint32_t adcData;
uint32_t adcDummy;
void adc_test() // PC13
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
//
void setADCpins() // FLOAT INPUTS
{                 // PA0....PA7   PB0,PB1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    GPIOA->CRL = (GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR = RCC_CFGR_ADCPRE_DIV8;
    GPIOB->CRL = (GPIO_CRL_CNF0 | GPIO_CRL_CNF1);
}

////////////////
/// SINGLE /////
void adc1_single_init()
{
    setADCpins();
    ADC1->CR2 |= ADON; //
    _delay_ms(20000);
    // ADC1->CR2 |= ADC_CR2_RSTCAL;
    ADC1->CR2 |= ADC_CR2_CAL;
    while (!(ADC1->CR2 & ADC_CR2_CAL))
        ;
    ADC1->CR2 &= ~ALIGN; // 1-left 0-right
    ADC1->CR2 &= ~CONT;

    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2; // 239.5 cyc
    ADC1->SQR3 |= 0;                                                       // adc-pin 1
    ADC1->SQR1 = 0;                                                        // adc-pin 1
}
uint16_t read_Single_Adc()
{
    ADC1->CR2 |= ADON;             // Start Conversion of regular' channels
    while (!(ADC1->SR & EOC_FLAG)) // 1-conv stops
        ;
    // eusart_send((ADC1->DR & 0x0F00) >> 8);
    eusart_send(ADC1->DR);
    return ADC1->DR;
}
//
volatile uint16_t adc_result;
volatile uint8_t conversion_complete = 0;

void ADC_Interrupt_Init(void)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

    // Configure PA0 analog
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // ADC configuration
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    ADC1->CR2 |= ADC_CR2_ADON;
    _delay_ms(10000);

    // Calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;

    // Configure
    ADC1->SMPR2 |= ADC_SMPR2_SMP0; // 239.5 cycles
    ADC1->SQR3 = 0;                // Channel 0

    // Enable interrupt
    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
    NVIC_SetPriority(ADC1_2_IRQn, 1);
}

void ADC_Start_Conversion(void)
{
    conversion_complete = 0;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC1_2_IRQHandler(void)
{
    if (ADC1->SR & ADC_SR_EOC)
    {
        adc_result = ADC1->DR;
        conversion_complete = 1;
    }
    eusart_send(adc_result);
}
///
///
#define ADC_BUFFER_SIZE 100
uint16_t adc_buffer[ADC_BUFFER_SIZE];

void ADC_DMA_Init(void)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Configure PA0 analog
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // ADC configuration
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    // DMA configuration
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)adc_buffer;
    DMA1_Channel1->CNDTR = ADC_BUFFER_SIZE;
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0;

    // ADC1 configuration
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_ADON;

    _delay_ms(10000);

    // Calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;

    // Configure sequence (single channel)
    ADC1->SQR1 = 0;
    ADC1->SQR3 = 0;

    // Enable DMA channel
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    // Enable ADC and start conversion
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}
////
////
void ADC_MultiChannel_Init(void)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    // Configure PA0, PA1, PC0 as analog inputs
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOC->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // ADC configuration
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    // ADC1 configuration
    ADC1->CR1 |= ADC_CR1_SCAN; // Enable scan mode
    ADC1->CR2 |= ADC_CR2_CONT; // Enable continuous conversion
    ADC1->CR2 |= ADC_CR2_ADON; // Enable ADC

    _delay_ms(10000);

    // Calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;

    // Configure sequence
    ADC1->SQR1 = (3 - 1) << 20; // 3 conversions

    // Channel sequence (Channel 0, 1, 10)
    ADC1->SQR3 = (0 << 0) | (1 << 5) | (10 << 10);

    // Sampling times
    ADC1->SMPR2 |= ADC_SMPR2_SMP0 | ADC_SMPR2_SMP1; // 239.5 cycles for channels 0,1
    ADC1->SMPR1 |= ADC_SMPR1_SMP10;                 // 239.5 cycles for channel 10

    // Enable EOC interrupt
    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t adc_values[3];

// void ADC1_2_IRQHandler(void)
// {
//     if (ADC1->SR & ADC_SR_EOC)
//     {
//         static uint8_t channel = 0;
//         adc_values[channel++] = ADC1->DR;
//         if (channel >= 3)
//             channel = 0;
//     }
// }
#endif // __ADC
