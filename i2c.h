#if !defined(__I2C)
#define __I2C
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "gpio.h"
/*
Setting the START bit while the BUSY bit is cleared generates a Start condition and switch to Master mode (M/SL bit set)

Once the Start condition is sent:
The SB bit is set by hardware and an interrupt is generated if the ITEVFEN bit is set.

Then the master waits for a read of the SR1 register followed by a write in the DR register with Slave address

The ADDR bit is set by hardware and an interrupt is generated if the ITEVFEN bit is set.

Then the master waits for a read of the SR1 register followed by a read of the SR2 register

The master can decide to enter Transmitter or Receiver mode depending on the LSB of the slave address sent.
In 7-bit addressing mode,
–To enter Tx mode, a master sends the slave address with LSB reset.(WRITE) xxxx xxx0 0x27 <<1 0x46
–To enter Rx mode, a master sends the slave address with LSB set.  (READ)  xxxx xxx1 0x27 <<1 0x36

In 10-bit addressing mode,
(The TRA bit indicates  master/Receiver or Transmitter mode.)
–To enter Tx mode, a master sends the header (11110xx0) then the slave address with LSB reset, (where xx denotes MSB of the address).
–To enter RX mode, a master sends the header (11110xx0) then the slave address with LSB reset. Then send a repeated Start condition
    then header (11110xx1), (where xx denotes MSB of the two address)

        Master TX
master sends bytes
master waits until TxE is cleared,

When the ack pulse is received:The TxE bit is set by hardware and  interr is gen if the ITEVFEN and ITBUFEN bits are set.
If TxE is set and DR reg. was not written before the end of the last data TX, BTF is set and the interface waits until BTF is cleared.

STOP bit is set by software to generate a Stop condition
*/
//
//  (I2C_CR1) Control register 1
// used to reinitialize the peripheral after an error or a locked state
#define SWRST I2C_CR1_SWRST // if the BUSY bit is set and remains locked
/* This bit is set and cleared by software, and cleared by hardware when PEC is transferred or
by a START or Stop condition or when PE = 0*/
#define PEC I2C_CR1_PEC
// Acknowledge/PEC Position (for data reception)
#define POS I2C_CR1_PEC
// This bit is set and cleared by software and cleared by hardware when PE=0
#define ACK I2C_CR1_ACK // Acknowledge enable
//
#define STOP I2C_CR1_STOP
#define START I2C_CR1_START
// This bit is used to disable clock stretching in slave mode when ADDR or BTF flag is se
#define NOSTRETCH ~(I2C_CR1_NOSTRETCH)
#define ENGC I2C_CR1_ENGC   // general call enable
#define ENPEC I2C_CR1_ENPEC // pec enable
#define PE I2C_CR1_PE       // peripheral enable

//   (I2C_CR2)   Control register 2
//
#define ITBUFEN I2C_CR2_ITBUFEN // Buffer interrupt enable
/*
SB = 1 (Master)
ADDR = 1 (Master/Slave)
ADD10= 1 (Master)
STOPF = 1 (Slave)
BTF = 1 with no TxE or RxNE event
TxE event to 1 if ITBUFEN = 1
RxNE event to 1if ITBUFEN = 1
*/
#define ITEVTEN I2C_CR2_ITEVTEN // Event interrupt enable
/*
BERR = 1
ARLO = 1
AF = 1
OVR = 1
PECERR = 1
TIMEOUT = 1
SMBALERT = 1
*/
#define ITERREN I2C_CR2_ITERREN // Error interrupt enable
/*
The minimum allowed frequency is 2 MHz,
the maximum frequency is limited by the maximum APB frequency and cannot exceed
50 MHz
*/
#define FREQ(REG, VAL) WRITE_REG(REG, VAL) //[5:0] 2=2MHZ......50MHZ=0X32 or 50
//
//  (I2C_OAR1)  Own address register 1
// Addressing mode(slave mode)
#define ADDMODE I2C_OAR1_ADDMODE              // 0==7-bit slave 1==10bit_slave
#define ADD10_H(REG, VAL) WRITE_REG(REG, VAL) //[9:8] 10-bit addr
#define ADD7_L(REG, VAL) WRITE_REG(REG, VAL)  //[7:1] 7-bit addr <<1
#define ADD10_L I2C_OAR1_ADD0                 // 10-bit addr
//
//       (I2C_OAR2)  Own address register 2
//[7:1] bits 7:1 of address in dual addressing mode
#define ADD2(REG, VAL) WRITE_REG(REG, VAL) 
//
//0==only 7bit from OAR1 is recognised 1==OAR1 & OAR2 7bit are recognised
#define ENDUAL I2C_OAR2_ENDUAL
//
//    (I2C_DR)  Data register
#define I2C_DR(REG,VAL) WRITE_REG(REG,VAL)// 8 Bit Register
//
//  (I2C_SR1)  Status register 1
#define TIMEOUT I2C_SR1_TIMEOUT // Timeout or Tlow error
#define PECERR ~(I2C_SR1_PECERR )  // PEC Error in reception
//
#define OVR I2C_SR1_OVR // Overrun/Underrun

//
void setI2cPins()
{
    GPIOB->CRL = (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1); // sda
    GPIOB->CRL = (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1); // clk
}

void i2c_init()
{
    // RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 = I2C_CR1_ACK;       // addr/data ack
    I2C1->CR1 = I2C_CR1_NOSTRETCH; // 0-clk strech enabled 1-disabled
    I2C1->CR1 = I2C_CR1_START;     // cleared by HW
    I2C1->CR1 = I2C_CR1_STOP;      // must clear BTF bit of the I2C_SR1
    I2C1->CR1 = I2C_CR1_PE;        // i2c enable
    I2C1->CR1 = I2C_CR1_SWRST;     // periph 1-under reset
    I2C1->CR1 = I2C_CR1_ENPEC;     // 0-PEC calc 0-disabled 1-enabled
    I2C1->CR1 = I2C_CR1_PEC;       // 0- no pkt error tx/rx 1-tx/rx error
    // I2C1->CR1 = I2C_CR1_POS;       // rx (N)ACK for PEC if txing 2 data bytes

    I2C1->CR2 &= ~I2C_CR2_ITBUFEN; // TxE=1 RxNE=1  1=intr enabled  0-intr disabled
    I2C1->CR2 = I2C_CR2_ITEVTEN;   // intr 1-enable 0-disable for (SB ,ADDR,ADDR10,BTF,TXE,RXNE) > master-mode
    I2C1->CR2 = I2C_CR2_ITERREN;   // 1 -Error intr enable for (BERR,ARLO,AF,OVR,PECERR,TIMEOUT);
    I2C1->CR2 = I2C_CR2_FREQ_4;    // 36MHZ/4mhz==9MHZ
    // setI2cPins();
}

// void i2c_Address(uint8_t addr, uint8_t mode, uint8_t dir)
// {
//     I2C1->OAR2 = (addr << 1);         // 7-bit addr
//     I2C1->SR2 = (I2C_SR2_TRA & mode); // 1=write 0=read (invalid if STOPF=1/ARLO=1 or PE=0 )
//     I2C1->SR2 = I2C_SR2_BUSY;         // 1-comm on bus
//     I2C1->SR2 = (I2C_SR2_MSL & dir);  // 0=slave 1=master
//                                       //    I2C1->CCR = (I2C_CCR_FS & foscMode);//0-std-mode 1-fast mode
//     I2C1->SR1 = I2C_SR1_OVR;          //* set by H/W IN slave mode NOSTRECH==1 reset by softw
//     I2C1->SR1 = I2C_SR1_AF;           //* 1-no ACK 0-ACK H/W set/reset or 0-by softw  (if PE=0 RESETS)
//     I2C1->SR1 = I2C_SR1_ARLO;         //* (master) arbitration lost 0-nope 1=arbitr detected clrd by setting =0
//     I2C1->SR1 = I2C_SR1_BERR;         //* bus error reset==0
//     I2C1->SR1 = I2C_SR1_TXE;          //* 0-not empty 1-empty
//     I2C1->SR1 = I2C_SR1_RXNE;         //* 0-empty 1-empty
//     I2C1->SR1 = I2C_SR1_BTF;          //* 0-no data txed 1-data txed **if NACK BTF is not SET
//     I2C1->SR1 = I2C_SR1_ADDR;         //* 1-slave addr resved acked
//     I2C1->SR1 = I2C_SR1_SB;           //* 1 start bit master mode
// }
// void i2c_addr(uint8_t addr)
// {
//     I2C1->SR2 = I2C_SR2_MSL;
//     I2C1->SR2 = I2C_SR2_TRA; // write
//     I2C1->SR1 = I2C_SR1_SB;
//     I2C1->OAR2 = (addr << 1);
//     while ((I2C1->CR1 & I2C_CR1_ACK) != I2C_CR1_ACK)
//         ;
// }

void i2c_Write(uint16_t addr)
{
    // while ((I2C1->SR1 & I2C_SR1_BERR) == I2C_SR1_BERR)
    //     ;
    // while ((I2C1->SR1 & I2C_SR1_ARLO) == I2C_SR1_ARLO)
    //     ;
    // while ((I2C1->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE)
    //     ;
    // I2C1->DR = addr; // txing TXE==1 rxing RXNE=1;
    // while ((I2C1->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF)
    //     ;
    // while ((I2C1->CR1 & I2C_CR1_ACK) != I2C_CR1_ACK)
    //     ;
}
void i2c_Stop()
{
    // while ((I2C1->SR1 & I2C_SR1_BTF) != I2C_SR1_BTF)
    //     ;
    // I2C1->CR1 = I2C_CR1_STOP;
}
#endif // __I2C
