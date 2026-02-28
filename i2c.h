#if !defined(__I2C)
#define __I2C
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "gpio.h"
#include "eusart.h"
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
/*@{*/
/* Master */

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
– RxNE event to 1 if ITBUFEN = 1
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
#define TIMEOUT_Flag I2C_SR1_TIMEOUT // Timeout or Tlow error
// PEC Error in reception
#define PECERR_Flag I2C_SR1_PECERR
// Overrun/Underrun
#define OVR_Flag I2C_SR1_OVR
// Acknowledge Failure. 1-NACK 0-ACK
#define AF_Flag I2C_SR1_AF
/*
Arbitration Lost (master mode) 1=error 0-no arbitration After an ARLO event the interface switches back automatically to Slave mode
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
//   (I2C_SR2)  Status register 2
// Packet Error Checking Register [7:0] when ENPEC=1.
#define PEC_Flag I2C_SR1_SB // 7:0
// 0==RXED  1==TXED
#define TRA_Flag I2C_SR1_SB // Transmitter/Receiver
// 0==Free  1==busy
#define BUSY_Flag I2C_SR2_BUSY
// 0==SLAVE 1==MASTER
#define MSL_FLAG I2C_SR2_MSL // PERIPH MODE
//
//   (I2C_CCR)  Clock control register
// 0==std-i2c-mode 1==fast i2c-mode
#define FS I2C_CCR_FS //
// Fast Mode Duty Cycle
#define DUTY I2C_CCR_DUTY
// Clock Control Register in Fast/Standard mode (Master mode)[11:0]
#define CCR(REG, VAL) WRITE_REG(REG, (VAL << I2C_CCR_CCR_Pos)) // 100KHZ 28h
//
//     I2C1->OAR1
#define ADD7(REG, VAL) WRITE_REG(REG, VAL)
//  I2C1->TRISE
#define TRISE(REG, VAL) WRITE_REG(REG, VAL)
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
volatile uint32_t i2cdummy;
volatile uint32_t i2cVal;
volatile uint32_t i2cdata;
uint8_t on = 1;
uint8_t off = 0;
uint8_t readi2c = 0;
#define SLA_W 0X4E
#define SLA_R 0X4F
void i2c1_stop(void);
////// 10mhz //////////////
void setI2c1Pins_mapr1() // 1==scl-PB8 sda-pb9
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    GPIOB->CRH |= (GPIO_CRH_CNF9_Msk | GPIO_CRH_MODE9_0); // sda pb9
    GPIOB->CRH |= (GPIO_CRH_CNF8_Msk | GPIO_CRH_MODE8_0); // clk pb8
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG_Msk);
}
void setI2c1Pins_mapr0() // 0==scl-PB6 sda-pb7
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
    GPIOB->CRL = (GPIO_CRL_CNF7_Msk | GPIO_CRL_MODE7_0);  // sda pb7
    GPIOB->CRL |= (GPIO_CRL_CNF6_Msk | GPIO_CRL_MODE6_0); // clk pb6
    GPIOC->CRH = (P_P10MHZ << GPIO_CRH_CNF13_Pos);
    AFIO->MAPR &= ~(AFIO_MAPR_I2C1_REMAP | AFIO_MAPR_SWJ_CFG_Msk);
}
/*
CCR = (APB1 clock) / (2 * SCL clock) = 8MHz / (2 * 100kHz) = 40
I2C1_TRISE = (APB1 clock / 1000000) + 1 = 8 + 1 = 9
*/

void I2C1_EV_IRQHandler()
{

    eusart_send((uint8_t)(I2C1->SR1) >> 8);
    eusart_send((uint8_t)(I2C1->SR1));
    if ((I2C1->SR1 & I2C_SR1_SB))
    {
        i2cdummy = I2C1->SR1;
    }
    if ((I2C1->SR1 & I2C_SR1_ADDR))
    {
        i2cdummy = I2C1->SR1;
        i2cdummy = I2C1->SR2;
    }
    // if ((I2C1->SR1 & I2C_SR1_BTF))
    // {
    //     I2C1->DR = (uint32_t)&i2cdata;
    // }
    // if ((I2C1->SR1 & I2C_SR1_RXNE))
    // {
    //     i2cVal = I2C1->DR;
    // }
    // if ((I2C1->SR1 & I2C_SR1_TXE))
    // {
    //     I2C1->DR = (uint32_t)&i2cdata;
    // }
    // if ((I2C1->SR1 & I2C_SR1_STOPF))
    // {
    //     i2cdummy = I2C1->SR1;
    // }
}
void I2C1_ER_IRQHandler()
{

    if ((I2C1->SR1 & TIMEOUT_Flag))
    {
        I2C1->SR1 &= ~TIMEOUT_Flag;
    }
    if ((I2C1->SR1 & AF_Flag))
    {
        I2C1->SR1 &= ~AF_Flag;
    }
    if ((I2C1->SR1 & ARLO_Flag))
    {
        I2C1->SR1 &= ~ARLO_Flag;
    }
    if ((I2C1->SR1 & BERR_Flag))
    {
        I2C1->SR1 &= ~BERR_Flag;
    }
    if ((I2C1->SR1 & OVR_Flag))
    {
        I2C1->SR1 &= ~OVR_Flag;
    }
    // eusart_send((uint8_t)(I2C1->SR1) >> 8);
    // eusart_send((uint8_t)(I2C1->SR1));
}
//
void i2c_chipSel(uint8_t state)
{
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    }
    if (!(GPIOC->CRH & GPIO_CRH_MODE13_1))
    {
        GPIOC->CRH = GPIO_CRH_MODE13_1; // 2MHZ P_P
    }
    if (state == on)
    {
        GPIOC->ODR = GPIO_ODR_ODR13;
    }
    if (state == off)
    {
        GPIOC->ODR &= ~GPIO_ODR_ODR13;
    }
    _delay_ms(1000000);
}

void i2c1_init() // scl-PB6 sda-pb7
{
    i2c_chipSel(off);
    i2c_chipSel(on);

    setI2c1Pins_mapr0();
    I2C1->CR1 = 0;
    I2C1->CR2 = 0;
    I2C1->SR1 = 0;
    I2C1->SR2 = 0;
    I2C1->CCR = 0;
    // Trise = (APB1 clock / 1000000) + 1
    I2C1->CR2 = (I2C_CR2_FREQ_Msk & 20);
    I2C1->CR1 &= ~(I2C_CR1_ENPEC) | (I2C_CR1_ENARP);
    // CCR = (APB1 clock) / (2 * SCL clock)
    I2C1->CCR = I2C_CCR_CCR_Msk & 100;
    I2C1->CCR &= ~(I2C_CCR_DUTY | I2C_CCR_FS);
    I2C1->TRISE = (I2C_TRISE_TRISE_Msk & 21);

    // I2C1->CR2 |= ITEVTEN; // SB ADDR ADDR10,STOPF BTF
    // I2C1->CR2 |= ITBUFEN; // ITEVFEN + TxE RxNE
    // I2C1->CR2 |= ITERREN; // BERR ARLO AF OVR PECERR TIMEOUT SMBALERT

    // NVIC_SetPriority(I2C1_EV_IRQn, 2);
    // NVIC_EnableIRQ(I2C1_EV_IRQn);
    // NVIC_SetPriority(I2C1_ER_IRQn, 3);
    // NVIC_EnableIRQ(I2C1_ER_IRQn);
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_PE;

    while ((I2C1->SR2 & I2C_SR2_BUSY))
    {
        // i2c_chipSel(off);
        // i2c_chipSel(on);
        I2C1->CR1 = I2C_CR1_SWRST;
        _delay_ms(200000);
        I2C1->CR1 &= ~I2C_CR1_SWRST;

        // Trise = (APB1 clock / 1000000) + 1
        I2C1->CR2 = (I2C_CR2_FREQ_Msk & 20);
        I2C1->CR1 &= ~(I2C_CR1_ENPEC) | (I2C_CR1_ENARP);
        // CCR = (APB1 clock) / (2 * SCL clock)
        I2C1->CCR = I2C_CCR_CCR_Msk & 100;
        I2C1->CCR &= ~(I2C_CCR_DUTY | I2C_CCR_FS);
        I2C1->TRISE = (I2C_TRISE_TRISE_Msk & 21);

        // I2C1->CR2 |= ITEVTEN; // SB ADDR ADDR10,STOPF BTF
        // I2C1->CR2 |= ITBUFEN; // ITEVFEN + TxE RxNE
        // I2C1->CR2 |= ITERREN; // BERR ARLO AF OVR PECERR TIMEOUT SMBALERT

        // NVIC_SetPriority(I2C1_EV_IRQn, 2);
        // NVIC_EnableIRQ(I2C1_EV_IRQn);
        // NVIC_SetPriority(I2C1_ER_IRQn, 3);
        // NVIC_EnableIRQ(I2C1_ER_IRQn);
        I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE;
    }
}
void i2cStart()
{

    if ((I2C1->SR1 & I2C_SR1_SB)) // 0-nostrt 1-strt
    {
        i2cdummy = I2C1->SR1;
        I2C1->DR = 0;
    }

    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)) // 0-nostrt 1-strt
        ;
    i2cdummy = I2C1->SR2;
    i2cdummy = I2C1->SR1;
}
void i2c1_send_address(uint8_t address)
{
    // i2cStart();
    I2C1->DR = address;
    if ((address == SLA_R) | (address == 0))
    {
        while (!(I2C1->SR1 & I2C_SR1_ADDR)) // addr 0-not txed 1-txed
            ;
        i2cdummy = I2C1->SR2;
        readi2c = 0;
    }
    if (address == SLA_W)
    {
        while (!(I2C1->SR1 & I2C_SR1_ADDR)) // addr 0-not txed 1-txed
            ;
        i2cdummy = I2C1->SR2;
        readi2c = 1;
    }
}
void i2c1_write(uint8_t data)
{
    /*
    TXE----BTF
    TRA--BUSY--MSL
    */
    // i2cStart();
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_TXE)) // 0-full 1-empty
        ;
    while (!(I2C1->SR1 & I2C_SR1_BTF)) // 0-not Txed 1-txed
        ;
    while (!(I2C1->SR2 & I2C_SR2_TRA)) // 0-not txed 1-txed
        ;
    while (!(I2C1->SR2 & I2C_SR2_BUSY)) // 0-free 1-busy
        ;
    while (!(I2C1->SR2 & I2C_SR2_MSL)) // 0-slv 1-master
        ;
    if ((I2C1->SR1 & I2C_SR1_RXNE)) // 0-Empty 1-full
    {
        i2cVal = I2C1->DR;
    }
    if (!(I2C1->SR1 & I2C_SR1_RXNE)) // 0-Empty 1-full
    {
        i2cVal = I2C1->DR;
    }
    // eusart_send(i2cVal);
}
void i2c1_stop(void)
{
    // Generate stop condition
    // I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_STOP;

    // Wait for stop to be cleared (bus free)
    while (!(I2C1->CR1 & I2C_CR1_STOP))
        ;
    eusart_send(0xee);
    // i2c_chipSel(off);
}

/////////////////
/////// DMA /////////
/* DMA I2-TX OR I2-RX
I2C_1- TX=channel 6
I2C_1- RX=channel 7

I2C_2- TX=channel 4
I2C_2- RX=channel 5
*/
char i2c_buff[255];
char rd_buff[255];
volatile uint8_t i2c_state;

void start();
void sendAddr(uint8_t address);

void DMA1_Channel6_IRQHandler()
{
    if ((DMA1->ISR & DMA_ISR_HTIF6)) // half txed
    {
        DMA1->IFCR |= DMA_IFCR_CHTIF6;
    }
    if ((DMA1->ISR & DMA_ISR_TCIF6)) // tx complete
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF6;
    }
    if ((DMA1->ISR & DMA_ISR_TEIF6)) // tx error
    {
        DMA1->IFCR |= DMA_IFCR_CTEIF6;
    }
    // DMA1_Channel6->CCR &= ~DMAEN;
}
void DMA1_Channel7_IRQHandler()
{
    if ((DMA1->ISR & DMA_ISR_TCIF7)) // tx complete
    {
        i2c_state = 1;
        DMA1->IFCR |= DMA_IFCR_CTCIF7;
    }
}
/////////////////////
void dma_i2cTx_init(char *msg, uint16_t size)
{
    i2c_chipSel(off);
    i2c_chipSel(on);
    strcpy(i2c_buff, msg);
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // setI2c1Pins_mapr0();

    // I2C1->CR1 = 0;
    // I2C1->CR2 = 0;
    // // Trise = (APB1 clock / 1000000) + 1
    // I2C1->CR2 = (I2C_CR2_FREQ_Msk & 20);
    // I2C1->CR1 &= ~(I2C_CR1_ENPEC) | (I2C_CR1_ENARP);
    // // CCR = (APB1 clock) / (2 * SCL clock)
    // I2C1->CCR |= I2C_CCR_CCR_Msk & 100;
    // I2C1->CCR &= ~(I2C_CCR_DUTY | I2C_CCR_FS);
    // I2C1->TRISE = (I2C_TRISE_TRISE_Msk & 21);

    DMA1_Channel6->CPAR = (uint32_t)&I2C1->DR;
    DMA1_Channel6->CMAR = (uint32_t)buff;
    DMA1_Channel6->CNDTR = size;
    DMA1_Channel6->CCR |= CIRC;  // 1-circ
    DMA1_Channel6->CCR |= MINC;  // mem incr
    DMA1_Channel6->CCR &= ~PINC; // periph no incr
    DMA1_Channel6->CCR |= DIR;

    DMA1_Channel6->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // peri/mem size
    DMA1_Channel6->CCR |= DMA_CCR_PL_0;                             // high prioty

    DMA1_Channel6->CCR |= TCIEN | TEIEN | HTIEN;
    NVIC_SetPriority(DMA1_Channel6_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    /*
 The DMAEN bit must be set only after receiving the address sequence, when ADDR is cleared
 */
    // I2C1->CR2 |= I2C_CR2_DMAEN; // I2C1 Tx channel=6
    // I2C1->CR1 |= I2C_CR1_PE;

    start();
    sendAddr(SLA_W);
    I2C1->CR2 |= I2C_CR2_DMAEN; // I2C1 Tx channel=6
}
void start()
{
    i2cStart();
}
void sendAddr(uint8_t address)
{
    i2c1_send_address(address);
}
////////
void dma_i2cRx_init(uint16_t size)
{

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    setI2c1Pins_mapr0();
    I2C1->CR1 = 0;
    I2C1->CR2 = 0;
    i2c_chipSel(off);
    i2c_chipSel(on);
    // Trise = (APB1 clock / 1000000) + 1
    I2C1->CR2 = (I2C_CR2_FREQ_Msk & 20);
    I2C1->CR1 &= ~(I2C_CR1_ENPEC) | (I2C_CR1_ENARP);
    // CCR = (APB1 clock) / (2 * SCL clock)
    I2C1->CCR |= I2C_CCR_CCR_Msk & 100;
    I2C1->CCR &= ~(I2C_CCR_DUTY | I2C_CCR_FS);
    I2C1->TRISE = (I2C_TRISE_TRISE_Msk & 21);

    DMA1_Channel7->CPAR = (uint32_t)&I2C1->DR;
    DMA1_Channel7->CMAR = (uint32_t)rd_buff;
    DMA1_Channel7->CNDTR = size;
    DMA1_Channel7->CCR |= CIRC;  // 1-circ
    DMA1_Channel7->CCR |= MINC;  // mem incr
    DMA1_Channel7->CCR &= ~PINC; // periph no incr
    DMA1_Channel7->CCR |= DIR;

    DMA1_Channel7->CCR &= ~(DMA_CCR_MSIZE_Msk | DMA_CCR_PSIZE_Msk); // peri/mem size
    DMA1_Channel7->CCR |= DMA_CCR_PL_0;                             // high prioty
    i2c_state = 0;
    DMA1_Channel7->CCR |= TCIEN; // | TEIEN | HTIEN;
    NVIC_SetPriority(DMA1_Channel7_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

#endif
