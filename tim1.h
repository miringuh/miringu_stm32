#if !defined(__TIMER)
#define __TIMER
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include "gpio.h" //
//     TIMx->CR1  control register 1
#define CLK_DIV(REG, VAL) WRITE_REG(REG, VAL) // 00 Clock Division.
// Auto-reload preload enable 1: TIMx_ARR register is buffered.
#define ARPE TIM_CR1_ARPE
// Center-aligned Mode Selection
#define CMS(REG, VAL) WRITE_REG(REG, VAL | READ_REG(REG)) // 00
// Direction
#define TIM_DIR TIM_CR1_DIR // 1-downcnt 0-upcnt
// Counter stops counting 0-no stop  1-the next update event & CEN is hware sets to 0
#define OPM TIM_CR1_OPM
// This bit is set and cleared by software to select the UEV event sources.
#define URS TIM_CR1_URS   // Only counter ovf/undf generates an update interrupt
#define UDIS TIM_CR1_UDIS // Update disable UEV events
#define CEN TIM_CR1_CEN   // Counter enable
//
//     TIMx->CR2  control register 2
// Reset=0 Enable=001 Update=010 compare pulse=010
#define MMS(REG, VAL) WRITE_REG(REG, VAL | READ_REG(REG)) // Master mode selection
//
//  [TIMx->DIER] DMA/Interrupt enable register
#define UDE TIM_DIER_UDE // Update DMA request enable
#define UIE TIM_DIER_UIE // Update interrupt enable
//
//  [TIMx->SR] STATUS register
/*
** 1==overflow or underflow and if UDIS = 0 in the TIMx_CR1 register.
** When CNT is reinitialized by software using the UG bit in the TIMx_EGR register,
** if URS = 0 and UDIS = 0 in the TIMx_CR1 register.
*/
#define UIF_FLAG TIM_SR_UIF // Update interrupt flag
//
//  [TIMx->EGR] event generation register
// Re-initializes the timer counter and generates an update of the registers.
#define UG TIM_EGR_UG // Update generation
//
//    (TIMx_CNT) counter
#define COUNTER(REG, VAL) WRITE_REG(REG, VAL) // CNT[15:0]
//
//  TIMx_PSC prescaler
// timer_freq=fCK_PSC / (PSC[15:0] + 1).
#define TIM_PRESC(REG, VAL) WRITE_REG(REG, VAL) // PRESC[15:0]
//
// TIMx_ARR auto-reload register
#define AUTO_RELOAD(REG, VAL) WRITE_REG(REG, VAL) // ARR[15:0]
//
/*
20  25hz   40ms
10  50hz   20ms
5   100hz  10ms
2.5 200hz   5ms

*/
/*
freq(hz)=Tclk/((PSC+1)(ARR+1))
time(ms)=(1/hz)*1000
*/
volatile uint16_t timer4_freq;
volatile uint16_t timer3_freq;
volatile uint16_t timer1_freq;
volatile uint16_t ptimer1_freq;
// TIMER 3
void TIM3_IRQHandler(void)
{
    if ((TIM3->SR & TIM_SR_UIF))
    {
        TIM3->SR &= ~TIM_SR_UIF;
        timer3_freq++;
    }
};

void timer3()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CR1 &= ~CEN;
    TIM3->PSC = 71; // 1us (1ms=1000us)
    TIM3->ARR = 999;
    TIM3->CNT = 0;
    TIM3->DIER = TIM_DIER_UIE | TIM_DIER_TIE;
    TIM3->CR1 = TIM_CR1_ARPE;
    TIM3->CR1 &= !(TIM_CR1_OPM | TIM_CR1_UDIS); // cnt stops
    TIM3->EGR |= TIM_EGR_UG;

    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= CEN;
}
void timer3_delay(uint16_t cyc)
{
    // TIM1->CR1 |= CEN;
    while (timer3_freq != cyc)
        ;
    timer3_freq = 0;

    // TIM1->CR1 &= ~CEN;
}
// TIMER 4
void TIM4_IRQHandler(void)
{
    if ((TIM4->SR & TIM_SR_UIF))
    {
        TIM4->SR &= ~TIM_SR_UIF;
        timer4_freq++;
    }
};
void timer4()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->CR1 &= ~CEN;
    TIM4->PSC = 71;  // 1us 1ms==1000us
    TIM4->ARR = 999; // 1ms
    TIM4->CNT = 0;
    TIM4->DIER = TIM_DIER_UIE; //| TIM_DIER_TIE;
    TIM4->CR1 = TIM_CR1_ARPE;
    TIM4->CR1 &= !(TIM_CR1_OPM | TIM_CR1_UDIS); // cnt stops
    TIM4->EGR |= TIM_EGR_UG;

    NVIC_SetPriority(TIM4_IRQn, 2);
    NVIC_EnableIRQ(TIM4_IRQn);
    TIM4->CR1 |= CEN;
}
void timer4_delay(uint16_t cyc)
{

    // TIM1->CR1 |= CEN;
    while (timer4_freq != cyc)
        ;
    timer4_freq = 0;

    // TIM1->CR1 &= ~CEN;
}
//////////////////////////////////
// TIMER 1
void TIM1_UP_IRQHandler(void)
{
    if ((TIM1->SR & TIM_SR_UIF))
    {
        TIM1->SR &= ~TIM_SR_UIF;
        timer1_freq++;
    }
};
void TIM1_DOWN_IRQHandler(void)
{

    if ((TIM1->SR & TIM_SR_UIF))
    {
        TIM1->SR &= ~TIM_SR_UIF;
        ptimer1_freq++;
    }
}
/*
The UEV event can be disabled by software by setting the UDIS bit in the TIM1_CR1
Then no update event occurs until the UDIS bit has been written to 0.
*/
void timer1_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    timer1_freq = 0;
    TIM1->CR1 &= ~CEN;
    TIM1->PSC = 19;  // 1us 1ms=1000us
    TIM1->ARR = 999; //
    TIM1->CNT = 0;
    TIM1->RCR = 0; //*****
    TIM1->DIER = TIM_DIER_UIE | TIM_DIER_TIE;
    TIM1->CR1 = TIM_CR1_ARPE;
    TIM1->CR1 &= ~(TIM_CR1_OPM | TIM_CR1_UDIS | TIM_DIR | TIM_CR1_URS); // cnt stops
    // TIM1->EGR |= TIM_EGR_UG;
    NVIC_SetPriority(TIM1_UP_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    TIM1->CR1 |= CEN;
}
void timer1_delay(uint16_t cyc)
{
    while (timer1_freq != cyc)
        ;
    timer1_freq = 0;
}
////////////////////

void timer1_Pwm()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    timer1_freq = 0;
    TIM1->CR1 &= ~CEN;
    TIM1->PSC = 19;  // 1us 1ms=1000us
    TIM1->ARR = 999; //
    TIM1->CNT = 0;
    TIM1->RCR = 0; //*****

    TIM1->DIER = TIM_DIER_UIE | TIM_DIER_TIE;
    TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
    TIM1->CR1 |= (0X03 << TIM_CR1_CMS_Pos); // up/down cnt__
    TIM1->CR1 &= ~(OPM | UDIS | TIM_DIR);   // cnt stops
    // TIM1->EGR |= TIM_EGR_UG;
    TIM1->CCMR1 &= ~TIM_CCMR1_CC2S; // output

    NVIC_SetPriority(TIM1_UP_IRQn, 2);
    NVIC_SetPriority(TIM1_CC_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    TIM1->CR1 |= CEN;
}
void timer1_pwm_delay(uint16_t cyc)
{
    while (timer1_freq != cyc)
        ;
    timer1_freq = 0;

    // TIM1->CR1 &= ~CEN;
}
#endif // __TIMER
