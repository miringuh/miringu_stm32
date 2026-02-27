#if !defined(__I2C_LCD)
#define __I2C_LCD
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "gpio.h"
#include "eusart.h"
#include "i2c.h"

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
#define FUNC_SET_8BIT 0X30
#define FUNC_SET_4BIT 0X20
#define FUNC_SET_2LINE 0X28
#define FUNC_SET_1LINE 0X20
#define FUNC_SET_5X10 0X24
#define FUNC_SET_5X8 0X20
#define FUNC_SET_8BIT_2LINE (FUNC_SET_8BIT | FUNC_SET_2LINE)
#define FUNC_SET_8BIT_1LINE (FUNC_SET_8BIT | FUNC_SET_1LINE)
#define FUNC_SET_4BIT_2LINE (FUNC_SET_4BIT | FUNC_SET_2LINE)
#define FUNC_SET_4BIT_1LINE (FUNC_SET_4BIT | FUNC_SET_1LINE)
#define FUNC_SET_5X10DOT_8BIT_1LINE (FUNC_SET_8BIT_1LINE | FUNC_SET_5X10)
#define FUNC_SET_5X8DOT_8BIT_2LINE (FUNC_SET_8BIT_2LINE | FUNC_SET_5X8)
#define FUNC_SET_5X10DOT_4BIT_1LINE (FUNC_SET_4BIT_1LINE | FUNC_SET_5X10)
#define FUNC_SET_5X8DOT_4BIT_2LINE (FUNC_SET_4BIT_2LINE | FUNC_SET_5X8)
#define RS 0X02
#define RW 0X04
#define EN 0X08
#define i2cdel 20000
volatile uint8_t i2cDummy;
volatile uint8_t lcd4Data;
uint8_t valh = 0;
uint8_t vall = 0;

void setData(uint8_t val)
{
    vall = (comm);
    valh = (comm << 4);
}


void i2cToggle(uint8_t comm, uint8_t mode)
{
    setData(comm);
    i2c1_write(valh);
    i2c1_write(mode | valh);
    _delay_ms(i2cdel);
    i2c1_write(vall);
    i2c1_write(mode | vall);
}

void lcd_i2c_set(uint8_t comm)
{
    setData(comm);
    i2c1_write(EN | valh);
    i2c1_write(0);
    _delay_ms(i2cdel);
}
void lcd_i2c_comm(uint8_t comm)
{
    i2cToggle(comm, EN);
    _delay_ms(i2cdel);
}
void lcd_i2c_write(uint8_t comm)
{
    i2cToggle(comm, RS|EN);
    _delay_ms(i2cdel);
}
// void lcd_Init(uint8_t comm, uint8_t mode)
// {
//     i2c1_init();
//     i2cStart();
//     i2c1_send_address(SLA_W);
//     setData(comm);
//     i2c1_write(valh);
//     i2c1_write(mode | valh);
//     _delay_ms(i2cdel);
//     i2c1_write(vall);
//     i2c1_write(mode | vall);
//     _delay_ms(i2cdel);
// }
void lcd_Init(){

    _delay_ms(50000);
    lcd_i2c_set(0x30);
    _delay_ms(5000);
    lcd_i2c_set(0x30);
    _delay_us(5000);
    lcd_i2c_set(0x30);
    _delay_us(2000);
    lcd_i2c_set(0X20);

    lcd_i2c_comm(DISP_OFF);
    lcd_i2c_comm(CLEAR_DISP);
    lcd_i2c_comm(ENTRY_MODE_INC);
    lcd_i2c_comm(CURSOR_BLINK_ON);
    lcd_i2c_comm(FUNC_SET_5X8DOT_4BIT_2LINE);
    lcd_i2c_comm(HOME);
    lcd_i2c_comm(CLEAR_DISP);
    lcd_i2c_comm(DISP_ON);
}
void lcd_i2c_char(char val)
{
    lcd_i2c_write(val);
    lcd_i2c_comm(DISP_ON);
}
void lcd4DataHigh(unsigned char *word)
{
    char buff[30];
    memset(buff, 0, 30);
    strcpy(buff, word);
    lcd_i2c_comm(CLEAR_DISP);

    for (uint8_t i = 0; i < strlen(word); i++)
    {
        lcd_i2c_char(buff[i]);
    }
}
void lcd4DataLow(unsigned char *word)
{
    char buff[30];
    memset(buff, 0, 30);
    strcpy(buff, word);

    for (uint8_t i = 0; i < strlen(word); i++)
    {
        lcd_i2c_char(buff[i]);
    }
}
void lcdScreenWrite(unsigned char *wordh, unsigned char *wordl)
{
    lcd4DataHigh(wordh);
    lcd_i2c_comm(LINE2);
    lcd4DataLow(wordl);
}
void lcd_stop()
{
    i2c1_stop();
}
#endif // __I2C_LCD
