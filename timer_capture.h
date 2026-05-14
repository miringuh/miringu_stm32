#if !defined(_TIM_CAPT)
#define _TIM_CAPT
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
// #include "portRemaps.h"
#include "eusart.h"
#include "gpio.h"
/*
| Register | Purpose                       |
| -------- | ----------------------------- |
| CR1      | Start/stop timer              |
| PSC      | Divide timer clock            |
| CNT      | Current counter value         |
| ARR      | Maximum count before reset    |
| DIER     | Enable interrupts             |
| SR       | Interrupt flags               |
| CCRx     | Capture/Compare values        |
| CCMRx    | Configure PWM/capture mode    |
| CCER     | Enable capture/output channel |

| Goal                  | Needed              |
| --------------------- | ------------------- |
| Delay                 | overflow            |
| Interrupt             | update event        |
| PWM                   | output compare      |
| Frequency measurement | input capture       |
| Count external pulses | external clock mode |

| Type            | Timers                         |
| --------------- | ------------------------------ |
| Basic           | TIM6/TIM7 (not always present) |
| General purpose | TIM2 TIM3 TIM4                 |
| Advanced        | TIM1                           |

1. What clock enters timer?
2. How fast should counter increment?
3. What value should stop/reset counter?
4. What should happen at event?

Most Important Registers Per Use Case
| Use            | Main Registers |
| -------------- | -------------- |
| Delay          | PSC ARR CNT    |
| Interrupt      | DIER SR        |
| PWM            | CCR CCMR CCER  |
| Input capture  | CCR CCMR CCER  |
| External count | SMCR           |
| One pulse      | CR1            |

| Register | Controls     |
| -------- | ------------ |
| CCMR1    | CH1 + CH2    |
| CCMR2    | CH3 + CH4    |
| CCER     | all channels |
| CCR1     | CH1 value    |
| CCR2     | CH2 value    |
| CCR3     | CH3 value    |
| CCR4     | CH4 value    |
	or
| Channel | CCMR        | CCR  | Enable Bit |
| ------- | ----------- | ---- | ---------- |
| CH1     | CCMR1 lower | CCR1 | CC1E       |
| CH2     | CCMR1 upper | CCR2 | CC2E       |
| CH3     | CCMR2 lower | CCR3 | CC3E       |
| CH4     | CCMR2 upper | CCR4 | CC4E       |
	
*/
volatile uint32_t capture;
void TIM2_IRQHandler(void)
{
    if ((TIM2->SR & TIM_SR_CC1IF))
    {
        // timer_freq++;
        TIM2->SR &= ~TIM_SR_CC1IF;
        capture = TIM2->CCR1;
        // eusart_send((capture & 0xFF00) >> 8);
        eusart_send((capture & 0x00FF));
    }
    if ((TIM2->SR & TIM_SR_CC2IF))
    {
        // timer_freq++;
        TIM2->SR &= ~TIM_SR_CC2IF;
        capture = TIM2->CCR2;
        // eusart_send((capture & 0xFF00) >> 8);
        eusart_send((capture & 0x00FF));
    }
};
void timer2_ch1_init() // PA0 in
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    confPinA(FLOAT_INP, 0);

    TIM2->CR1 &= ~CEN;
    TIM2->PSC = (20 - 1); // 1us 1ms=1000us
    TIM2->ARR = 0xFFFFFFFF;

    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; // ch1-input
    TIM2->CCER |= TIM_CCER_CC1P;  // fall-edge
    TIM2->CCER |= TIM_CCER_CC1E;  // en capture -->>Reg CCR1
    TIM2->DIER |= TIM_DIER_CC1IE; // capture intr enable

    // TIM2->CCMR1 |= TIM_CCMR1_IC1F_3; // filters
    // TIM2->CCMR1 |= TIM_CCMR1_IC1PSC_Msk ; // filters
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
    TIM2->CR1 |= TIM_CR1_CEN ;
    // REG--ch1 TIM2->CCR1
}
void timer2_ch2_init() // pa1
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_2;
    AFIO->MAPR &= ~(AFIO_MAPR_TIM2_REMAP);

    confPinA(AF_P_P50MHZ, 1);

    TIM2->CR1 &= ~CEN;
    TIM2->PSC = (20 - 1); // 1us 1ms=1000us
    TIM2->ARR = 0xFFFFFFFF;

    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S; // ch2-output
    // TIM2->CCMR1 = TIM_CCMR1_ ; //** */
    TIM2->CCER &= ~TIM_CCER_CC1P; // pin-high

    TIM2->CCR2 = 0X03F;

    TIM2->DIER = TIM_DIER_CC2IE;
    // TIM2->CCR2=
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
    TIM2->CR1 |= CEN;
}

#endif // _TIM_CAPT
