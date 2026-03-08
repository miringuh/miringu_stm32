#if !defined(__TIMER)
#define __TIMER
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include "eusart.h"
//
//     TIMx->CR1  control register 1
#define CLK_DIV(REG, VAL) WRITE_REG(REG, VAL) // 00 Clock Division.
// Auto-reload preload enable 1: TIMx_ARR register is buffered.
#define ARPE TIM_CR1_ARPE
// Center-aligned Mode Selection
#define CMS(REG, VAL) WRITE_REG(REG, VAL) // 00
// Direction
// 1-downcnt 0-upcnt
#define TIM_DIR TIM_CR1_DIR
// Counter stops counting 0-no stop  1-the next update event & CEN is hware sets to 0
#define OPM TIM_CR1_OPM
// This bit is set and cleared by software to select the UEV event sources.
#define URS TIM_CR1_URS   // Only counter ovf/underflow generates an update interrupt
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
// 25HZ=8892 50HZ=4446 100HZ=2223
/*
((2385×100)÷50)÷4
*/
#define _204800HZ 0  // 3us
#define _102400HZ 1  // 6us
#define _51200HZ 2   // 11us
#define _25600HZ 4   // 41us
#define _12800HZ 9  // 82us
#define _6400HZ 19   // 163us
#define _3200HZ 37   // 325us
#define _1600HZ 74   // 650us
#define _800HZ 149   // 1.25ms
#define _400HZ 298   // 2.5ms
#define _200HZ 596   // 5ms
#define _100HZ 1192  // 10ms
#define _50HZ 2385   // 20ms
#define _25HZ 4760   // 40ms
#define _12HZ 9880   // 83ms
#define _6HZ 19760   //
#define _3HZ 39520   //
//
void test_tim6();
void TIM_IRQHandler(void)
{
    // TIMx_SR (UIF flag)
    if ((TIM1->SR & UIF_FLAG))
    {
        TIM1->SR &= ~TIM_SR_UIF;
    }
};

void timer1_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->CR1 &= ~(OPM | UDIS | TIM_DIR | TIM_CR1_CMS);
    TIM1->SR &= ~(TIM_SR_UIF);
    TIM1->CR1 = URS | ARPE;
    TIM1->CR2 &= ~(TIM_CR2_MMS | TIM_CR2_TI1S | TIM_CR2_CCDS);
    TIM1->DIER = UIE;
    // TIM1->CR1 |= TIM_CR1_;
    //
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }
    if (!(GPIOC->CRH & GPIO_CRH_MODE13_Msk))
    {
        GPIOC->CRH = GPIO_CRH_MODE13_Msk; // 50MHZ P_P
    }
    //
    // TIM1->CR1 |= TIM_CR1_ ;
    TIM1->CR1 |= CEN;
}
void timer1(uint16_t cnt_val, uint16_t pres_val, uint16_t reload)
{
    COUNTER(TIM1->CNT, cnt_val);
    TIM_PRESC(TIM1->PSC, pres_val);
    AUTO_RELOAD(TIM1->ARR, reload);
    NVIC_SetPriority(TIM1_UP_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    // while ((TIM1->SR & UIF_FLAG))
    // {
    // }
}
void tim_del(uint16_t cyc)
{
    uint16_t cnt = cyc;
    while (cnt >= 1)
    {
        timer1(0, 19, 0XFFFE);
        cnt--;
    }
    test_tim6();
}

void test_tim6()
{
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    // tim_del(del);
}

#endif // __TIMER
       /*
       
       
       
       */