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

// CALLBACK //
typedef void (*callback)(uint32_t);
void readReg(uint32_t val, callback cb)
{
    cb(val);
}
void getRegA(uint32_t val)
{
    GPIOA->ODR = val;
}
void getRegB(uint32_t val)
{
    GPIOB->ODR = (val << 8);
}
// readReg(val,getReg)

#endif // __GPIO
