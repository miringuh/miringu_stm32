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

In 10-bit addressing mode, (The TRA bit indicates  master/Receiver or Transmitter mode.)
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
void setI2cPins()
{
    GPIOB->CRL = (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1); // sda
    GPIOB->CRL = (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1); // clk
}
/*

*/
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
