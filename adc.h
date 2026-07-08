#if !defined(__ADC)
#define __ADC
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include "eusart.h"
#include "tim1.h"
#include "gpio.h"
#include "spi_Lcd.h"

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
// dual-modes  0110: Regular simultaneous mode only
#define DUALMOD(REG, VAL) WRITE_REG(REG, VAL) // xxx
/*DISCNUM define the num of regular channels to be converted
 in discontinuous mode, after receiving an external trigger*/
#define DISCNUM(REG, BIT) WRITE_REG(REG, BIT) // DISCNUM[2:0] 0...0x07
#define DISCEN ADC_CR1_DISCEN                 // enable/disable Discontinuous mode
#define SCAN_MODE ADC_CR1_SCAN                // enable/disable Scan mode
#define EOCIE ADC_CR1_EOCIE                   // enable/disable the End of Conversion interrupt
//
//         ADC->CR2
// temp sensor vref channel Enable
#define TEMP_SEN_VREF_EN ADC_CR2_TSVREFE
/* start conversion and cleared by hardware as soon as conversion starts.
if SWSTART is selected as trigger event by the EXTSEL[2:0] bits*/
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
// SQRx [4:0] Channel/channels selects 0....17
#define ADCPA0 0
#define ADCPA1 1
#define ADCPA2 2
#define ADCPA3 3
#define ADCPA4 4
#define ADCPA5 5
#define ADCPA6 6
#define ADCPA7 7
//
#define ADCPB0 8
#define ADCPB1 9
//
#define ADCPC0 10
#define ADCPC1 11
#define ADCPC2 12
#define ADCPC3 13
#define ADCPC4 14
#define ADCPC5 15
//
#define ADCTEMP 16
//
#define SEQ_LEN(REG, VAL) WRITE_REG(REG, VAL) // ADC_SQR1_Lxx
//
//// SMPx 1.5 cyc [2:0]
#define SMPR(REG, VAL) WRITE_REG(REG, VAL) // ADC_SMPRx_SMPx

//
#define CYC1_5 0
#define CYC7_5 1
#define CYC13_5 2
#define CYC28_5 3
#define CYC41_5 4
#define CYC55_5 5
#define CYC71_5 6
#define CYC239_5 7
/*
SQRn GPIO MAPING

SQR3[4:0] ch0....ch5
SQR2[4:0] ch6....ch11
SQR1[4:0] ch12....ch15
sequence length SQR1[23:20]

PINA analog pin (PA0......PA10) CH0.......CH7
PINA 0.........PINA 7
SMPR2[2:0].....SMPR2[23:21]
//
PINB analog pin (PB0 & PB1) [CH8 & CH9]
PINB0 & PINB1
SMPR2[26:24] & SMPR2[29:27]

TEMP SENSOR
SQR1 ch16

VREF Internal
SQR1 ch17
*/
//      ADC_DR  ADC regular data register
#define DUAL_DR(REG) READ_REG(REG)
#define DATA_DR(REG) READ_REG(REG)
//
uint32_t adcData;
uint32_t adcDummy;
volatile uint16_t adc_result;
volatile uint8_t conversion_complete = 0;
#define ADC_BUFFER_SIZE 64
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint16_t adc_values[3];
uint8_t conv_cnt = 0;
//
void ADC1_2_IRQHandler(void)
{
    if ((ADC1->SR & (ADC_SR_EOC)))
    {
        adc_result = ADC1->DR;
    }
    if ((ADC1->SR & (ADC_SR_AWD)))
    {
        ADC1->SR &= ~ADC_SR_AWD;
    }
    if ((ADC1->SR & (ADC_SR_STRT)))
    {
        ADC1->SR &= ~ADC_SR_STRT;
    }
    if ((ADC1->SR & (ADC_SR_EOS)))
    {
        ADC1->SR &= ~ADC_SR_EOS;
    }
    if ((ADC1->SR & (ADC_SR_JEOC)))
    {
        ADC1->SR &= ~ADC_SR_JEOC;
    }
    if ((ADC1->SR & (ADC_SR_JSTRT)))
    {
        ADC1->SR &= ~ADC_SR_JSTRT;
    }
}

void adc_pin_config()
{
    GPIOA->CRL = (ANALOG << GPIO_CRL_CNF0_Pos);
    GPIOA->CRL |= (ANALOG << GPIO_CRL_CNF1_Pos);
    GPIOA->CRL |= (ANALOG << GPIO_CRL_CNF2_Pos);
    //
    ADC1->SQR3 = (0 << ADC_SQR3_SQ1_Pos);
    ADC1->SQR3 |= (1 << ADC_SQR3_SQ2_Pos);
    ADC1->SQR3 |= (2 << ADC_SQR3_SQ3_Pos);
    // sample time for each channel
    ADC1->SMPR2 = (1 << ADC_SMPR2_SMP0_Pos);
    ADC1->SMPR2 |= (1 << ADC_SMPR2_SMP1_Pos);
    ADC1->SMPR2 |= (1 << ADC_SMPR2_SMP2_Pos);
    // total conv SEQ_LEN
    ADC1->SQR1 = (2 << ADC_SQR1_L_Pos); //
}
void adcInit()
{

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

    adc_pin_config(); // REG B0

    // RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
    ADC1->CR2 |= ADC_CR2_CONT;
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_EXTTRIG;    // ext trg enable
    ADC1->CR2 |= ADC_CR2_EXTSEL_Msk; // swstart
    ADC1->SR &= ~ADC_SR_STRT;

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL)
        ;

    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_CAL))
        ;

    while ((ADC1->SR & ADC_SR_STRT))
    {
        ADC1->SR &= ~ADC_SR_STRT;
    }
    ADC1->CR1 |= ADC_CR1_EOCIE;
    // NVIC_EnableIRQ(ADC1_2_IRQn);
    // NVIC_SetPriority(ADC1_2_IRQn, 2);
    ADC1->CR2 |= ADC_CR2_ADON;
    // eusart_send((uint8_t)(ADC1->SR & 0X0FF));
}
uint16_t getAdc()
{
    // ADC1->CR2 |= ADC_CR2_ADON;
    // ADC1->SR &= ~ADC_SR_STRT;
    while (!(ADC1->SR & ADC_SR_STRT))
    {
    }
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_CR2_SWSTART))
    {
    }
    while (!(ADC1->SR & ADC_SR_EOC))
        ;

    uint16_t val = (ADC1->DR);
    // eusart_send((uint8_t)(val & 0XF00) >> 8);
    // eusart_send((uint8_t)(val & 0X0FF));
    return val;
}

void ADC_Init(uint32_t regv, uint32_t conf_mode, uint8_t pos)
{
    // Enable clocks
    // RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // Configure PA0 analog
    // GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    gpioConfig(regv, conf_mode, pos);
    // ADC configuration
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
    ADC1->CR2 &= ~ADC_CR2_CONT;
    ADC1->CR2 |= ADC_CR2_ADON;
    timer4_delay(50);
    ADC1->CR2 &= ~(ADC_CR2_ALIGN);
    // ADC1->CR2 |= (ADC_CR2_ALIGN);
    // Calibration
    ADC1->CR2 &= ~ADC_CR2_EXTSEL_Msk;
    ADC1->CR2 |= ADC_CR2_CAL;
    // ADC1->CR1 |= ADC_CR1_SCAN;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;
    // Configure
    ADC1->SQR3 |= ADCPB0; // channel sel
    // ADC1->SMPR2 |=(7 << (3 * 8)); // cyc
    SMPR(ADC1->SMPR2, (CYC239_5 << 24)); //
    // Enable interrupt
    ADC1->CR1 |= ADC_CR1_EOCIE;
    // NVIC_EnableIRQ(ADC1_2_IRQn);
    // NVIC_SetPriority(ADC1_2_IRQn, 2);
    ADC1->CR2 |= ADC_CR2_ADON;
}
uint16_t get_ADC(void) // 12 bit
{
    ADC1->CR2 |= ADC_CR2_ADON;
    // ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_STRT))
    {
    }
    while (!(ADC1->SR & ADC_SR_EOC))
    {
    }
    uint16_t val = (ADC1->DR);
    lcd_command(CLEAR_DISP);
    // write4Char((uint8_t)((val & 0xF000) >> 12) | 0x30);
    write4Char((uint8_t)((val & 0xF00) >> 8) | 0x30);
    write4Char((uint8_t)((val & 0x0F0) >> 4) | 0x30);
    write4Char((uint8_t)(val & 0x00F) | 0x30);
    write4Char(' ');
    timer4_delay(500);
    ADC1->CR2 &= ~ADC_CR2_CAL;
    return ADC1->DR;
}
///
// dma channel 1 == ADC1
///
void DMA1_Channel1_IRQHandler() // tx
{
    if ((DMA1->ISR & DMA_ISR_TCIF5))
    {
        DMA1->IFCR &= ~(DMA_IFCR_CTCIF1);
    }
    // eusart_send((uint8_t)(adc_buffer&0XFF00) >> 8);
    // eusart_send(0X22);
}
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
    DMA1_Channel1->CCR = DMA_CCR_TCIE;

    // ADC1 configuration
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_ADON;

    timer4_delay(50);

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

    timer4_delay(50);

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
//
//////// ADC/LCD ///////////
//
void adc_lcd_init(uint32_t reg, uint32_t conf_mode, uint8_t pos)
{
    adcInit(reg, conf_mode, pos);
    // lcd4_init(BAUD_FCLK_64);
    // lcd_4_init();
}
// void get_adc_lcd()
// {
//     getAdc();
// }

#endif // __ADC
