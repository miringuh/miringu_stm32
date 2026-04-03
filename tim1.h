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
/*

*/
#define _1638400MHZ _5MHZ / 327680 // 0.6 us /600ns
#define _819200HZ _5MHZ / 163840   // 1.2 us
#define _409600HZ _5MHZ / 81920    // 2.4 us
#define _204800HZ _5MHZ / 40960    // 4.9 us
#define _102400HZ _5MHZ / 20480    // 9.7 us
#define _51200HZ _5MHZ / 10240     // 19.5 us
#define _25600HZ _5MHZ / 5120      // 39 us
#define _12800HZ _5HZ / 2560       // 78.12 us
#define _6400HZ _5HZ / 1280        // 156.25 us
#define _3200HZ _5HZ / 640         // 312.5 us
#define _1600HZ _5HZ / 320         // 625 us
#define _800HZ _5HZ / 160          // 1.25 ms
#define _600HZ _5HZ / 120          // 1.7 ms
#define _400HZ _5HZ / 80           // 2.5 ms
#define _200HZ _5HZ / 40           // 5 ms
//
#define _160HZ _5HZ / 32           // 6.25 ms
#define _155HZ _5HZ / 31           // 6.5 ms
#define _150HZ _5HZ / 30           // 6.7 ms
#define _145HZ _5HZ / 29           //
#define _140HZ _5HZ / 28           // 7 ms
#define _135HZ _5HZ / 27           //
#define _130HZ _5HZ / 26           //
#define _125HZ _5HZ / 25           //
#define _120HZ _5HZ / 24           // 8.3 ms
#define _115HZ _5HZ / 23           //
#define _110HZ _5HZ / 22           // 9.1 ms
#define _105HZ _5HZ / 21           //
#define _100HZ _5HZ / 20           // 10ms
#define _95HZ _5HZ / 19            //
#define _90HZ _5HZ / 18            //
#define _85HZ _5HZ / 17            //
#define _80HZ _5HZ / 16            // 12.5 ms
#define _75HZ _5HZ / 15            //
#define _70HZ _5HZ / 14            // 14.1 ms
#define _65HZ _5HZ / 13            //
#define _60HZ _5HZ / 12            // 16.7 ms
#define _55HZ _5HZ / 11            //
#define _50HZ _5HZ / 10            // 20 ms
#define _45HZ _5HZ / 9             //
#define _40HZ _5HZ / 8             // 25 ms
#define _35HZ _5HZ / 7             //
#define _30HZ _5HZ / 6             // 33.3 ms
#define _25HZ _5HZ / 5             //
#define _20HZ _5HZ / 4             // 50 ms
#define _15HZ _5HZ / 3             //
#define _10HZ _5HZ / 2             // 100 ms
#define _5HZ 26900                 // 200 ms
#define _2HZ 65500                 // 500 ms
// 1sec ==1000ms == 1,000,000 us
// (1/freq HZ)*1000==ms
//
// #define clk_ms(REG, (1 / FREQ) * 1000) WRITE_REG(REG, FREQ)
#define _1us _819200HZ
#define _2us _409600HZ
#define _5us _204800HZ
#define _20us _51200HZ
#define _39us _25600HZ
#define _78us _12800HZ
#define _625us _1600HZ
#define _1ms _800HZ
#define _2ms _400HZ
#define _5ms _200HZ
#define _10ms _100HZ
#define _20ms _50HZ
#define _50ms _20HZ
#define _100ms _10HZ
#define _200ms _5HZ
#define _500ms _2HZ

// #define PINC13(VAL) WRITE_REG(GPIOC->CRH, (VAL << 20))

volatile uint32_t tim1_cnt = 0;

void TIM_IRQHandler(void)
{
    // TIMx_SR (UIF flag)
    if ((TIM1->SR & UIF_FLAG))
    {
        TIM1->SR &= ~TIM_SR_UIF;
    }
};
// up-count
void timer1_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN;
    // PINC13(P_P2MHZ);

    TIM1->CR1 &= ~CEN;
    TIM1->CR1 = 0;
    TIM1->CR2 = 0;

    TIM1->CR1 &= ~(TIM_DIR | TIM_CR1_CMS_0 | TIM_CR1_CMS_1 | ARPE | UDIS);
    TIM1->SR &= ~(TIM_SR_UIF);

    TIM1->CR1 |= URS | OPM;
    TIM1->CR2 |= (UIE);
    // NVIC_SetPriority(TIM1_UP_IRQn, 3);
    // NVIC_EnableIRQ(TIM1_UP_IRQn);
    TIM1->CR1 |= CEN;
}

void timer1(uint16_t cnt_val, uint16_t pres_val, uint16_t reload)
{
    COUNTER(TIM1->CNT, cnt_val);
    TIM_PRESC(TIM1->PSC, pres_val);
    AUTO_RELOAD(TIM1->ARR, reload);
    while ((TIM1->SR & UIF_FLAG)) // 0=NULL  1-intrr
        ;
    TIM1->SR &= ~TIM_SR_UIF;
    TIM1->CR1 |= TIM_CR1_URS;
    TIM1->CR1 |= CEN;
}
void timer1_del(uint16_t cyc)
{
    uint16_t cnt = cyc;
    while (cnt >= 1)
    {
        timer1(0XFF, 0XFFFF, 0XFFFF);
        cnt--;
    }
    // GPIOC->ODR ^= GPIO_ODR_ODR13;
}
//
// centre Aligned
/*
 cnt 0......>>ARR [OVF]......>CNT O[UDF]
*/
void timer1_centre_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPCEN;

    TIM1->CR1 &= ~CEN;

    TIM1->CR1 &= ~UDIS;
    TIM1->SR &= ~(TIM_SR_UIF);
    TIM1->CR1 = URS | ARPE | OPM;
    TIM1->CR1 |= TIM_CR1_CMS_0; // centre aligned

    TIM1->CR2 &= ~(TIM_CR2_MMS | TIM_CR2_TI1S | TIM_CR2_CCDS);
    TIM1->DIER = UIE;
    TIM1->CR1 |= CEN;
    NVIC_SetPriority(TIM1_UP_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
}

#endif // __TIMER
       /*
       
       
       
       */