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
// Software Reset
#define SWRST I2C_CR1_SWRST
// Packet Error Checking.
#define PEC I2C_CR1_PEC
// Acknowledge/PEC Position (for data reception)
#define POS I2C_CR1_PEC
#define ACK I2C_CR1_ACK // Acknowledge enable
// In Master mode, the BTF bit of the I2C_SR1 register must be cleared
#define STOP I2C_CR1_STOP
// Start Generation
#define START I2C_CR1_START
// Clock Stretching Disable (Slave mode)
#define NOSTRETCH ~(I2C_CR1_NOSTRETCH)
// General Call Enable
#define ENGC I2C_CR1_ENGC // general call enable
// pec enable
#define ENPEC I2C_CR1_ENPEC
#define I2CPEN I2C_CR1_PE // peripheral enable

//   (I2C_CR2)   Control register 2
/*
0: TxE = 1 or RxNE = 1 does not generate any interrupt.
1:TxE = 1 or RxNE = 1 generates Event Interrupt (whatever the state of
DMAEN)
*/
#define ITBUFEN I2C_CR2_ITBUFEN // Buffer interrupt enable
/*
– SB = 1 (Master)
– ADDR = 1 (Master/Slave)
– ADD10= 1 (Master)
– STOPF = 1 (Slave)
– BTF = 1 with no TxE or RxNE event
– TxE event to 1 if ITBUFEN = 1
– RxNE event to 1if ITBUFEN = 1
*/
#define ITEVTEN I2C_CR2_ITEVTEN // Event interrupt enable
/*
– BERR = 1
– ARLO = 1
– AF = 1
– OVR = 1
– PECERR = 1
– TIMEOUT = 1
– SMBAlert = 1
*/
#define ITERREN I2C_CR2_ITERREN // Error interrupt enable
/*
The minimum allowed frequency is 2 MHz,
the maximum frequency is limited by the maximum APB frequency 50 MHz
*/
#define FREQ(REG, VAL) WRITE_REG(REG, VAL) //[5:0] 2=2MHZ......50MHZ=0X32 or 50
//
//    (I2C_OAR1)  Own address register 1
// Addressing mode(slave mode)
#define ADDMODE I2C_OAR1_ADDMODE              // 0==7-bit slave 1==10bit_slave
#define ADD10_H(REG, VAL) WRITE_REG(REG, VAL) //[9:8] 10-bit addr
#define ADD7_L(REG, VAL) WRITE_REG(REG, VAL)  //[7:1] 7-bit addr <<1
// 1==10-bit addressing mode: bit 0 of address
#define ADD10_0 I2C_OAR1_ADD0
//
//    (I2C_OAR2)  Own address register 2
//[7:1] bits 7:1 of address in dual addressing mode
#define ADD2(REG, VAL) WRITE_REG(REG, VAL) //
// 0==only 7bit from OAR1 is recognised 1==OAR1 & OAR2 7bit are recognised
#define ENDUAL I2C_OAR2_ENDUAL
//
//    (I2C_DR)  Data register
#define I2C_DR(REG, VAL) WRITE_REG(REG, VAL) // 8 Bit Register
//
//    (I2C_SR1)  Status register 1
#define TIMEOUT_Flag I2C_SR1_TIMEOUT  // Timeout or Tlow error
#define PECERR_Flag ~(I2C_SR1_PECERR) // PEC Error in reception
#define OVR_Flag I2C_SR1_OVR          // Overrun/Underrun
// Acknowledge Failure. 1-NACK 0-ACK
#define AF_Flag ~(I2C_SR1_AF)
/* Arbitration Lost (master mode) 1=error 0-no arbitration After an ARLO event the interface switches back automatically to Slave mode
 */
#define ARLO_Flag I2C_SR1_ARLO
// Set by hardware when the interface detects a misplaced Start or Stop condition
#define BERR_Flag I2C_SR1_BERR // BUS ERROR
/*
0-data full 1-empty reg
TxE is not set if either a NACK is received, or if next byte to be transmitted is PEC (PEC=1)
*/
#define TxE_Flag I2C_SR1_TXE
/*
0-Empty 1-full
Cleared by software reading or writing the DR register or by hardware when PE=0.
*/
#define RxNE_Flag I2C_SR1_RXNE
/*
10-bit header sent (Master mode)
Set by hardware when the master has sent the first byte in 10-bit address mode.
Cleared by software reading the SR1 register followed by a write in the DR register of
*/
#define ADD10_Flag I2C_SR1_ADD10
/*
0=Not txed 1=not txed
In transmission when a new byte should be sent and DR has not been written yet (TxE=1)
*/
#define BTF_Flag I2C_SR1_BTF
// Start Bit (Master mode). 1=Start is generated
#define SB_Flag I2C_SR1_SB
//
//   (I2C_SR1)  Status register 1
// Packet Error Checking Register [7:0] when ENPEC=1.
#define PEC_Flag I2C_SR1_SB//7:0 
//0==RXED  1==TXED
#define TRA_Flag I2C_SR1_SB // Transmitter/Receiver
// 0==Free  1==busy
#define BUSY_Flag I2C_SR1_BUSY 
//0==SLAVE 1==MASTER
#define MSL_FLAG I2C_SR1_MSL//PERIPH MODE
//
//   (I2C_CCR)  Clock control register
//0==std-i2c-mode 1==fast i2c-mode
#define F_SModes I2C_CCR_F //

//
/*
rcc->apb1enr I2C2EN
rcc->apb1enr I2C1EN
rcc->apb2enr AFIOEN
//
    AFIO_MAPR
I2C1_REMAP 0==scl-PB6 sda-pb7
I2C1_REMAP 1==scl-PB8 sda-pb9
//
i2c1-sda-b9
i2c1-scl-b8
//
i2c1-sda-b7
i2c1-scl-b6
//
i2c2-sda-b11
i2c2-scl-b10
*/
void setI2c2Pins() // 1==scl-PB8 sda-pb9
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB1ENR_AFIOEN;
    GPIOB->CRH = (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1); // sda pb9
    GPIOB->CRH = (GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1); // clk pb8
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG_MSK);
}
void setI2c2Pins() // 0==scl-PB6 sda-pb7
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB1ENR_AFIOEN;
    GPIOB->CRL = (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1); // sda pb7
    GPIOB->CRL = (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1); // clk pb6
    AFIO->MAPR &= ~(AFIO_MAPR_I2C1_REMAP | AFIO_MAPR_SWJ_CFG_MSK);
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
