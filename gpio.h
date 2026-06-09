#if !defined(__GPIO)
#define __GPIO
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"

#define P_P2MHZ 0b0010 // CNFn<>MODEn
#define O_D2MHZ 0b0110
#define AF_P_P2MHZ 0b1010
#define AF_O_D2MHZ 0b1110
//
#define P_P10MHZ 0b0001
#define O_D10MHZ 0b0101
#define AF_P_P10MHZ 0b1001
#define AF_O_D10MHZ 0b1101 // Open drain
//
#define P_P50MHZ 0b0011
#define O_D50MHZ 0b0111
#define AF_P_P50MHZ 0b1011
#define AF_O_D50MHZ 0b1111
// INPUT
#define ANALOG 0b0000
#define FLOAT_INP 0b0100 // DEFAULT
#define INP_PPULL 0b1000
//
// #define PORT_IO(REG,VAL) WRITE_REG()
/*
MODES @ gpio.h 2MHZ 10MHZ or 50MHZ
CRL   7   |6   |5   |4   |3   |2  |1  |0
POS   28  |24  |20  |16  |12  |8  |4  |0 **********
CRH   15  |14  |13  |12  |11  |10 |9  |8
*/
//// SET PIN CONFS

#define REG_A 1
#define REG_B 2
#define REG_C 3
void confPinC(uint16_t conf_mode, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    switch (pin)
    {
    case 13:
        GPIOC->CRH = (conf_mode << GPIO_CRH_MODE13_Pos);
        break;
    case 14:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE14_Pos);
        break;
    case 15:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE15_Pos);
        break;
    default:

        break;
    }
}
void confPinB(uint16_t conf_mode, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    switch (pin)
    {
    case 0:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE0_Pos);
        break;
    case 1:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE1_Pos);
        break;
    case 3:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE3_Pos);
        break;
    case 4:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE4_Pos);
        break;
    case 5:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE5_Pos);
        break;
    case 6:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE6_Pos);
        break;
    case 7:
        GPIOB->CRL = (conf_mode << GPIO_CRL_MODE7_Pos);
        break;
    case 8:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE8_Pos);
        break;
    case 9:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE9_Pos);
        break;
    case 10:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE10_Pos);
        break;
    case 11:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE11_Pos);
        break;
    case 12:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE12_Pos);
        break;
    case 13:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE13_Pos);
        break;
    case 14:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE14_Pos);
        break;
    case 15:
        GPIOB->CRH = (conf_mode << GPIO_CRH_MODE15_Pos);
        break;
    default:
        break;
    }

}
void confPinA(uint16_t conf_mode, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // uint32_t mem_l = GPIOA->CRL;
    // uint32_t mem_h = GPIOA->CRH;

    switch (pin)
    {
    case 0:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE0_Pos);
        break;
    case 1:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE1_Pos);
        break;
    case 2:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE2_Pos);
        break;
    case 3:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE3_Pos);
        break;
    case 4:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE4_Pos);
        break;
    case 5:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE5_Pos);
        break;
    case 6:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE6_Pos);
        break;
    case 7:
        GPIOA->CRL = (conf_mode << GPIO_CRL_MODE7_Pos);
        break;
    case 8:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE8_Pos);
        break;
    case 9:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE9_Pos);
        break;
    case 10:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE10_Pos);
        break;
    case 11:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE11_Pos);
        break;
    case 12:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE12_Pos);
        break;
    case 15:
        GPIOA->CRH = (conf_mode << GPIO_CRH_MODE15_Pos);
        break;
    default:
        break;
    }
}

void gpioConfig(uint32_t regv, uint8_t conf_mode, uint8_t pos)
{
    if (regv == REG_A)
    {
        confPinA(conf_mode, pos);
    }
    if (regv == REG_B)
    {
        confPinB(conf_mode, pos);
    }
    if (regv == REG_C)
    {
        confPinC(conf_mode, pos);
    }
}
//////////
// CALLBACK //
typedef void (*callback1)(void);
void func_void(callback1 cb)
{
    cb();
}
typedef void (*callback2)(uint32_t);
void func_void_param(uint32_t val, callback2 cb)
{
    cb(val);
}
typedef void (*callback3)(uint32_t, uint32_t);
void func_void_2param(uint32_t v, uint32_t val1, callback3 cb)
{
    cb(v, val1);
}
/////
typedef uint32_t (*callback4)(void);
uint32_t func_return(callback4 cb)
{
    return cb();
}
typedef uint32_t (*callback5)(uint32_t);
uint32_t func_return_param(uint32_t val, callback5 cb)
{
    return cb(val);
}
typedef uint32_t (*callback6)(uint32_t, uint32_t);
uint32_t func_return_2param(uint32_t v, uint32_t val1, callback6 cb)
{
    return cb(v, val1);
}

#endif // __GPIO
