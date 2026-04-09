#if !defined(__GPIO)
#define __GPIO
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"

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
/*
MODES @ gpio.h 2MHZ 10MHZ or 50MHZ
CRL   7   |6   |5   |4   |3   |2  |1  |0
POS   28  |24  |20  |16  |12  |8  |4  |0 **********
CRH   15  |14  |13  |12  |11  |10 |9  |8
*/
// REG manipulations
#define PIN_MODE(REG, VAL) WRITE_REG(REG, VAL)

#define WR_REG(REG, VAL) WRITE_REG(REG, VAL)
#define RD_REG(REG, VAL) READ_REG(REG, VAL)
#define CLR_REG(REG, VAL) CLEAR_REG(REG, VAL)
// BITS manipulations
#define SET_REG_BIT(REG, BIT) SET_BIT(REG, BIT)
#define READ_REG_BIT(REG, BIT) READ_BIT(REG, BIT)
#define CLR_REG_BIT(REG, BIT) CLEAR_BIT(REG, BIT)
//
////////// SET PIN CONFS
#define REG_A 1
#define REG_B 2
#define REG_C 3
void setPinC(uint8_t conf_val, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    switch (pin)
    {
    case 13:
        GPIOC->CRH |= (conf_val << 20);
        break;
    case 14:
        GPIOC->CRH |= (conf_val << 24);
        break;
    case 15:
        GPIOC->CRH |= (conf_val << 28);
        break;
    default:

        break;
    }
}
void setPinB(uint8_t conf_val, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    switch (pin)
    {
    case 0:
        GPIOB->CRL |= (conf_val);
        break;
    case 1:
        GPIOB->CRL |= (conf_val << 4);
        break;
    // case 2:
    //     GPIOB->CRL |= (conf_val << 8);
    //     break;
    case 3:
        GPIOB->CRL |= (conf_val << 12);
        break;
    case 4:
        GPIOB->CRL |= (conf_val << 14);
        break;
    case 5:
        GPIOB->CRL |= (conf_val << 20);
        break;
    case 6:
        GPIOB->CRL |= (conf_val << 24);
        break;
    case 7:
        GPIOB->CRL |= (conf_val << 28);
        break;
    case 8:
        GPIOB->CRH |= (conf_val);
        break;
    case 9:
        GPIOB->CRH |= (conf_val << 4);
        break;
    case 10:
        GPIOB->CRH |= (conf_val << 8);
        break;
    case 11:
        GPIOB->CRH |= (conf_val << 12);
        break;
    case 12:
        GPIOB->CRH |= (conf_val << 14);
        break;
    case 13:
        GPIOB->CRH |= (conf_val << 20);
        break;
    case 14:
        GPIOB->CRH |= (conf_val << 24);
        break;
    case 15:
        GPIOB->CRH |= (conf_val << 28);
        break;
    default:
        break;
    }
}
void setPinA(uint8_t conf_val, uint8_t pin)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    switch (pin)
    {
    case 0:
        GPIOA->CRL |= (conf_val);
        break;
    case 1:
        GPIOA->CRL |= (conf_val << 4);
        break;
    case 2:
        GPIOA->CRL |= (conf_val << 8);
        break;
    case 3:
        GPIOA->CRL |= (conf_val << 12);
        break;
    case 4:
        GPIOA->CRL |= (conf_val << 14);
        break;
    case 5:
        GPIOA->CRL |= (conf_val << 20);
        break;
    case 6:
        GPIOA->CRL |= (conf_val << 24);
        break;
    case 7:
        GPIOA->CRL |= (conf_val << 28);
        break;
    case 8:
        GPIOA->CRH |= (conf_val);
        break;
    case 9:
        GPIOA->CRH |= (conf_val << 4);
        break;
    case 10:
        GPIOA->CRH |= (conf_val << 8);
        break;
    case 11:
        GPIOA->CRH |= (conf_val << 12);
        break;
    case 12:
        GPIOA->CRH |= (conf_val << 14);
        break;
    // case 13:
    //     GPIOA->CRH |= (conf_val << 20);
    //     break;
    // case 14:
    //     GPIOA->CRH |= (conf_val << 24);
    //     break;
    case 15:
        GPIOA->CRH |= (conf_val << 28);
        break;
    default:
        break;
    }
}
void gpioConfig(uint32_t regv, uint8_t conf_mode, uint8_t pos)
{
    if (regv == REG_A)
    {
        setPinA(conf_mode, pos);
    }
    if (regv == REG_B)
    {
        setPinB(conf_mode, pos);
    }
    if (regv == REG_C)
    {
        setPinC(conf_mode, pos);
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
void func_void_2param(uint32_t val0, uint32_t val1, callback3 cb)
{
    cb(val0, val1);
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
uint32_t func_return_2param(uint32_t val0, uint32_t val1, callback6 cb)
{
    return cb(val0, val1);
}

#endif // __GPIO
