#if !defined(__ADVTIM1)
#define __ADVTIM1
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include "eusart.h"
/*
The Time Base Unit includes:
●   Counter Register (TIM1_CNT)
●   Prescaler Register (TIM1_PSC):
●   Auto-Reload Register (TIM1_ARR)
●   Repetition Counter Register (TIM1_RCR)
*/
//     TIMx->CR1  control register 1
// div ratio OF  timer clock (CK_INT) frequency, dead time and sampling clock
#define CKDIV1(REG,VAL) WRITE_REG(REG,VAL)
// Auto-reload preload enable. 1=Buffered
#define ARPE1 TIM_CR1_ARPE;
// Center-aligned mode selection.
#define CMS1(REG, VAL) WRITE_REG(REG, VAL)
#define EDGE_ALIGNED ~(TIM_CR1_CMS_1 | TIM_CR1_CMS_0)
#define CENTRE_ALIGNED  TIM_CR1_CMS_0
// Direction. 1-upcnt 0-downCnt
#define DIR1 TIM_CR1_DIR;
// 0: Counter is not stopped @ update event
#define OPM1 TIM_CR1_OPM;
// Update request source.select the UEV event sources 1-OVF or UNDF 0-OVF/UNDF or UG bit or from slave
#define URS1 TIM_CR1_URS
//update disable
#define UDIS1 TIM_CR1_UDIS
//0-disable 1-enable
#define CEN1 TIM_CR1_CEN
//
//      TIM1_dier  Control register 2
// Trigger DMA request enable.
#define TDE1 TIM_DIER_TDE
// COM DMA request enable.
#define COMDE1 TIM_DIER_COMDE
// Capture/Compare 4 DMA request enable
#define CC4DE1 TIM_DIER_CC4DE
// Capture/Compare 3 DMA request enable
#define CC3DE1 TIM_DIER_CC3DE
// Capture/Compare 2 DMA request enable
#define CC2DE1 TIM_DIER_CC2DE
// Capture/Compare 1 DMA request enable
#define CC1DE1 TIM_DIER_CC1DE
// Update DMA request enable.
#define UDE1 TIM_DIER_UDE
// Break interrupt enable.
#define BIE1 TIM_DIER_BIE
// Trigger interrupt enable.
#define TIE1 TIM_DIER_TIE
// Comm interrupt enable.
#define COMIE1 TIM_DIER_COMIE
// Capture/Compare 4 interrupt enable.
#define CC4IE1 TIM_DIER_CC4IE
// Capture/Compare 3 interrupt enable.
#define CC3IE1 TIM_DIER_CC3IE
// Capture/Compare 2 interrupt enable.
#define CC2IE1 TIM_DIER_CC2IE
// Capture/Compare 1 interrupt enable.
#define CC1IE1 TIM_DIER_CC1IE
// Update interrupt enable.
#define UIE1 TIM_DIER_UIE
//
// TIM1_SR  Status register
// Capture/Compare 4 Overcapture Flag.
#define CC4OF TIM_SR_CC4IF
// Capture/Compare 3 Overcapture Flag.
#define CC3OF TIM_SR_CC3IF
// Capture/Compare 2 Overcapture Flag.
#define CC2OF TIM_SR_CC2IF
// Capture/Compare 1 Overcapture Flag.
#define CC1OF TIM_SR_CC1IF
// Break interrupt Flag.
#define BIF TIM_SR_BIF
//
#endif // __ADVTIM1
