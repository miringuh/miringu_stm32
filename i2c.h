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
/** \ingroup util_twi
    \def TW_START
    start condition transmitted */
#define TW_START 0x08

/** \ingroup util_twi
    \def TW_REP_START
    repeated start condition transmitted */
#define TW_REP_START 0x10

/* Master Transmitter */
/** \ingroup util_twi
    \def TW_MT_SLA_ACK
    SLA+W transmitted, ACK received */
#define TW_MT_SLA_ACK 0x18

/** \ingroup util_twi
    \def TW_MT_SLA_NACK
    SLA+W transmitted, NACK received */
#define TW_MT_SLA_NACK 0x20

/** \ingroup util_twi
    \def TW_MT_DATA_ACK
    data transmitted, ACK received */
#define TW_MT_DATA_ACK 0x28

/** \ingroup util_twi
    \def TW_MT_DATA_NACK
    data transmitted, NACK received */
#define TW_MT_DATA_NACK 0x30

/** \ingroup util_twi
    \def TW_MT_ARB_LOST
    arbitration lost in SLA+W or data */
#define TW_MT_ARB_LOST 0x38

/* Master Receiver */
/** \ingroup util_twi
    \def TW_MR_ARB_LOST
    arbitration lost in SLA+R or NACK */
#define TW_MR_ARB_LOST 0x38

/** \ingroup util_twi
    \def TW_MR_SLA_ACK
    SLA+R transmitted, ACK received */
#define TW_MR_SLA_ACK 0x40

/** \ingroup util_twi
    \def TW_MR_SLA_NACK
    SLA+R transmitted, NACK received */
#define TW_MR_SLA_NACK 0x48

/** \ingroup util_twi
    \def TW_MR_DATA_ACK
    data received, ACK returned */
#define TW_MR_DATA_ACK 0x50

/** \ingroup util_twi
    \def TW_MR_DATA_NACK
    data received, NACK returned */
#define TW_MR_DATA_NACK 0x58

/* Slave Transmitter */
/** \ingroup util_twi
    \def TW_ST_SLA_ACK
    SLA+R received, ACK returned */
#define TW_ST_SLA_ACK 0xA8

/** \ingroup util_twi
    \def TW_ST_ARB_LOST_SLA_ACK
    arbitration lost in SLA+RW, SLA+R received, ACK returned */
#define TW_ST_ARB_LOST_SLA_ACK 0xB0

/** \ingroup util_twi
    \def TW_ST_DATA_ACK
    data transmitted, ACK received */
#define TW_ST_DATA_ACK 0xB8

/** \ingroup util_twi
    \def TW_ST_DATA_NACK
    data transmitted, NACK received */
#define TW_ST_DATA_NACK 0xC0

/** \ingroup util_twi
    \def TW_ST_LAST_DATA
    last data byte transmitted, ACK received */
#define TW_ST_LAST_DATA 0xC8

/* Slave Receiver */
/** \ingroup util_twi
    \def TW_SR_SLA_ACK
    SLA+W received, ACK returned */
#define TW_SR_SLA_ACK 0x60

/** \ingroup util_twi
    \def TW_SR_ARB_LOST_SLA_ACK
    arbitration lost in SLA+RW, SLA+W received, ACK returned */
#define TW_SR_ARB_LOST_SLA_ACK 0x68

/** \ingroup util_twi
    \def TW_SR_GCALL_ACK
    general call received, ACK returned */
#define TW_SR_GCALL_ACK 0x70

/** \ingroup util_twi
    \def TW_SR_ARB_LOST_GCALL_ACK
    arbitration lost in SLA+RW, general call received, ACK returned */
#define TW_SR_ARB_LOST_GCALL_ACK 0x78

/** \ingroup util_twi
    \def TW_SR_DATA_ACK
    data received, ACK returned */
#define TW_SR_DATA_ACK 0x80

/** \ingroup util_twi
    \def TW_SR_DATA_NACK
    data received, NACK returned */
#define TW_SR_DATA_NACK 0x88

/** \ingroup util_twi
    \def TW_SR_GCALL_DATA_ACK
    general call data received, ACK returned */
#define TW_SR_GCALL_DATA_ACK 0x90

/** \ingroup util_twi
    \def TW_SR_GCALL_DATA_NACK
    general call data received, NACK returned */
#define TW_SR_GCALL_DATA_NACK 0x98

/** \ingroup util_twi
    \def TW_SR_STOP
    stop or repeated start condition received while selected */
#define TW_SR_STOP 0xA0

/* Misc */
/** \ingroup util_twi
    \def TW_NO_INFO
    no state information available */
#define TW_NO_INFO 0xF8

/** \ingroup util_twi
    \def TW_BUS_ERROR
    illegal start or stop condition */
#define TW_BUS_ERROR 0x00

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
volatile uint8_t i2cdummy;
volatile uint8_t i2cVal;
#define SLA_W (0X4E)
#define SLA_R (0X4F)
void i2c1_stop(void);
void I2C1_EV_IRQHandler()
{
    if ((I2C1->SR1 & I2C_SR1_TXE))
    {
        i2cdummy = I2C1->DR;
    }
}
void I2C1_ER_IRQHandler()
{
    // if ((I2C1->SR1 & TIMEOUT_Flag))
    // {
    //     I2C1->SR1 &= ~TIMEOUT_Flag;
    // }
    // if ((I2C1->SR1 & AF_Flag))
    // {
    //     I2C1->SR1 &= ~AF_Flag;
    // }
    // if ((I2C1->SR1 & ARLO_Flag))
    // {
    //     I2C1->SR1 &= ~ARLO_Flag;
    // }
    // if ((I2C1->SR1 & BERR_Flag))
    // {
    //     I2C1->SR1 &= ~BERR_Flag;
    // }
    eusart_send(0x22);
}

void setI2cPins_mapr1() // 1==scl-PB8 sda-pb9
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    GPIOB->CRH = (GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1); // sda pb9
    GPIOB->CRH = (GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1); // clk pb8
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
    // AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG_Msk);
}
void setI2c1Pins_mapr0() // 0==scl-PB6 sda-pb7
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    _delay_ms(10000);
    GPIOB->CRL = (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1); // sda pb7
    GPIOB->CRL = (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1); // clk pb6
    AFIO->MAPR &= ~(AFIO_MAPR_I2C1_REMAP);
}
/*
CCR = (APB1 clock) / (2 * SCL clock) = 8MHz / (2 * 100kHz) = 40
I2C1_TRISE = (APB1 clock / 1000000) + 1 = 8 + 1 = 9
*/
//
void i2cStart()
{
    I2C1->CR1 |= START;

    // eusart_send(0x01);
    _delay_ms(100000);

    while (!(I2C1->CR1 & I2C_CR1_START)) //
        ;
    // eusart_send(0x02);
    while ((I2C1->SR1 & I2C_SR1_BERR)) //
        ;
    // eusart_send(0x03);
    i2cdummy = I2C1->SR1;
    i2cdummy = I2C1->SR2;
    i2cdummy = I2C1->CR1;
    i2cdummy = I2C1->CR2;
}
void i2c1_init()
{
    setI2c1Pins_mapr0();
    I2C1->CR1 = 0;
    I2C1->CR2 = 0;
    // I2C1->CR1 &= ~I2C_CR1_SMBUS;
    _delay_ms(100000);
    FREQ(I2C1->CR2, (2 << I2C_CR2_FREQ_Pos));
    // I2C1->OAR2 &= ~(ENDUAL);
    // I2C1->CCR &= ~FS;
    CCR(I2C1->CCR, 100);
    TRISE(I2C1->TRISE, 200);

    // I2C1->CR2 |= (ITBUFEN); // | // 1:TxE/RxNE gen. Event Interrupt
    // I2C1->CR2 |= ITEVTEN; // BTF, TxE/RXNE events==1 if ITBUFEN == 1
    // I2C1->CR2 |= ITERREN;   // BERR ARLO AF OVR PECERR TIMEOUT == 1

    // NVIC_SetPriority(I2C1_ER_IRQn, 2);
    // NVIC_SetPriority(I2C1_EV_IRQn, 2);
    // NVIC_EnableIRQ(I2C1_EV_IRQn);
    // NVIC_EnableIRQ(I2C1_ER_IRQn);

    I2C1->CR1 |= I2C_CR1_PE;
    // _delay_ms(10000);
    i2cStart();
}
void i2c1_send_address(uint8_t address)
{
    // Send address
    i2cdummy = I2C1->SR1;
    i2cdummy = I2C1->SR2;
    i2cdummy = I2C1->DR;
    I2C1->DR = address;
    
    while (!(I2C1->SR1 & I2C_SR1_AF)) // 1-ack 0-nack
    {
        goto ends;
    }

    while (!(I2C1->SR1 & I2C_SR1_ADDR)) // addr 1-txed 0-no tx
    {
        goto ends;
    }
    while (!(I2C1->CR1 & I2C_CR1_ACK)) // 1-ack 0-nack
    {
        goto ends;
    }
    eusart_send(0x05);
    while (!(I2C1->SR2 & I2C_SR2_TRA)) // addr 1-txed 0-rxed
        ;
    eusart_send(0x06);

    while (!(I2C1->SR2 & I2C_SR2_MSL)) // 0-slv 1=mst
        ;
    eusart_send(0x07);

    i2cdummy = I2C1->SR1;
    i2cdummy = I2C1->SR2;
    eusart_send(0x08);

    eusart_send(I2C1->DR);
ends:
    eusart_send(I2C1->DR);
}

void i2c1_write(uint8_t data)
{
    /*
    SR1>> BTF, !AF, !ARLO, !BERR,
    SR2>> TRA* BUSY
    */
    I2C1->DR = data;
    while ((I2C1->SR1 & I2C_SR1_TXE))
        ;
    eusart_send(0x06);

    while (!(I2C1->SR1 & I2C_SR1_BTF)) // data not txed
        ;
    eusart_send(0x07);

    // eusart_send(I2C1->DR);
}

uint8_t i2c1_get_address(uint8_t address)
{
    I2C1->CR1 |= I2C_CR1_PE;
    i2cStart();
    I2C1->DR = address;
    // _delay_ms(1000);
    if ((I2C1->SR1 & I2C_SR1_ADDR)) // ack
    {
        // return I2C1->DR;
        eusart_send(0X00);
        return address;
    }
    if (!(I2C1->SR1 & I2C_SR1_ADDR)) // ack
    {
        goto ends;
    }
ends:
    eusart_send(address);
    i2c1_stop();
    return 0;
}
void i2c_read_init()
{
    i2cStart();
    i2c1_send_address(SLA_R);
}
void i2c1_stop(void)
{
    I2C1->SR1 &= ~I2C_SR1_BTF;
    while (I2C1->SR1 & I2C_SR1_BTF)
        ;

    I2C1->CR1 |= I2C_CR1_STOP;
    while (!(I2C1->CR1 & I2C_CR1_STOP))
        ;
}
#endif
/*
#include "i2c1.h"
#include <stdint.h>

// STM32F103 memory map
#define RCC_BASE        0x40021000
#define GPIOB_BASE      0x40010C00
#define I2C1_BASE       0x40005400

// RCC registers
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// GPIOB registers
#define GPIOB_CRL       (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_CRH       (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))
#define GPIOB_BSRR      (*(volatile uint32_t *)(GPIOB_BASE + 0x10))

// I2C1 registers
#define I2C1->CR1        (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2        (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_OAR1       (*(volatile uint32_t *)(I2C1_BASE + 0x08))
#define I2C1->DR         (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1->SR1        (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1->SR2        (*(volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C1_CCR        (*(volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE      (*(volatile uint32_t *)(I2C1_BASE + 0x20))

// Clock enables
#define RCC_APB2ENR_IOPBEN     (1 << 3)  // GPIOB clock enable
#define RCC_APB1ENR_I2C1EN     (1 << 21) // I2C1 clock enable

// I2C CR1 bits
#define I2C_CR1_PE          (1 << 0)  // Peripheral enable
#define I2C_CR1_START       (1 << 8)  // Generate start condition
#define I2C_CR1_STOP        (1 << 9)  // Generate stop condition
#define I2C_CR1_ACK         (1 << 10) // Acknowledge enable
#define I2C_CR1_POS         (1 << 11) // Acknowledge position

// I2C CR2 bits
#define I2C_CR2_FREQ_MASK   0x3F

// Simple delay function
static void delay(volatile uint32_t count) {
    while (count--) {
        __asm__("nop");
    }
}

void i2c1_init(void) {
    // Enable clocks for GPIOB and I2C1
    RCC_APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;

    delay(1000); // Wait for clocks to stabilize

    // Configure PB6 (SCL) and PB7 (SDA) as alternate function open-drain
    // PB6 and PB7 are in CRL register (bits 24-31)
    GPIOB_CRL &= ~(0xFF << 24); // Clear PB6 and PB7 configuration
    GPIOB_CRL |= (0x3 << 26) | (0x3 << 30); // PB6 and PB7 as 10MHz open-drain alternate function

    // Reset I2C1
    I2C1->CR1 = 0;

    // Configure I2C timing for 100kHz (Standard mode)
    // Assuming 8MHz system clock (typical for Blue Pill)
    uint32_t freq = 8; // 8MHz APB1 clock
    I2C1_CR2 = freq & I2C_CR2_FREQ_MASK;

    // Configure clock control register for 100kHz
    // CCR = (APB1 clock) / (2 * SCL clock) = 8MHz / (2 * 100kHz) = 40
    I2C1_CCR = 40; // Standard mode (FM/SM bit = 0)

    // Configure rise time register
    // Trise = (APB1 clock / 1000000) + 1 = 8 + 1 = 9
    I2C1_TRISE = 9;

    // Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;

    delay(1000); // Wait for I2C to stabilize
}

void i2c1_start(void) {
    // Clear any pending start flag by reading SR1 and writing to SR2
    if (I2C1->SR1 & I2C_SR1_SB) {
        uint32_t temp = I2C1->SR1;
        temp = I2C1->SR2; // Clear ADDR if set
        (void)temp;
    }

    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START;

    // Wait for start bit to be generated
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c1_stop(void) {
    // Generate stop condition
    I2C1->CR1 |= I2C_CR1_STOP;

    // Wait for stop to be cleared (bus free)
    while (I2C1->CR1 & I2C_CR1_STOP);
}

void i2c1_send_address(uint8_t address, uint8_t read) {
    uint8_t addr_byte = (address << 1) | (read ? 1 : 0);

    // Send address
    I2C1->DR = addr_byte;

    // Wait for address to be sent
    while (!(I2C1->SR1 & I2C_SR1_ADDR));

    // Clear ADDR flag by reading SR2
    uint32_t temp = I2C1->SR2;
    (void)temp;
}

void i2c1_write(uint8_t data) {
    // Wait for TXE flag (transmit buffer empty)
    while (!(I2C1->SR1 & I2C_SR1_TXE));

    // Send data
    I2C1->DR = data;

    // Wait for transfer to complete
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

uint8_t i2c1_read_ack(void) {
    // Enable ACK for next byte
    I2C1->CR1 |= I2C_CR1_ACK;

    // Wait for RXNE flag (data received)
    while (!(I2C1->SR1 & I2C_SR1_RXNE));

    // Read data
    return I2C1->DR;
}

uint8_t i2c1_read_nack(void) {
    // Disable ACK for last byte
    I2C1->CR1 &= ~I2C_CR1_ACK;

    // Wait for RXNE flag (data received)
    while (!(I2C1->SR1 & I2C_SR1_RXNE));

    // Read data
    return I2C1->DR;
}

uint8_t i2c1_is_ready(uint8_t address) {
    uint8_t ready = 0;

    // Generate start condition
    i2c1_start();

    // Send address for write
    i2c1_send_address(address, 0);

    // Check for acknowledge failure
    if (!(I2C1->SR1 & I2C_SR1_AF)) {
        ready = 1;
    }

    // Generate stop condition
    i2c1_stop();

    return ready;
}


*/
