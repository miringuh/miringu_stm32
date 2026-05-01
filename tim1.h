#if !defined(__TIMER)
#define __TIMER
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include "eusart.h"
#include "gpio.h"
//
//     TIMx->CR1  control register 1
#define CLK_DIV(REG, VAL) WRITE_REG(REG, VAL) // 00 Clock Division.
// Auto-reload preload enable 1: TIMx_ARR register is buffered.
#define ARPE TIM_CR1_ARPE
// Center-aligned Mode Selection
#define CMS(REG, VAL) WRITE_REG(REG, VAL) // 00
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
#define MMS(REG, VAL) WRITE_REG(REG, VAL) // Master mode selection
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
// freq=fCK_PSC / (PSC[15:0] + 1).
#define TIM_PRESC(REG, VAL) WRITE_REG(REG, VAL) // PRESC[15:0]
//
// TIMx_ARR auto-reload register
#define AUTO_RELOAD(REG, VAL) WRITE_REG(REG, VAL) // ARR[15:0]
//
#define _12800HZ _5HZ / 2560 // .07 us
#define _6400HZ _5HZ / 1280  // .16 us
#define _3200HZ _5HZ / 640   // .31 us
#define _1600HZ _5HZ / 320   // .625 us
#define _800HZ _5HZ / 160    // 1.25 ms
#define _400HZ _5HZ / 80     // 2.5 ms
#define _200HZ _5HZ / 40     // 5 ms
#define _195HZ _5HZ / 39     //
#define _190HZ _5HZ / 38     //
#define _185HZ _5HZ / 37     //
#define _180HZ _5HZ / 36     //
#define _175HZ _5HZ / 35     //
#define _170HZ _5HZ / 34     //
#define _165HZ _5HZ / 33     //
#define _160HZ _5HZ / 32     // 6.25ms
#define _155HZ _5HZ / 31     //
#define _150HZ _5HZ / 30     // 7ms
#define _145HZ _5HZ / 29     //
#define _140HZ _5HZ / 28     //
#define _135HZ _5HZ / 27     //
#define _130HZ _5HZ / 26     //
#define _125HZ _5HZ / 25     //
#define _120HZ _5HZ / 24     // 8.3ms
#define _115HZ _5HZ / 23     //
#define _110HZ _5HZ / 22     //
#define _105HZ _5HZ / 21     //
#define _100HZ _5HZ / 20     // 10 ms
#define _50HZ _5HZ / 10      //
#define _40HZ _5HZ / 8       // 25 ms
#define _35HZ _5HZ / 7       //
#define _30HZ _5HZ / 6       // 33.3ms
#define _25HZ _5HZ / 5       // 40 ms
#define _20HZ _5HZ / 4       // 50 ms
#define _15HZ _5HZ / 3       //
#define _10HZ _5HZ / 2       // 100 ms
#define _5HZ 11400
#define _2HZ 28000
#define _1HZ 56000

/*
freq(hz)=Tclk/((PSC+1)(ARR+1))
time(ms)=(1/hz)*1000
*/
volatile uint32_t tim1_cnt = 0;
void TIM1_IRQHandler(void)
{
    if ((TIM1->SR & TIM_SR_UIF))
    {
        TIM1->SR &= ~TIM_SR_UIF;
    }
    NVIC_DisableIRQ(TIM1_UP_IRQn);
};
void TIM3_IRQHandler(void)
{
    if ((TIM3->SR & TIM_SR_UIF))
    {
        TIM3->SR &= ~TIM_SR_UIF;
    }
    NVIC_DisableIRQ(TIM3_IRQn);
};
void TIM4_IRQHandler(void)
{
    if ((TIM4->SR & TIM_SR_UIF))
    {
        TIM4->SR &= ~TIM_SR_UIF;
    }
    NVIC_DisableIRQ(TIM4_IRQn);
};

// TIMER 3/4 WORKING****
void timer3()
{
    // RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN;
    // tim1_cnt = 0;
    TIM3->CR1 &= ~CEN;
    TIM3->PSC = (20 - 1); // 1us 1ms=1000us
    TIM3->ARR = 1000 - 1;
    TIM3->CNT = 0;
    TIM3->DIER = TIM_DIER_UIE; //|TIM_DIER_TIE;
    TIM3->CR1 = TIM_CR1_ARPE;  // | TIM_CR1_OPM || TIM_CR1_UDIS; // cnt stops
    TIM3->EGR |= TIM_EGR_UG;

    NVIC_SetPriority(TIM3_IRQn, 3);
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= CEN;
}
void timer3_delay(uint16_t cyc)
{
    for (uint16_t i = 0; i < cyc; i++)
    {
        timer3();
    }
}
//
// TIMER 4
void timer4()
{
    // RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN;
    // tim1_cnt = 0;
    TIM4->CR1 &= ~CEN;
    TIM4->PSC = (20 - 1); // 1us 1ms=1000us
    TIM4->ARR = 1000 - 1;
    TIM4->CNT = 0;
    TIM4->DIER = TIM_DIER_UIE; //|TIM_DIER_TIE;
    TIM4->CR1 = TIM_CR1_ARPE;  // | TIM_CR1_OPM || TIM_CR1_UDIS; // cnt stops
    TIM4->EGR |= TIM_EGR_UG;

    NVIC_SetPriority(TIM4_IRQn, 3);
    NVIC_EnableIRQ(TIM4_IRQn);
    TIM4->CR1 |= CEN;
}
void timer4_delay(uint16_t cyc)
{
    for (uint16_t i = 0; i < cyc; i++)
    {
        timer4();
    }
}

//
#endif // __TIMER
