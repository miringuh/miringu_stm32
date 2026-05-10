#if !defined(_PORT_REMAPS)
#define _PORT_REMAPS
#include "/home/jeff/STM32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include "gpio.h"
// TIMERS REMAPPING
/*MODES @ gpio.h 2MHZ 10MHZ or 50MHZ
CRL   7   |6   |5   |4   |3   |2  |1  |0
POS   28  |24  |20  |16  |12  |8  |4  |0 **********
CRH   15  |14  |13  |12  |11  |10 |9  |8
*/
//////// TIMER4 ///////////////
#define TIM4_CH1_MAP0(VAL) WRITE_REG(GPIOB->CRL, (VAL << 24)) // PB6
#define TIM4_CH2_MAP0(VAL) WRITE_REG(GPIOB->CRL, (VAL << 28)) // PB7
#define TIM4_CH3_MAP0(VAL) WRITE_REG(GPIOB->CRH, VAL)         // PB8
#define TIM4_CH4_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 4))  // PB9

/////// TIMER3 //////////////
#define TIM3_CH1_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 24)) // PA6
#define TIM3_CH2_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 28)) // PA7
#define TIM3_CH3_MAP0(VAL) WRITE_REG(GPIOB->CRL, VAL)         // PB0
#define TIM3_CH4_MAP0(VAL) WRITE_REG(GPIOB->CRL, (VAL << 4))  // PB1
//
#define TIM3_CH1_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 16)) // PB4
#define TIM3_CH2_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 20)) // PB5

//////// TIMER2 ///////////////
#define TIM2_CH1_MAP0_ETR(VAL) WRITE_REG(GPIOA->CRL, VAL)     // PA0
#define TIM2_CH1_MAP0(VAL) WRITE_REG(GPIOA->CRL, VAL)         // PA0
#define TIM2_CH2_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 4))  // PA1
#define TIM2_CH3_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 8))  // PA2
#define TIM2_CH4_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 12)) // PA3
//
#define TIM2_CH1_MAP1_ETR(VAL) WRITE_REG(GPIOA->CRH, (VAL << 28)) // PA15
#define TIM2_CH1_MAP1(VAL) WRITE_REG(GPIOA->CRH, (VAL << 28))     // PA15
#define TIM2_CH2_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 12))     // PB3

///////// TIMER 1 //////////////
#define TIM1_MAP0_ETR(VAL) WRITE_REG(GPIOA->CRH, (VAL << 16)) // PA12
#define TIM1_CH1_MAP0(VAL) WRITE_REG(GPIOA->CRH, VAL)         // PA8
#define TIM1_CH2_MAP0(VAL) WRITE_REG(GPIOA->CRH, (VAL << 4))  // PA9
#define TIM1_CH3_MAP0(VAL) WRITE_REG(GPIOA->CRH, (VAL << 8))  // PA10
#define TIM1_CH4_MAP0(VAL) WRITE_REG(GPIOA->CRH, (VAL << 12)) // PA11

///////// TIMER 1_N //////////////
#define TIM1_BKIN_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 16)) // PB12
#define TIM1_BKIN_MAP1(VAL) WRITE_REG(GPIOA->CRL, (VAL << 24)) // PA6
#define TIM1_CH1N_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 20)) // PB13
#define TIM1_CH1N_MAP1(VAL) WRITE_REG(GPIOA->CRL, (VAL << 28)) // PA7
#define TIM1_CH2N_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 24)) // PB14
#define TIM1_CH2N_MAP1(VAL) WRITE_REG(GPIOB->CRL, VAL)         // PB0
#define TIM1_CH3N_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 28)) // PB15
#define TIM1_CH3N_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 4))  // PB1

//////// USART3 //////////
#define USART3_TX_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 8))  // PB10
#define USART3_RX_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 12)) // PB11
#define USART3_CK_MAP0(VAL) WRITE_REG(GPIOB->CRH, (VAL << 16)) // PB12
#define USART3_CTS(VAL) WRITE_REG(GPIOB->CRH, (VAL << 20))     // PB13
#define USART3_RTS(VAL) WRITE_REG(GPIOB->CRH, (VAL << 24))     // PB14

///////// USART2 //////////
#define USART2_TX_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 8))  // PA2
#define USART2_RX_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 12)) // PA3
#define USART2_CK_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 16)) // PA4
#define USART2_CTS_MAP0(VAL) WRITE_REG(GPIOA->CRL, VAL)        // PA0
#define USART2_RTS_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 4)) // PA1

//////// USART 0/1 ////
#define USART1_TX_MAP0(VAL) WRITE_REG(GPIOA->CRH, (VAL << 4)) // PA9
#define USART1_RX_MAP0(VAL) WRITE_REG(GPIOA->CRH, (VAL << 8)) // PA10

#define USART1_TX_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 24)) // PB6
#define USART1_RX_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 28)) // PB7

//////// I2C 0/1 /////////////////
#define I2C1_SCL_MAP0(VAL) WRITE_REG(GPIOB->CRL, (VAL << 24)) // PB6
#define I2C1_SDA_MAP0(VAL) WRITE_REG(GPIOB->CRL, (VAL << 28)) // PB7

#define I2C1_SCL_MAP1(VAL) WRITE_REG(GPIOB->CRH, VAL)         // PB8
#define I2C1_SDA_MAP1(VAL) WRITE_REG(GPIOB->CRH, (VAL << 4)) // PB9

///////// SPI 0/1 //////////////////
#define SPI1_NSS_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 16)) // PA4
#define SPI1_SCK_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 20)) // PA5
#define SPI1_MISO_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 24)) // PA6
#define SPI1_MOS1_MAP0(VAL) WRITE_REG(GPIOA->CRL, (VAL << 28)) // PA7

#define SPI1_NSS_MAP1(VAL) WRITE_REG(GPIOA->CRH, (VAL << 28)) // PA15
#define SPI1_SCK_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 12)) // PB3
#define SPI1_MISO_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 16))  // PB4
#define SPI1_MOS1_MAP1(VAL) WRITE_REG(GPIOB->CRL, (VAL << 20)) // PB5

//
#endif // _PORT_REMAPS
