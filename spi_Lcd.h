#if !defined(_SPI_LCD)
#define _SPI_LCD
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include "gpio.h"
#include "spi.h"
#include "tim1.h"

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
//
#define PINC14(VAL) WRITE_REG(GPIOC->CRH, (VAL << 24))
#define PINC13(VAL) WRITE_REG(GPIOC->CRH, (VAL << 20))
//           RS  R/W
// command   0   0
// READ BUSY 0   1
// DR-WRITE  1   0
// DR-READ   1   1
uint8_t vall;
uint8_t valh;
#define dely 2

typedef void (*lcdfunc)(uint8_t);
void func_lcd(uint8_t val, lcdfunc cb)
{
    cb(val);
}
//////
void spi_latch()
{
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
    timer1_delay(10);
    GPIOC->ODR |= GPIO_ODR_ODR13;
    timer1_delay(10);
}
void lcd4_init(uint8_t baud) // SPI2
{
    // RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPCEN;
    setPinC(P_P50MHZ, 13);
    spi2_init(baud);
    spi2_send(0);
    spi_latch();
    timer1_delay(50);
}
void lcd4_setup(uint8_t comm)
{
    valh = (comm & 0xF0);
    vall = (comm << 4);
}
void toggle(uint8_t comm, uint8_t mode)
{
    lcd4_setup(comm);

    spi2_send(valh);
    spi_latch();
    spi2_send(mode | valh);
    spi_latch();
    timer1_delay(dely);
    //
    spi2_send(vall);
    spi_latch();
    spi2_send(mode | vall);
    spi_latch();
    timer1_delay(dely);
}
//
void setCGram(uint8_t addr, uint8_t data)
{
    toggle(addr, EN);
    toggle(data, EN);
}
void setDDram(uint8_t addr, uint8_t data)
{
    toggle(0x80 | addr, EN);
    toggle(data, EN);
}
void readram(uint8_t addr)
{
    toggle(addr, RS | RW | EN);
}
//

void lcd_set(uint8_t comm)
{
    lcd4_setup(comm);
    //
    spi2_send(valh);
    spi_latch();
    spi2_send(EN | valh);
    spi_latch();
    timer1_delay(dely);
    //
    spi2_send(0);
    spi_latch();
    timer1_delay(dely);
}
void lcd_command(uint8_t comm)
{
    toggle(comm, EN);
}
void lcd_data(uint8_t comm)
{
    toggle(comm, EN | RS);
}
/////////
void lcd_4_init()
{
    timer1_delay(5);
    lcd_set(0x30);
    timer1_delay(5);
    lcd_set(0x30);
    timer1_delay(1);
    lcd_set(0x30);
    timer1_delay(1);
    lcd_set(0x20);

    lcd_command(DISP_OFF);
    lcd_command(CLEAR_DISP);
    lcd_command(ENTRY_MODE_INC);
    lcd_command(CURSOR_BLINK_ON);
    lcd_command(FUNC_SET_5X8DOT_4BIT_2LINE);
    lcd_command(HOME);
    lcd_command(CLEAR_DISP);
    lcd_command(DISP_ON);
}
void write4Char(uint16_t val) // char
{
    lcd_data(val);
    lcd_command(DISP_ON);
}
void write4DataHigh(char *word)
{
    char buff[30];
    memset(buff, 0, 30);
    strcpy(buff, word);
    lcd_command(CLEAR_DISP);
    for (uint8_t i = 0; i < strlen(word); i++)
    {
        write4Char(buff[i]);
    }
}
void write4DataLow(char *word)
{
    char buff[30];
    memset(buff, 0, 30);
    strcpy(buff, word);
    for (uint8_t i = 0; i < strlen(word); i++)
    {
        write4Char(buff[i]);
    }
}
void write4Data(char *wordh, char *wordl)
{
    write4DataHigh(wordh);
    lcd_command(LINE2);
    write4DataLow(wordl);
}
void lcd4_stop()
{
    spi2_send(0);
    spi_latch();
    timer1_delay(dely);
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
    timer1_delay(dely);
    GPIOC->ODR |= GPIO_ODR_ODR13;
    timer1_delay(dely);
    spi1_stop();
}

//
#endif // _SPI_LCD
