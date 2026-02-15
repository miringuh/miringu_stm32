#if !defined(__TIMER)
#define __TIMER
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include "eusart.h"
//
//     TIMx->CR1  control register 1
#define ARPE TIM_CR1_ARPE // Auto-reload preload enable
#define OPM TIM_CR1_OPM   // Counter stops counting at the next update event
#define URS TIM_CR1_URS   // Only counter overflow/underflow generates an update interrupt
#define UDIS TIM_CR1_UDIS // Update disable
#define CEN TIM_CR1_CEN   // Counter enable
//
//     TIMx->CR2  control register 2
// Reset=0 Enable=001 Update=010
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
//
#define _100HZ 2223 // 10ms
#define _50HZ 4446  // 20ms
#define _25HZ 8892  // 40ms
//
void TIM_IRQHandler(void)
{
    // TIMx_SR (UIF flag)
    CLEAR_BIT(TIM1->SR, TIM_SR_UIF);
};
//

//
void timer6_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->CR1 &= ~(OPM | UDIS);
    TIM1->SR &= ~(TIM_SR_UIF);
    TIM1->CR1 = URS | ARPE;
    MMS(TIM1->CR2, 0);
    TIM1->DIER = UIE;
    TIM1->CR1 |= CEN;
}
void timer6(uint16_t cnt_val, uint16_t pres_val, uint16_t reload)
{
    COUNTER(TIM1->CNT, cnt_val);
    TIM_PRESC(TIM1->PSC, pres_val);
    AUTO_RELOAD(TIM1->ARR, reload);
    // NVIC_SetPriority(TIM1_UP_IRQn, 3);
    // NVIC_EnableIRQ(TIM1_UP_IRQn);
    while (!(TIM1->SR & UIF_FLAG))
    {
    }
}
void tim_del(uint16_t val)
{
    uint16_t cnt = val;
    while (cnt >= 1)
    {
        timer6(0, 0xFFFF, 0XFFFF);
        cnt--;
    }
}

void test_tim6(uint16_t del)
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
    tim_del(del);
}

//
// void timer1_init()
// {
//     RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
//     SET_BIT(TIM6->CR1, TIM_CR1_ARPE);
//     CLEAR_BIT(TIM1->CR1, TIM_CR1_DIR); // 0-up 1-down
//     CLEAR_BIT(TIM1->CR1, TIM_CR1_OPM); // 0-continue cnt 1-cnt stops @ overflow
//     CLEAR_BIT(TIM1->CR1, TIM_CR1_URS); // 0-no event interrupt 1-events intrrupt enabled
//     CLEAR_BIT(TIM1->EGR, TIM_EGR_UG);  // 1-counter an PSC are re-initiallized
//     SET_BIT(TIM1->DIER, TIM_DIER_UIE); // update interrupt 1-enabled
//     SET_BIT(TIM1->CR1, TIM_CR1_CEN);
// }
// // The prescaler can divide the counter clock frequency by any factor between 1 and 65536
// void timer1(uint32_t val, uint32_t prescale)
// {
//     WRITE_REG(TIM1->CNT, val);
//     WRITE_REG(TIM1->PSC, prescale);
//     WRITE_REG(TIM1->ARR, val);
//     SET_BIT(TIM1->CR1, TIM_CR1_CEN);
// }

// void delay_us(uint32_t reload)
// {
//     delay_ms((reload / 1000));
// }

#endif // __TIMER
