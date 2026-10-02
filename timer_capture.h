#if !defined(_TIM_CAPT)
#define _TIM_CAPT
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
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
| Use            | Main Registers   |
| -------------- | --------------   |
| Delay          | PSC ARR CNT      |
| Interrupt      | DIER SR          |
| PWM            | CCRn CCMRn CCER    |
| Input capture  | CCRn CCMRn CCER  |
| External count | SMCR             |
| One pulse      | CR1              |

| Register | Controls     |
| -------- | ------------ |
| CCMR1    | CH1 + CH2    |
| CCMR2    | CH3 + CH4    |_
| CCER     | all channels |enable Reg
| CCR1     | CH1 value    |
| CCR2     | CH2 value    |
| CCR3     | CH3 value    |
| CCR4     | CH4 value    |
--------------------------------------------
| Channel | CCMR        | CCR  | Enable Bit |
| ------- | ----------- | ---- | ---------- |
| CH1     | CCMR1 lower | CCR1 | CC1E       |
| CH2     | CCMR1 upper | CCR2 | CC2E       |
--------------------------------------------
| CH3     | CCMR2 lower | CCR3 | CC3E       |
| CH4     | CCMR2 upper | CCR4 | CC4E       |
--------------------------------------------
freq=timer/(arr+1)(psc+1)



*/

volatile uint32_t capture;
void TIM2_IRQHandler(void)
{
    // eusart_send(TIM2->SR);

    if ((TIM2->SR & TIM_SR_CC1IF))
    {
        // timer_freq++;
        TIM2->SR &= ~TIM_SR_CC1IF;
        capture = TIM2->CCR1;
        eusart_send((capture & 0xFF00) >> 8);
        eusart_send((capture & 0xff));
    }
    if ((TIM2->SR & TIM_SR_CC2IF))
    {
        // timer1_freq++;
        TIM2->SR &= ~TIM_SR_CC2IF;
        capture = TIM2->CCR2;
        // capture = TIM2->CNT;
        // eusart_send((capture & 0xff00) >> 8);
        // eusart_send((capture & 0xff));
    }
};

// 1ms == 1000us
//
// freq=tm_clk/(PSC+1)(ARR+1)
// 40,000,000÷(50,000×800) = 1hz      ==1 sec
// 40,000,000÷(5000×800) = 100hz
// 40,000,000÷(500×80)     =1000hz    ==1ms
// 40,000,000÷(5×8)        =1000000hz ==1000ms

void timer2_ch1_init() // PA0 in
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    confPinA(FLOAT_INP, 0);

    TIM2->CR1 &= ~CEN;
    //    TIM2->PSC = 499; // 1us 1ms=1000us
    //    TIM2->ARR = 79;  // 1ms

    TIM2->PSC = 49990; //
    TIM2->ARR = 799;   // 1hz
    TIM2->CNT = 0;

    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; // ch1-input
    TIM2->CCER |= TIM_CCER_CC1P;     // fall-edge
    TIM2->CCER |= TIM_CCER_CC1E;     // en capture -->>Reg CCR1
    TIM2->DIER |= TIM_DIER_CC1IE;    // capture intr enable

    // TIM2->CCMR1 |= TIM_CCMR1_IC1F_3;     // filters
    // TIM2->CCMR1 |= TIM_CCMR1_IC1PSC_Msk; // filters

    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
    TIM2->CR1 |= TIM_CR1_CEN;
    // REG--ch1 TIM2->CCR1
}
/*
General purpose | TIM2
CH2     | CCMR1 upper | CCR2 | CC2E
CCMR1    | CH1 + CH2
CCER     | all channels
CCR2     | CH2 value
Interrupt      | DIER SR
*/
void timer2_ch2_init() // pa1
{
    // RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // AFIO->MAPR = AFIO_MAPR_SWJ_CFG_2;
    // AFIO->MAPR |= (0X00 << AFIO_MAPR_TIM2_REMAP_Pos); // PA1
    TIM2->CNT = 0;
    TIM2->CR1 = 0;
    TIM2->CCMR1 = 0;
    TIM2->CCER = 0;
    GPIOA->CRL = (AF_P_P50MHZ << GPIO_CRL_MODE1_Pos); // pA1

    TIM2->PSC = 49999;
    TIM2->ARR = 799; // 1ms
    TIM2->CCR2 = 0;
    TIM2->CNT = 0;
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
    TIM2->CR1 |= (0 << TIM_CR1_CMS_Pos);
    TIM2->CR1 &= ~(TIM_CR1_OPM | TIM_CR1_UDIS | TIM_DIR | TIM_CR1_URS); // cnt stops

    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S; // ch2-output

    TIM2->CCMR1 |= (0X03 << TIM_CCMR1_OC2M_Pos); // toggle ocr2ref
    TIM2->CCER &= ~TIM_CCER_CC2P;                // pin -high
    TIM2->CCER |= TIM_CCER_CC2E;                 // PIN out en

    TIM2->CCMR1 |= TIM_CCMR1_OC2PE; // Preload enable.
    TIM2->DIER = TIM_DIER_CC2IE;
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
    TIM2->CR1 |= CEN;
}

//////
#endif // _TIM_CAPT
