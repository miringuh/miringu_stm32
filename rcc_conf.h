#if !defined(__RCC)
#define __RCC
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"

#define FREQ_72MHZ ()

void pllInit()
{
        /*
           RCC->CR
        PLLRDY*
        PLLON
            RCC->CFGR
        PLLXTPRE 1= (div/2) 0=no Div
        PLLSRC   1-hse->pll
        PLLMULT  2,3,4...16
        HPRE AHB sysclk-div 2.4....512

        sw   0-hsi 1-hse 2-pll
        SWS* 0-hsi 1-hse 2-pll
        PPRE2/PPRE1 2,4,8,16
        */

        RCC->CR |= RCC_CR_HSEON;
        while (!(RCC->CR & RCC_CR_HSERDY))
                ;
        FLASH->ACR |= FLASH_ACR_PRFTBE; // enable prefetch buffer
        FLASH->ACR &= FLASH_ACR_LATENCY;
        FLASH->ACR |= FLASH_ACR_LATENCY_2; // FOR 72MHZ

        // RCC->CFGR &= ~RCC_CFGR_HPRE; DIV AHB SYSCLK DIV (2,4,8,16,64,128,256,512)
        RCC->CFGR &= ~RCC_CFGR_PLLXTPRE_HSE; // HSE div fosc or fosc/2
        RCC->CFGR |= RCC_CFGR_PLLSRC;        // HSE as pll entry src--8mhz
        RCC->CFGR = RCC_CFGR_PLLMULL9;       // 1-HSE  0-hsi/2 (2......16) MULTI

        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // 72mhz (**RCC_CFGR_PLLXTPRE_HSE**) (2 4 8 16) DIV
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // apb1=36mhz (2 4 8 16) DIV

        RCC->CR |= RCC_CR_PLLON;
        while (!(RCC->CR & RCC_CR_PLLRDY))
                ;
        RCC->CFGR &= ~RCC_CFGR_SW;
        RCC->CFGR |= RCC_CFGR_SW_PLL; //==SYSCLK
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
                ;
}

void hseInit()
{

        RCC->CR = RCC_CR_HSEON;
        while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY)
                ;
        RCC->CSR = RCC_CR_CSSON;
        while ((RCC->CIR & RCC_CIR_CSSC) == RCC_CIR_CSSC)
        {
                RCC->CIR = RCC_CIR_CSSC;
        }
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // 8mhz
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // 8mhz
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV1; // 8mhz
        RCC->CFGR |= RCC_CFGR_SW_HSE;     // sysclk
        while ((RCC->CFGR & RCC_CFGR_SWS_HSE) != RCC_CFGR_SWS_HSE)
                ;
}
void hse_delay(uint32_t ms)
{
        // uint32_t val = (ms);
        while (ms > 0)
        {
                ms--;
        }
}
//

void rcc_init(void)
{
        RCC->CR |= RCC_CR_HSEON;
        while (!(RCC->CR & RCC_CR_HSERDY))
                ;

        FLASH->ACR |= FLASH_ACR_PRFTBE; // enable prefetch buffer
        FLASH->ACR &= FLASH_ACR_LATENCY;
        FLASH->ACR |= FLASH_ACR_LATENCY_2; // FOR 72MHZ

        RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PPRE2);

        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // apb1=36mhz others 72mhz

        RCC->CR |= RCC_CR_PLLON;
        while (!(RCC->CR & RCC_CR_PLLRDY))
                ;
        RCC->CFGR &= ~RCC_CFGR_SW;
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
                ;
}
void SysTick_Init(void)
{
        SysTick->CTRL = 0;
        SysTick->LOAD = 72000 - 1;
        SysTick->VAL = 0;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}
void _delay_ms(uint32_t ms)
{
        for (uint32_t i = 0; i < ms; i++)
        {
                while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
                        ;
        }
}

#endif // __RCC

/*
#define SET_BIT(REG, BIT) ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT) ((REG) & (BIT))
#define CLEAR_REG(REG) ((REG) = (0x0))
#define WRITE_REG(REG, VAL) ((REG) = (VAL))
#define READ_REG(REG) ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL) (__CLZ(__RBIT(VAL)))

INTERRUPTS

typedef enum
{
        *****Cortex - M3 Processor Exceptions Numbers *************************************************
                          NonMaskableInt_IRQn = -14,
        !< 2 Non Maskable Interrupt
                HardFault_IRQn = -13,
        !< 3 Cortex - M3 Hard Fault Interrupt
                          MemoryManagement_IRQn = -12,
        !< 4 Cortex - M3 Memory Management Interrupt
                          BusFault_IRQn = -11,
        !< 5 Cortex - M3 Bus Fault Interrupt
                          UsageFault_IRQn = -10,
        !< 6 Cortex - M3 Usage Fault Interrupt
                          SVCall_IRQn = -5,
        !< 11 Cortex - M3 SV Call Interrupt
                           DebugMonitor_IRQn = -4,
        !< 12 Cortex - M3 Debug Monitor Interrupt
                           PendSV_IRQn = -2,
        !< 14 Cortex - M3 Pend SV Interrupt
                           SysTick_IRQn = -1,
        !< 15 Cortex - M3 System Tick Interrupt

                           *****STM32 specific Interrupt Numbers *******************************************************WWDG_IRQn = 0,
        !< Window WatchDog Interrupt
                PVD_IRQn = 1,
        !< PVD through EXTI Line detection Interrupt
                TAMPER_IRQn = 2,
        !< Tamper Interrupt
                RTC_IRQn = 3,
        !< RTC global Interrupt
                FLASH_IRQn = 4,
        !< FLASH global Interrupt
                RCC_IRQn = 5,
        !< RCC global Interrupt
                EXTI0_IRQn = 6,
        !< EXTI Line0 Interrupt
                EXTI1_IRQn = 7,
        !< EXTI Line1 Interrupt
                EXTI2_IRQn = 8,
        !< EXTI Line2 Interrupt
                EXTI3_IRQn = 9,
        !< EXTI Line3 Interrupt
                EXTI4_IRQn = 10,
        !< EXTI Line4 Interrupt
                DMA1_Channel1_IRQn = 11,
        !< DMA1 Channel 1 global Interrupt
                DMA1_Channel2_IRQn = 12,
        !< DMA1 Channel 2 global Interrupt
                DMA1_Channel3_IRQn = 13,
        !< DMA1 Channel 3 global Interrupt
                DMA1_Channel4_IRQn = 14,
        !< DMA1 Channel 4 global Interrupt
                DMA1_Channel5_IRQn = 15,
        !< DMA1 Channel 5 global Interrupt
                DMA1_Channel6_IRQn = 16,
        !< DMA1 Channel 6 global Interrupt
                DMA1_Channel7_IRQn = 17,
        !< DMA1 Channel 7 global Interrupt
                ADC1_2_IRQn = 18,
        !< ADC1 and ADC2 global Interrupt
                USB_HP_CAN1_TX_IRQn = 19,
        !< USB Device High Priority or CAN1 TX Interrupts
                                           USB_LP_CAN1_RX0_IRQn = 20,
        !< USB Device Low Priority or CAN1 RX0 Interrupts
                                          CAN1_RX1_IRQn = 21,
        !< CAN1 RX1 Interrupt
                CAN1_SCE_IRQn = 22,
        !< CAN1 SCE Interrupt
                EXTI9_5_IRQn = 23,
        !< External Line [9:5] Interrupts
                TIM1_BRK_IRQn = 24,
        !< TIM1 Break Interrupt
                TIM1_UP_IRQn = 25,
        !< TIM1 Update Interrupt
                TIM1_TRG_COM_IRQn = 26,
        !< TIM1 Trigger and Commutation Interrupt
                TIM1_CC_IRQn = 27,
        !< TIM1 Capture Compare Interrupt
                TIM2_IRQn = 28,
        !< TIM2 global Interrupt
                TIM3_IRQn = 29,
        !< TIM3 global Interrupt
                TIM4_IRQn = 30,
        !< TIM4 global Interrupt
                I2C1_EV_IRQn = 31,
        !< I2C1 Event Interrupt
                I2C1_ER_IRQn = 32,
        !< I2C1 Error Interrupt
                I2C2_EV_IRQn = 33,
        !< I2C2 Event Interrupt
                I2C2_ER_IRQn = 34,
        !< I2C2 Error Interrupt
                SPI1_IRQn = 35,
        !< SPI1 global Interrupt
                SPI2_IRQn = 36,
        !< SPI2 global Interrupt
                USART1_IRQn = 37,
        !< USART1 global Interrupt
                USART2_IRQn = 38,
        !< USART2 global Interrupt
                USART3_IRQn = 39,
        !< USART3 global Interrupt
                EXTI15_10_IRQn = 40,
        !< External Line [15:10] Interrupts
                RTC_Alarm_IRQn = 41,
        !< RTC Alarm through EXTI Line Interrupt
                USBWakeUp_IRQn = 42,
        !< USB Device WakeUp from suspend through EXTI Line Interrupt
} IRQn_Type;
*/