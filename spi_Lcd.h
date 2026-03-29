#if !defined(_SPI_LCD)
#define _SPI_LCD
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "gpio.h"
#include "spi.h"
#define HOME 0X02
#define CLEAR_DISP 0X01
#define LINE2 0XC0
// xxxx 0 1 i/d s (i-incr d-decr s-accompany)  **only done on data rd/wr
#define ENTRY_MODE_INC 0X06
#define ENTRY_MODE_DEC 0X04
#define ENTRY_MODE_ACC 0X09
#define ENTRY_MODE_INC_ACC (ENTRY_MODE_INC | ENTRY_MODE_ACC) // 0X0F
#define ENTRY_MODE_DEC_ACC (ENTRY_MODE_DEC | ENTRY_MODE_ACC) // 0X0D
// xxxx 1 D C B (D-disp C-cursor B-blink)
#define DISP_ON 0X0C
#define DISP_OFF 0X08
#define CURSOR_ON (0X02 | DISP_ON) // 0E
#define CURSOR_OFF DISP_ON
#define BLINK_ON (0X01 | DISP_ON)
#define BLINK_OFF (DISP_ON)
#define CURSOR_BLINK_ON (CURSOR_ON | BLINK_ON)

// xxx1 s/c r/l 0 0 ( s/c (1-disp 0-cursor)shift or moving r/l (1)r-right (0)l-left  )
#define MOVE_CURSOR_RIGHT 0X14  // Ob0001 0100
#define MOVE_CURSOR_LEFT 0X10   // Ob0001 0000
#define MOVE_DISPLAY_RIGHT 0X1C // Ob0001 1100
#define MOVE_DISPLAY_LEFT 0X18  // Ob0001 1000
//
// xx 1 d/l  n f x x (d/l 1-8bit 0-4bit) (n 1-2ln 0-1ln) (f 1-5x10 0-5x8)
#define FUNC_SET_2LINE 0X28
#define FUNC_SET_1LINE 0X20
#define FUNC_SET_4BIT 0X20
#define FUNC_SET_4BIT_2LINE (FUNC_SET_4BIT | FUNC_SET_2LINE)
#define FUNC_SET_4BIT_1LINE (FUNC_SET_4BIT | FUNC_SET_1LINE)
// #define FUNC_SET_8BIT 0X30
// #define FUNC_SET_8BIT_2LINE (FUNC_SET_8BIT | FUNC_SET_2LINE)
// #define FUNC_SET_8BIT_1LINE (FUNC_SET_8BIT | FUNC_SET_1LINE)
#define FUNC_SET_5X10 0X24
#define FUNC_SET_5X10DOT_8BIT_1LINE (FUNC_SET_8BIT_1LINE | FUNC_SET_5X10)
#define FUNC_SET_5X10DOT_4BIT_1LINE (FUNC_SET_4BIT_1LINE | FUNC_SET_5X10)
#define FUNC_SET_5X8 0X20
#define FUNC_SET_5X8DOT_8BIT_2LINE (FUNC_SET_8BIT_2LINE | FUNC_SET_5X8)
#define FUNC_SET_5X8DOT_4BIT_2LINE (FUNC_SET_4BIT_2LINE | FUNC_SET_5X8)
#define FUNC_SET_5X8DOT_4BIT_1LINE (FUNC_SET_4BIT_1LINE | FUNC_SET_5X8)
#define FUNC_SET_5X10DOT_4BIT_2LINE (FUNC_SET_4BIT_2LINE | FUNC_SET_5X10)
#define RS 0X01
#define RW 0X02
#define EN 0X04
//           RS  R/W
// command   0   0
// READ BUSY 0   1
// DR-WRITE  1   0
// DR-READ   1   1
uint8_t vall;
uint8_t valh;
#define dely 2

void lcd4_init(uint8_t baud)
{
    RCC->APB1ENR = RCC ;
    spi0_init(baud);
    spi0_send(0);
}

#endif // _SPI_LCD
