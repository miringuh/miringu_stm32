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
#define CKDIV1(REG, VAL) WRITE_REG(REG, VAL)
// Auto-reload preload enable. 1=Buffered
#define ARPE1 TIM_CR1_ARPE;
// Center-aligned mode selection.
#define CMS1(REG, VAL) WRITE_REG(REG, VAL)
#define EDGE_ALIGNED ~(TIM_CR1_CMS_1 | TIM_CR1_CMS_0)
#define CENTRE_ALIGNED TIM_CR1_CMS_0
// Direction. 1-upcnt 0-downCnt
#define CNT_DIR TIM_CR1_DIR;
// 0: Counter is not stopped @ update event
#define PULSE_MODE TIM_CR1_OPM;
// Update request source.select the UEV event sources 1-OVF/UNDF 0-OVF/UNDF or UG bit or from slave
#define UPDATE_REQ_SRC TIM_CR1_URS
// update disable
#define UPDATE_DISABLE TIM_CR1_UDIS
// 0-disable 1-enable
#define COUNT_EN TIM_CR1_CEN
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
#define CC4OF_FLAG TIM_SR_CC4IF
// Capture/Compare 3 Overcapture Flag.
#define CC3OF_FLAG TIM_SR_CC3IF
// Capture/Compare 2 Overcapture Flag.
#define CC2OF_FLAG TIM_SR_CC2IF
// Capture/Compare 1 Overcapture Flag.
#define CC1OF_FLAG TIM_SR_CC1IF
// Break interrupt Flag.
#define BIF_FLAG TIM_SR_BIF
// Trigger interrupt Flag.
#define TIF_FLAG TIM_SR_TIF
// COM interrupt Flag.
#define COMIF_FLAG TIM_SR_COMIF
// Capture/Compare 4 interrupt Flag
#define CC41F_FLAG TIM_SR_CC41F
// Capture/Compare 3 interrupt Flag
#define CC31F_FLAG TIM_SR_CC31F
// Capture/Compare 2 interrupt Flag
#define CC42F_FLAG TIM_SR_CC21F
// Capture/Compare 1 interrupt Flag
#define CC11F_FLAG TIM_SR_CC11F
// Capture/Compare 4 interrupt Flag
#define UIF_FLAG TIM_SR_UIF

//       TIM1_EGR  Event generation register
// Break Generation.
#define BG TIM_EGR_BG
// Trigger Generation.
#define TG TIM_EGR_TG
// Update Generation.
#define UG TIM_EGR_TG
//
//     TIM1_CNT [15:0]
#define TIM1_CNT_REG(REG,VAL) WRITE_REG(REG,VAL)
//     TIM1_PSC [15:0]  The counter clock frequency (CK_CNT) is equal to fCK_PSC / (PSC[15:0] + 1).
#define TIM1_PSC_REG(REG, VAL) WRITE_REG(REG, VAL)
// ARR is the value to be loaded in the actual auto-reload register.
#define TIM1_ARR_REG(REG, VAL) WRITE_REG(REG, VAL)
// Repetition Counter Value.[8:0]
#define TIM1_RCR_REG(REG, VAL) WRITE_REG(REG, VAL)
//

/*
In up-counting mode, the counter counts from 0 to the auto-reload value (content of the TIM1_ARR register) then restarts from 0 and generates a counter overflow event.

If the repetition counter is used, the update event (UEV) is generated after up-counting is repeated for the number of times programmed in the repetition counter register (TIM1_RCR).

Setting the UG bit in the TIM1_EGR register (by software or by using the slave mode controller) also generates an update event.

The UEV event can be disabled by software by setting the UDIS bit in the TIM1_CR1 register

When an update event occurs, all the registers are updated and the update flag (UIF bit in TIM1_SR register) is set (depending on the URS bit):

The counter clock can be provided by the following clock sources:
       ●Internal clock (CK_INT)
       ●External clock mode1: external input pin
       ●External clock mode2: external trigger input ETR
       ●Internal trigger inputs (ITRx): using one timer as prescaler for another timer, for
example, you can configure Timer 1 to act as a prescaler for Timer 2. Refer to

*/

#endif // __ADVTIM1
