#if !defined(__ADC)
#define __ADC
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
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
volatile uint16_t adc_result;
volatile uint8_t conversion_complete = 0;
#define ADC_BUFFER_SIZE 64
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint16_t adc_values[3];
//
void lcd_char2hex(uint16_t val)
{
    if (val > 0x00 && val <= 0x09) // 0--9
    {
        lcd_command(CLEAR_DISP);
        write4Char(0x30 + val);
    }
    if (val >= 10 && val <= 19) // 10--19
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char(0x30 + (val - 10));
    }
    if (val >= 20 && val <= 29)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char(0x30 + (val - 20));
    }
    if (val >= 30 && val <= 39)
    {
        lcd_command(CLEAR_DISP);
        write4Char('3');
        write4Char(0x30 + (val - 30));
    }
    if (val >= 40 && val <= 49)
    {
        lcd_command(CLEAR_DISP);
        write4Char('4');
        write4Char(0x30 + (val - 40));
    }
    if (val >= 50 && val <= 59)
    {
        lcd_command(CLEAR_DISP);
        write4Char('5');
        write4Char(0x30 + (val - 50));
    }
    if (val >= 60 && val <= 69)
    {
        lcd_command(CLEAR_DISP);
        write4Char('6');
        write4Char(0x30 + (val - 60));
    }
    if (val >= 70 && val <= 79)
    {
        lcd_command(CLEAR_DISP);
        write4Char('7');
        write4Char(0x30 + (val - 70));
    }
    if (val >= 80 && val <= 89)
    {
        lcd_command(CLEAR_DISP);
        write4Char('8');
        write4Char(0x30 + (val - 80));
    }
    if (val >= 90 && val <= 99)
    {
        lcd_command(CLEAR_DISP);
        write4Char('9');
        write4Char(0x30 + (val - 90));
    }
    if (val >= 100 && val <= 109)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('0');
        write4Char(0x30 + (val - 100));
    }
    if (val >= 110 && val <= 119)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('1');
        write4Char(0x30 + (val - 110));
    }
    if (val >= 120 && val <= 129)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('2');
        write4Char(0x30 + (val - 120));
    }
    if (val >= 130 && val <= 139)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('3');
        write4Char(0x30 + (val - 130));
    }
    if (val >= 140 && val <= 149)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('4');
        write4Char(0x30 + (val - 140));
    }
    if (val >= 150 && val <= 159)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('5');
        write4Char(0x30 + (val - 150));
    }
    if (val >= 160 && val <= 169)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('6');
        write4Char(0x30 + (val - 160));
    }
    if (val >= 170 && val <= 179)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('7');
        write4Char(0x30 + (val - 170));
    }
    if (val >= 180 && val <= 189)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('8');
        write4Char(0x30 + (val - 180));
    }
    if (val >= 190 && val <= 199)
    {
        lcd_command(CLEAR_DISP);
        write4Char('1');
        write4Char('9');
        write4Char(0x30 + (val - 190));
    }
    if (val >= 200 && val <= 209)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('0');
        write4Char(0x30 + (val - 200));
    }
    if (val >= 210 && val <= 219)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('1');
        write4Char(0x30 + (val - 210));
    }
    if (val >= 220 && val <= 229)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('2');
        write4Char(0x30 + (val - 220));
    }
    if (val >= 230 && val <= 239)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('3');
        write4Char(0x30 + (val - 230));
    }
    if (val >= 240 && val <= 249)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('4');
        write4Char(0x30 + (val - 240));
    }
    if (val >= 250 && val <= 255)
    {
        lcd_command(CLEAR_DISP);
        write4Char('2');
        write4Char('5');
        write4Char(0x30 + (val - 250));
    }
}
//
void ADC1_2_IRQHandler(void)
{
    if (ADC1->SR & ADC_SR_EOC)
    {
        adc_result = ADC1->DR;
        conversion_complete = 1;
    }
    // lcd_command(CLEAR_DISP);
    // write4Char((char) adc_result);
}

void ADC_Init(uint32_t regv, uint32_t conf_mode)
{
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

    // Configure PA0 analog
    // GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    regv |= conf_mode;

    // ADC configuration
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
    // ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 |= ADC_CR2_ADON;
    timer1_del(_50ms);
    ADC1->CR2 &= ~(ADC_CR2_ALIGN);

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

uint16_t get_ADC(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    conversion_complete = 0;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    return adc_result;
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

    timer1_del(_50ms);

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

    timer1_del(_50ms);

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
void adc_lcd_init(uint32_t reg, uint32_t conf_mode, uint8_t baud)
{
    ADC_Init(reg, conf_mode);
    lcd4_init(baud);
    lcd_4_init();
}
void get_adc_lcd()
{
    get_ADC();
    _delay_ms(50000);
}

#endif // __ADC
