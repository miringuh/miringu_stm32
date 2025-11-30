#if !defined(__TIMER)
#define __TIMER
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"

void TIM_IRQHandler(void)
{
    // TIMx_SR (UIF flag)
    CLEAR_BIT(TIM1->SR, TIM_SR_UIF);
};

void timer1_init()
{
    // RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    SET_BIT(TIM1->CR1, TIM_CR1_ARPE);
    CLEAR_BIT(TIM1->CR1, TIM_CR1_DIR); // 0-up 1-down
    CLEAR_BIT(TIM1->CR1, TIM_CR1_OPM); // 0-continue cnt 1-cnt stops @ overflow
    CLEAR_BIT(TIM1->CR1, TIM_CR1_URS); // 0-no event interrupt 1-events intrrupt enabled
    CLEAR_BIT(TIM1->EGR, TIM_EGR_UG);  // 1-counter an PSC are re-initiallized
    SET_BIT(TIM1->DIER, TIM_DIER_UIE); // update interrupt 1-enabled
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
}
// The prescaler can divide the counter clock frequency by any factor between 1 and 65536
void timer1(uint32_t val, uint32_t prescale)
{
    WRITE_REG(TIM1->CNT, val);
    WRITE_REG(TIM1->PSC, prescale);
    WRITE_REG(TIM1->ARR, val);
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
}
void delay_ms(uint32_t reload)
{
    timer1(60000, reload);
    while (TIM1->CNT != 0)
    {
    }
}
void delay_us(uint32_t reload)
{
    delay_ms((reload / 1000));
}

#endif // __TIMER
