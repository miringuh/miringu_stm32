#if !defined(_PORT_REMAPS)
#define _PORT_REMAPS
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
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

    timer ch
ch1-   
*/
///// TIMER1 ////////
/*
        TIMER1-------------------
#define TIM1_CH1_MAP0 PA8
#define TIM1_CH2_MAP0 PA9
#define TIM1_CH3_MAP0 PA10
#define TIM1_CH4_MAP0 PA11

#define TIM1_CH1N_MAP0 PB13
#define TIM1_CH2N_MAP0 PB14
#define TIM1_CH3N_MAP0 PB15

#define TIM1_CH1N_MAP1 PA7
#define TIM1_CH2N_MAP1 PB0
#define TIM1_CH3N_MAP1 PB1

#define TIM1_BKIN_MAP0 PB12
#define TIM1_CHIN_MAP0 PB13

#define TIM1_CHIN_MAP1 PA7
#define TIM1_BKIN_MAP1 PA6

#define TIM1_CH2N_MAP0 PB14
#define TIM1_CH2N_MAP1 PB0

#define TIM1_CH3N_MAP0 PB15
#define TIM1_CH3N_MAP1 PB1

        TIMER2----------------------
#define TIM2_CH1_MAP0_ETR PA0
#define TIM2_CH2_MAP0 PA1
#define TIM2_CH3_MAP0 PA2*
#define TIM2_CH4_MAP0 PA3*
            timr2 remap[1:0]=0x01
#define TIM2_CH1_MAP1_ETR PA15
#define TIM2_CH1_MAP1 PA15
#define TIM2_CH2_MAP1 PB3
#define TIM2_CH3_MAP1 PA2*
#define TIM2_CH4_MAP1 PA3*
            timr2 remap[1:0]=0x02
#define TIM2_CH1_MAP1_ETR PA0
#define TIM2_CH2_MAP1 PA1
#define TIM2_CH3_MAP1 PB10
#define TIM2_CH4_MAP1 PB11
            timr2 remap[1:0]=0x03
#define TIM2_CH1_MAP_1_ETR PA15
#define TIM2_CH2_MAP_1 PB3
#define TIM2_CH3_MAP_1 PB10
#define TIM2_CH4_MAP_1 PB11

        TIMER3--------------------------
            timr3 remap[1:0]=0x00
#define TIM3_CH1_MAP0 PA6
#define TIM3_CH2_MAP0 PA7
#define TIM3_CH3_MAP0 PB0
#define TIM3_CH4_MAP0 PB1
            timr3 remap[1:0]=0x02
#define TIM3_CH1_MAP1 PA4
#define TIM3_CH2_MAP1 PA5
#define TIM3_CH3_MAP1 PB0
#define TIM3_CH4_MAP1 PB1
            timr3 remap[1:0]=0x03
#define TIM3_CH1_MAP_1 PC6
#define TIM3_CH2_MAP_1 PC7
#define TIM3_CH3_MAP_1 PC8
#define TIM3_CH4_MAP_1 PC9

        TIMER4-------------------------
            timr4 remap[1:0]=0x00
#define TIM4_CH1_MAP0 PB6
#define TIM4_CH2_MAP0 PB7
#define TIM4_CH3_MAP0 PB8
#define TIM4_CH4_MAP0 PB9
*/

//
#endif // _PORT_REMAPS
