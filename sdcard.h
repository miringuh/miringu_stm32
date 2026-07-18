#if !defined(_SD_CARD)
#define _SD_CARD
#include "/home/jeff/STM32Cube_FW_F1_V1.8.0/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include "rcc_conf.h"
#include "spi.h"
#include "eusart.h"
#include "tim1.h"
#include "dma.h"
// #include "registers.h"
///////////
// sdsc ccs=0 byte unit address SDHC/XC use block unit address
#define CMD0 (0X00 | 0X40) // R1 go_idle stuff args (0x01)
// #define CMD1 (0X01|0X40)  // R1 1.4mm card only (0)
#define CMD6 (0X06 | 0X40)  // R1 switch func
#define CMD8 (0X08 | 0X40)  // R7 send_if_cond (OCR)4-bytes arg [11.8] host volt
#define CMD9 (0X09 | 0X40)  // R1 send_csd stuff arg**16bytes
#define CMD10 (0X0A | 0X40) // R1 send_cid stuff arg**16bytes
#define CMD12 (0X0C | 0X40) // r1b stop_tx stuff arg
#define CMD13 (0X0D | 0X40) // R2 send_status stuff arg
#define CMD16 (0X10 | 0X40) // r1 set_blk_len args==blk len
#define CMD17 (0X11 | 0X40) // r1 rd_single_blk args==address
#define CMD18 (0X12 | 0X40) // r1 rd_multi_blks args==address
#define CMD24 (0X18 | 0X40) // r1 wr_blk args==address
#define CMD25 (0X19 | 0X40) // r1 write multi_blk args==address
#define CMD27 (0X1B | 0X40) // r1 program csd r/w bit arg stuff bits
#define CMD28 (0X1C | 0X40) // r1b set_wr_prot args==address
#define CMD29 (0X1D | 0X40) // r1b clr_wr_prot args==address
#define CMD30 (0X1E | 0X40) // r1 send_wr_protect args==wp data addr
#define CMD32 (0X20 | 0X40) // r1 er/wr blk_start_addr args==data addr
#define CMD33 (0X21 | 0X40) // r1 er/wr blk_end_addr args==data addr
#define CMD38 (0X26 | 0X40) // r1b erase selected blk stuff_bits
#define CMD42 (0X2A | 0X40) // r1 lock_unlock  args stuff bit
//
#define CMD55 (0X67 | 0X40) // r1 app_cmd (appl spec comm arg stuff
#define CMD56 (0X70 | 0X40) // r1 gen_cmd [31.1]'0' 0 rd/wr
#define CMD58 (0X72 | 0X40) // r3 read ocr args ccs[30]=0(sdsc) arg stuff bits
#define CMD59 (0X73 | 0X40) // r1 crc_on_off [31.1]='0' bit[0]= 0>crc off
// acmd *must start with cmd55
#define ACMD13 (0X0D | 0X40) // r2 sd status stuff args
#define ACMD22 (0X16 | 0X40) // r1 send_num_wr_blk stuff args
#define ACMD23 (0X17 | 0X40) // r1 set wr_blk_er_cnt [31.23]stuff bit [22.0]num_blk
#define ACMD41 (0X29 | 0X40) // r1 send_op_cond [30]hcs stuff args
#define ACMD51 (0X33 | 0X40) // r1 read/send_scr args stuff

// responses are txed MSB_First
// R1 REAPONSE msb-always 0
#define R1_zero 0x80            // card idle
#define R1_param_error 0x40     // comm args addr are out of range
#define R1_addr_error 0x20      // mis_aligned addr
#define R1_erase_seq_error 0x10 // er seq error
#define R1_crc_error 0x08       // crc error
#define R1_illegal_comm 0x04    // comm err
#define R1_erase_reset 0x02     // while erase error
#define R1_idle 0x01

// R1b RESPONSE ==R1 response & extra (polled) bytes
#define R1_B 0x00 // card ready for next comm
// R2 RESPONSE ==R1 response + R2
#define R2_zero 0x00            // card idle
#define R2_param_error 0x40     // comm args addr are out of range
#define R2_addr_error 0x20      // mis_aligned addr
#define R2_erase_seq_error 0x10 // er seq error
#define R2_crc_error 0x08       // crc error
#define R2_illegal_comm 0x04    // comm err
#define R2_erase_reset 0x02     // while erase error
#define R2_idle 0x01
// byte 2
#define R2_csd_overwrite 0x80
#define R2_er_param 0x40     // invalid selection for erase
#define R2_wp_violation 0x20 // the blk is wr protected
#define R2_ecc_fail 0x10     // internal card controller applied but failed
#define R2_cc_error 0x08     // internal card controller fail
#define R2_error 0x04        // unknown error
#define R2_lock_err 0x02
#define R2_card_locked 0x01 // card locked

// RESPONSE R3= 5 BYTES response R1-RESPoresponse +[31.0]==OCR DATA
#define R3_zero 0x00            // card idle
#define R3_param_error 0x40     // comm args addr are out of range
#define R3_addr_error 0x20      // mis_aligned addr
#define R3_erase_seq_error 0x10 // er seq error
#define R3_crc_error 0x08       // crc error
#define R3_illegal_comm 0x04    // comm err
#define R3_erase_reset 0x02     // while erase error
#define R3_idle 0x01
// CYCLE 4 TIMES
// DATA TOKEN for I/O
// DATA RESPONSE
#define data_resp_crc_err 0x0B // stop tx by cmd12
#define data_resp_wr_err 0x0D  // stop tx by cmd12
#define data_resp_ok 0x05
// DATA tx WR/RD token byte **(MSB_FIRST)
//  single_blk_rd/single_blk_wr or multi_blk_rd
//[token]byte : [2-513]bytes data : [2] bytes crc ==> 516 bytes
#define DATA_TK_BYTE 0XEF
#define DATA_MULTI_WRITE 0X3F
#define DATA_STOP_WR 0XBF
// CARD I/O token response
#define RESP_TOKEN_ERROR 0X01
#define RESP_CC_ERROR 0X02
#define RESP_ECC_ERROR 0X04
#define RESP_ERROR_RANGE 0X08
//
#define CS_ON GPIO_BSRR_BR0
#define CS_OFF GPIO_BSRR_BS0
//
uint16_t del = 1000; // 625us
uint8_t version = 0;
uint8_t rd_buff[8];
uint8_t mbuff[8];
uint8_t csd_buff[16];
char sd_buff[512];
char wr_buff[512];
uint8_t response = 0;
uint16_t TIMEOUT;
uint16_t read_capacity = 0;
//
uint8_t chipStatus();
void stopTrans();
void EraseCard(uint32_t addr);
void spi_dma_write(char data[], uint16_t ssize);

void power(uint8_t val) // pb0
{
    switch (val)
    {
    case 1:
        GPIOB->BSRR = GPIO_BSRR_BS1;
        timer4_delay(2);
        break;
    case 0:
        GPIOB->BSRR = GPIO_BSRR_BR1;
        timer4_delay(2);
        break;
    default:
        break;
    }
}

void set_dma_comm(uint8_t comm, uint32_t args, uint8_t crc)
{
    char data[8];
    data[0] = (comm);
    data[1] = ((char)((args & 0xFF000000) >> 24));
    data[2] = ((char)((args & 0x00FF0000) >> 16));
    data[3] = ((char)((args & 0x0000FF00) >> 8));
    data[4] = ((char)(args & 0x000000FF));
    data[5] = (crc);
    data[6] = (0XFF);
    data[7] = (0XFF);
    spi_dma_write(data, 8);
}

uint8_t command(uint8_t comm, uint8_t err_num, uint32_t args, uint8_t crc)
{
    GPIOB->BSRR = CS_ON; // CS
    TIMEOUT = 0XFFE;
    // spi_dma_write(sd_buff, 512);
    do
    {
        spi2_send(comm);
        spi2_send((uint8_t)((args & 0xFF000000) >> 24));
        spi2_send((uint8_t)((args & 0x00FF0000) >> 16));
        spi2_send((uint8_t)((args & 0x0000FF00) >> 8));
        spi2_send((uint8_t)(args & 0x000000FF));
        spi2_send(crc);
        spi2_send(0XFF);
        spi2_send(0XFF);

        response = spi2_send(0XFF);
        // eusart_send(response);
        if (response == err_num)
        {
            goto commData;
        }
        TIMEOUT--;
        timer4_delay(4);
    } while (TIMEOUT >= 1);

    eusart_send(0xee);
    eusart_send(response);
    eusart_send(comm);
    SPI2->CR1 &= ~SPE;
    GPIOB->BSRR = CS_OFF;
    power(0);
    return (0);
commData:
    // eusart_send(response);
    return response;
}

void sd_init(uint16_t baud)
{
    GPIOB->CRL = (P_P50MHZ << GPIO_CRL_MODE0_Pos);  // CS
    GPIOB->CRL |= (P_P50MHZ << GPIO_CRL_MODE1_Pos); // POWER

    spi2_init(baud);
    timer4_delay(2);

    power(1);
    for (uint8_t i = 0; i < 10; i++)
    {
        spi2_send(0XFF);
    }
    command(CMD0, 0x01, 0x0000, 0X95); // 2GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void sd_card_cond_8() // 5 R7
{
    command(CMD8, 0x01, 0x000001AA, 0x87); // 2GB
    for (uint8_t i = 0; i < 4; i++)
    {
        response = spi2_send(0XFF);
        // eusart_send(response);
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void read_opt_cond_41() // 4
{
    command(CMD55, 0x05, 0x00000000, 0X95); // 2GB
    spi2_send(0xff);
    spi2_send(0xff);
    GPIOB->BSRR = CS_OFF;
    command(ACMD41, 0x01, 0x40000000, 0X95); // 2GB
    // command(ACMD41, 0x05, 0x40000000, 0X95); // 8GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
    command(ACMD41, 0x00, 0x40000000, 0X95); // 2GB
    // command(ACMD41, 0x05, 0x40000000, 0X95); // 8GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void read_ocr_58()
{
    command(CMD58, 0x04, 0x00000000, 0X95); // 2GB
    // command(CMD58, 0x05, 0x00000000, 0X95); // 8GB
    for (uint8_t i = 0; i < 4; i++)
    {
        response = spi2_send(0XFF);
        eusart_send(response);
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void acmd_status13()
{
    response = 0;
    command(CMD55, 0x05, 0x00000000, 0X95); // 2GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    // return response;
    GPIOB->BSRR = CS_OFF;

    command(ACMD13, 0x01, 0x00000000, 0X95); // 2GB
    // command(ACMD41, 0x05, 0x40000000, 0X95); // 8GB
    spi2_send(0XFF);
    spi2_send(0XFF);

    GPIOB->BSRR = CS_OFF;
}
void get_csd()
{
    command(CMD9, 0x00, 0x00000000, 0X95); // 2GB
    // command(CMD9, 0x05, 0x40000000, 0X95); // 8GB
    for (uint8_t i = 16; i > 0; i--)
    {
        response = spi2_send(0XFF);
        csd_buff[i] = response;
        eusart_send(response);
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
    // getByteValue(csd_buff);
    // getCsdValueV2(csd_buff);
    // getCsdValueV1(csd_buff);
}
void optionCrc(uint8_t val)
{ // 1=crc-on 0=crc-off
    command(CMD59, 0x00, (0x00 | val), 0X95);
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void set_wr_blk_erase(uint32_t address)
{ // 1=crc-on 0=crc-off
    command(CMD55, 0x00, 0x00, 0X95);
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
    command(ACMD23, 0x00, (address << 9), 0X95);
    for (uint16_t i = 0; i < 0xFFFE; i++)
    {
        GPIOB->BSRR = CS_OFF;
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
uint8_t chipStatus()
{
    command(CMD13, 0x00, 0x0000, 0X95); // 2GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    for (uint16_t i = 0; i < 0xFF; i++)
    {
        GPIOB->BSRR = CS_OFF; // This is critical WHY?? don know why.....
        timer4_delay(2);
    }
    return response;
}
void stopTrans() // INCASE OF MULTY WR
{
    command(CMD12, 0x01, 0x00000000, 0X95); // 2GB
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void sdErase(uint32_t addr_st, uint32_t addr_end)
{
    command(CMD32, 0x00, (addr_st << 9), 0X95); // start addr
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
    command(CMD33, 0x00, (addr_end << 9), 0X95); // end addr
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
    command(CMD38, 0x00, 0x00, 0X95); // erase
    spi2_send(0XFF);
    spi2_send(0XFF);
    GPIOB->BSRR = CS_OFF;
}
void stopSpi()
{
    SPI2->CR1 &= ~SPE;
    GPIOB->BSRR = CS_ON;
    power(0);
}
//
// READ BUFF
char *sdRead(uint32_t addr) // cmd17 sd_buff
{
    uint16_t ccn = 0x4FF;
    response = command(CMD17, 0x00, (addr << 9), 0X95); // 2GB
    do
    {
        response = spi2_send(0XFF);
        if (response == 0xFE) // 2GB
        {
            goto post;
        }
        ccn--;
    } while (ccn >= 1);
    GPIOB->BSRR = CS_OFF; // cs
    SPI2->CR1 &= ~SPE;
    eusart_send(0Xee);
    eusart_send(response);
    power(0);
    return sd_buff;
post:
    for (uint16_t i = 0; i < 512; i++)
    {
        response = spi2_send(0XFF);
        sd_buff[i] = response;
        // eusart_send(sd_buff[i]);
        timer4_delay(2);
    }
    for (uint16_t i = 0; i < 0xFFFE; i++)
    {
        GPIOB->BSRR = CS_OFF; // This is critical WHY?? don know why.....
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
    return sd_buff;
}
void sdRead_buff(uint32_t addr, uint16_t posStr, uint16_t posEnd) // cmd17 sd_buff
{
    uint16_t ccn = 0x4FF;
    response = command(CMD17, 0x00, (addr << 9), 0X95); // 2GB
    do
    {
        response = spi2_send(0XFF);
        if (response == 0xFE) // 2GB
        {
            goto post;
        }
        ccn--;
    } while (ccn >= 1);
    GPIOB->BSRR = CS_OFF;
    SPI2->CR1 &= ~SPE;
    eusart_send(0Xee);
    eusart_send(response);
    power(0);
    // return 0;
post:
    for (uint16_t i = 0; i < 512; i++)
    {
        response = spi2_send(0xFF);
        sd_buff[i] = response;
        if ((i >= posStr) & (i <= posEnd))
        {
            sd_buff[i] = response;
            // eusart_send(sd_buff[i]);
        }
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
    // dma_uart_send(sd_buff, 512);
    // return 0;
}
uint8_t sdRead_pos(uint32_t addr, uint16_t begins, uint16_t ends) // cmd17 sd_buff
{
    uint16_t ccn = 0x4FF;
    response = command(CMD17, 0x00, (addr << 9), 0X95); // 2GB
    do
    {
        response = spi2_send(0XFF);
        if (response == 0xFE) // 2GB
        {
            goto post;
        }
        ccn--;
    } while (ccn >= 1);
    GPIOB->BSRR = CS_OFF;
    SPI2->CR1 &= ~SPE;
    eusart_send(0Xee);
    eusart_send(response);
    power(0);
    return 0;
post:

    for (uint16_t i = 0; i < 512; i++)
    {
        response = spi2_send(0xFF);
        timer4_delay(10);
        if (i >= begins && i <= ends)
        {
            sd_buff[i] = response;
            eusart_send(sd_buff[i]);
        }
        else
        {
            sd_buff[i] = response;
        }
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
    return 0;
}
//
// DMA

void spi_dma_read(uint32_t addr) // not working
{
    response = command(CMD17, 0x00, (addr << 9), 0X95); // 2GB
    char msg[514];
    spi2_dma_rx_init(BAUD_FCLK_64, msg, 514);
    for (uint16_t i = 0; i < 514; i++)
    {
        eusart_send(spi_buff[i]);
    }
}
//
void spi_dma_write(char data[], uint16_t ssize)
{
    dma_spi_send(data, ssize);
}
//
void EraseCard(uint32_t addr) // cmd17 sd_buff
{
    uint16_t ccn = 0x4FF;
    command(CMD24, 0x00, (addr << 9), 0X95); // 2GB
    response = spi2_send(0xFE);

    for (uint16_t i = 0; i < 512; i++)
    {
        spi2_send(0xFF);
        // timer4_delay(1);
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    do
    {
        response = spi2_send(0XFF);
        if (response == 0x05)
        {
            goto wrMem;
        }
        ccn--;
    } while (ccn >= 1);
wrMem:
    for (uint16_t i = 0; i < 0xFFFE; i++)
    {
        GPIOB->BSRR = CS_OFF; // This is critical WHY?? don know why.....
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
    spi2_send(0XFF);
    spi2_send(0XFF);
}
//
// WRITE BUFF
void sdWrite_String(uint32_t addr, char data[]) // cmd24
{
    char buff[512];
    memset(buff, 0xFF, 512);
    strcpy(buff, data);
    uint16_t count = 0;
    uint16_t ccn = 0x2FF;

    for (uint16_t i = 0; i < strlen(data); i++)
    {
        buff[i] = data[count];
        count++;
    }

    command(CMD24, 0x00, (addr << 9), 0X95); // 2GB

    response = spi2_send(0xFE);
    for (uint16_t i = 0; i < 512; i++)
    {
        spi2_send(buff[i]);
    }
    // spi_dma_write(buff,512);

    spi2_send(0XFF);
    spi2_send(0XFF);
    do
    {
        response = spi2_send(0XFF);
        if (response == 0x05)
        {
            goto wrMem;
        }
        ccn--;
    } while (ccn >= 1);
wrMem:
    for (uint16_t i = 0; i < 0xFFFE; i++)
    {
        GPIOB->BSRR = CS_OFF; // This is critical WHY?? don know why.....
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
}
void sdWrite_pos_buff(uint32_t addr, char buff[], uint16_t posStr)
{
    uint16_t count = 0;
    uint16_t ccn = 0x4FF;

    sdRead(addr); // sd_buff
    EraseCard(addr);

    for (uint16_t i = 0; i <= strlen(buff); i++)
    {
        sd_buff[posStr + i] = buff[count];
        count++;
    }
    command(CMD24, 0x00, (addr << 9), 0X95); // 2GB

    response = spi2_send(0xFE);
    for (uint16_t i = 0; i < 512; i++)
    {
        spi2_send(sd_buff[i]);
        // timer4_delay(1);
    }
    // spi_dma_write(sd_buff, 512);

    spi2_send(0XFF);
    spi2_send(0XFF);
    do
    {
        response = spi2_send(0XFF);
        if (response == 0x05)
        {
            goto wrMem;
        }
        ccn--;
    } while (ccn >= 1);
wrMem:
    for (uint16_t i = 0; i < 0xFFFE; i++)
    {
        GPIOB->BSRR = CS_OFF; // This is critical WHY?? don know why.....
    }
    spi2_send(0XFF);
    spi2_send(0XFF);
    chipStatus();
    GPIOB->BSRR = CS_OFF;
    spi2_send(0XFF);
    spi2_send(0XFF);
}
// BUFF nanipulate
uint16_t comp_buff(char buff1[], char buff2[], uint16_t size)
{
    uint16_t cnt = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        if (buff1[i] != buff2[i])
        {
            cnt = 0;
        }
        if (buff1[i] == buff2[i])
        {
            cnt++;
        }
    }
    return cnt;
}
uint16_t get_address(char mem_buff[], char my_word[], uint16_t size)
{
    uint16_t cnt = 0;
    uint16_t posEnd = 0;
    char buff[size];

    for (uint16_t i = 0; i < 512; i++)
    {
        for (uint16_t j = 0; j < size; j++)
        {
            if (mem_buff[i] != my_word[j])
            {
                cnt = 0;
            }
            if (mem_buff[i] == my_word[j])
            {
                buff[j] = my_word[j];
                posEnd = i;
                // eusart_send(my_word[j]); //************
                i++;
                cnt++;
            }
        }
        if (comp_buff(buff, my_word, size) == size)
        {
            break;
        }
    }
    return (posEnd - size);
}

// Get the first word ocurrence
uint16_t sd_get_addr(uint32_t addr, char *word)
{
    char buff[strlen(word)];
    memset(sd_buff, 0, 512);
    strcpy(buff, word);
    sdRead(addr); // sd_buff
    return get_address(sd_buff, buff, strlen(word));
}
void sd_read_Address(uint32_t addr, uint16_t pos, uint16_t size)
{
    sdRead_pos(addr, pos, (pos + size));
}
///////////////
///////////////
///////////////
////// func pointers ///
uint32_t counter = 0;
uint32_t countx = 0;
uint32_t addr_jump = 0;
uint32_t addr = 0;
//
//
uint32_t get_file_addr()
{
    sdRead_buff(0, 0, 3); // sd_buff
    uint32_t address;
    address = (sd_buff[0] & 0xFF);
    address |= ((sd_buff[1] & 0xFF) << 8);
    address |= ((sd_buff[2] & 0xFF) << 16);

    return address;
}
void set_file_addr(uint32_t addr, uint32_t f_addr, uint16_t pos)
{
    char buff[3];
    buff[3] = ((f_addr & 0xFF0000) >> 16);
    buff[2] = ((f_addr & 0x00FF00) >> 8);
    buff[1] = ((f_addr & 0x0000FF));
    sdWrite_pos_buff(addr, buff, pos);
}
void write_fname(uint32_t addr, char file[], uint16_t pos)
{
    // char buff[3];

    // sdRead_buff(0, 0, 3); // sd_buff

    // buff[0] = '0';
    // buff[1] = '0';
    // buff[2] = '0';

    // file[ssize + 1] = buff[2];
    // file[ssize + 2] = buff[1];
    // file[ssize + 3] = buff[0];
    // strcat(file, buff);
    sdWrite_pos_buff(addr, file, pos);
}
//--------------------------------------

#endif // _SD_CARD
