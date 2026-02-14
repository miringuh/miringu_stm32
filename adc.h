#if !defined(__ADC)
#define __ADC
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
// #include <stdlib.h>
// #include <stdlib.h>
// #include <stdint.h>

// ADC->SR
// regular ch clrd by softw 1==started
#define CONV_STRT_FLAG ADC_SR_STRT
// injected ch clrd by softw 1==started
#define CONV_STRTJ_FLAG ADC_SR_JSTRT
// reg ch end conv 1==comp
#define CONV_END_FLAG ADC_SR_EOC
// injected ch conv end 1==comp
#define CONV_ENDJ_FLAG ADC_SR_JEOC
// Watchdog flag 1==voltage range crossed set by ADC_LTR & ADC_HTR
#define CONV_AWD_FLAG ADC_SR_AWD

// ADC->CR1
#define WDT_EN ADC_CR1_AWDEN   // 1==WDT en
#define JWDT_EN ADC_CR1_JAWDEN // 1==JWDT en
//      MODES
#define IND_MODE 0
#define COMB_REG_SIMULT_INJ_SIMULT_MODE (0X01 << 16)
#define COMB_REG_SIMULT_ALT_TRIGGER_MODE (0X02 << 16)
#define COMB_INJ_SIMULT_FAST_INTERLEAVED_MODE (0X03 << 16)
#define COMB_INJ_SIMULT_SLOW_INTERLEAVED_MODE (0X04 << 16)
#define COMB_INJ_SIMULT_MODE (0X05 << 16)
#define COMB_REG_SIMULT_MODE (0X06 << 16)
#define FAST_INTERLEAVED_MODE (0X07 << 16)
#define SLOW_INTERLEAVED_MODE (0X08 << 16)
#define ALT_TRIG_MODE (0X09 << 16)
/*DISCNUM define the num of regular channels to be converted
 in discontinuous mode, after receiving an external trigger*/
#define DISC_CH_NUM(REG, BIT) SET_BIT(REG, BIT) // DISCNUM[2:0] 0...0x07
#define DISC_INJ_CH_EN ADC_CR1_JDISCEN          // discontinue inj.ch enable
#define DISC_REG_CH_EN ADC_CR1_DISCEN           // discontinue reg.ch enable
#define AUTO_INJ_EN ADC_CR1_JAUTO
#define WDT_EN_ONE_CH ADC_CR1_AWDSGL    // identified by AWDCH[4:0]
#define WDT_EN_ALL_CH ~(ADC_CR1_AWDSGL) // identified by AWDCH[4:0]
// enable/disable  from inputs set by ADC_SQRRx/ADC_JSQRx registers
#define SCAN_MODE ADC_CR1_SCAN
// INTR ENABLE [JEOCIE AWDIE EOCIE]
#define INTREN(REG, BIT) SET_BIT(REG, BIT)
#define WDT_CH(REG, BIT) SET_BIT(REG, BIT) // AWDCH[4:0]: 0......17
//
//      ADC->CR2
// temp sensor vref channel Enable
#define TEMP_SEN_VREF_EN ADC_CR2_TSVREFE
/* start conversion and cleared by hardware as soon as conversion starts.
if SWSTART is selected as trigger event by the EXTSEL[2:0] bits
*/
#define START_REG_CONV ADC_CR2_SWSTART
/*
if JSWSTART is selected as
trigger event by the JEXTSEL[2:0] bits
*/
#define JSWSTART ADC_CR2_JSWSTART
// enable/disable the external trigger used to start conversion
#define EXTTRIG ADC_CR2_EXTTRIG
// External event select for regular group
#define EXT_REG_EVENT_SEL(REG, BIT) SET_BIT(REG, BIT)
// External trigger conversion mode for injected channels
#define JEXTTRIG ADC_CR2_JEXTTRIG
// External event select for injected group
#define EXT_REG_EVENT_SEL(REG, BIT) SET_BIT(REG, BIT)
// Data alignment
#define ALIGN ADC_CR2_ALIGN
// Direct memory access mode
#define DMA ADC_CR2_DMA
// Reset calibration
#define RSTCAL ADC_CR2_RSTCAL
// set by software to start the calibration.
#define CAL ADC_CR2_CAL
// Continuous conversion
#define CONT ADC_CR2_CONT // 0=single conv mode 1==continous conv
// A/D converter ON / OFF
#define ADON ADC_CR2_ADON // 1==ON 0==OFF
//
//   ADC_SMPR1   ADC sample time register
// Channel x Sample time selection
#define SMPX1(REG, BIT, CHANNEL) (SET_BIT(REG, BIT) << CHANNEL) // 000 ADC->SMPR1
//
//   ADC_SMPR2   ADC sample time register 2
// Channel x Sample time selection
#define SMPX2(REG, BIT, CHANNEL) (SET_BIT(REG, BIT) << CHANNEL) // 000 ADC->SMPR2
//

//
void testADC()
{
    // SET_BIT(ADC1->CR1,ADC_CR1_AWDCH);
    DISC_CH_NUM(ADC1->CR1, ADC_CR1_AWDCH);
    INTREN(ADC1->CR1, ADC_CR1_EOCIE);
    
}
#endif // __ADC
