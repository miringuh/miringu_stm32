#if !defined(_ATS)
#define _ATS
#include "/usr/lib/stm32/stm32F1xx_headers/stm32f1xx.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "gpio.h"
#include "eusart.h"
#include "spi_Lcd.h"
/////// INPUT MAN SEL SWITCH PB3..PB5 //////
#define MAN_STRT_PIN GPIO_IDR_IDR3
#define MAN_PIN GPIO_IDR_IDR4
#define AUTO_PIN GPIO_IDR_IDR5
/////// INPUT 240V SSD PB6...PB7 XXXX ////////
#define GEN_PW GPIO_IDR_IDR6
#define KPLC_PW GPIO_IDR_IDR7
/////  OUTPUT 12/24V DC RELAY/SSD PB8...PB11 /////
#define K1 GPIO_ODR_ODR8 // kplc
#define K2 GPIO_ODR_ODR9 // gen
#define CHOKE GPIO_ODR_ODR10
#define GEN_IGN GPIO_ODR_ODR11 //***CRH
///// ADC ANALOGS  PA0...PA2 //////////
#define OIL GPIO_IDR_IDR0
#define COOLANT GPIO_IDR_IDR1
#define FUEL GPIO_IDR_IDR2
///////////////
#define STATUS (GPIOB->IDR & 0X0F8)
//////
uint8_t error_status = 0;
uint8_t status = 0;
uint8_t Atsdel = 2;
/////// LCD->ADC //////

void ats_delay(uint16_t cyc)
{
    for (uint16_t i = 0; i < cyc; i++)
    {
        timer1_delay(500);
        timer1_delay(500);
    }
}

void lcd_char2hex(uint16_t val) // 12bit
{

    // uint8_t h = (uint8_t)((val & 0xC00) >> 8) + 256;
    // uint8_t m = (uint8_t)((val & 0x0F0) >> 4) + 16;
    // uint8_t l = (uint8_t)((val & 0x00F));

    uint8_t high = (uint8_t)((val & 0xF00) >> 8);
    uint8_t mid = (uint8_t)((val & 0x0F0) >> 4);
    uint8_t low = (uint8_t)(val & 0x00F);

    lcd_command(CLEAR_DISP);
    write4Char(' ');
    write4Char(0x30 | high);
    write4Char(0x30 | mid);
    write4Char(0x30 | low);
}

void lcd_adc_config()
{
    // RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    GPIOB->CRL |= GPIO_CRL_CNF3_0; // MAN STRT
    GPIOB->CRL |= GPIO_CRL_CNF4_0; // MAN
    GPIOB->CRL |= GPIO_CRL_CNF5_0; // AUT0
    GPIOB->CRL |= GPIO_CRL_CNF6_0; // GEN PW
    GPIOB->CRL |= GPIO_CRL_CNF7_0; // KPLC PW

    GPIOB->CRH |= (P_P10MHZ << GPIO_CRH_MODE8_Pos);  // K1
    GPIOB->CRH |= (P_P10MHZ << GPIO_CRH_MODE9_Pos);  // K2
    GPIOB->CRH |= (P_P10MHZ << GPIO_CRH_MODE10_Pos); // CHOKE
    GPIOB->CRH |= (P_P10MHZ << GPIO_CRH_MODE11_Pos); // GEN_IGN

    GPIOA->CRL &= ~(GPIO_CRL_CNF0_Msk | GPIO_CRL_MODE0_Msk); // PA0
    GPIOA->CRL &= ~(GPIO_CRL_CNF1_Msk | GPIO_CRL_MODE1_Msk); // PA1
    GPIOA->CRL &= ~(GPIO_CRL_CNF2_Msk | GPIO_CRL_MODE2_Msk); // PA2
    // CONV NUM
    ADC1->SQR3 = (0 << ADC_SQR3_SQ1_Pos);
    ADC1->SQR3 |= (1 << ADC_SQR3_SQ2_Pos);
    ADC1->SQR3 |= (2 << ADC_SQR3_SQ3_Pos);
    // sample time for each channel
    ADC1->SMPR2 = (1 << ADC_SMPR2_SMP0_Pos);
    ADC1->SMPR2 |= (1 << ADC_SMPR2_SMP1_Pos);
    ADC1->SMPR2 |= (1 << ADC_SMPR2_SMP2_Pos);
    // total conv SEQ_LEN
    ADC1->SQR1 = ADC_SQR1_L_2; //
}
void lcd_adcInit()
{
    // RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
    ADC1->SR = 0;
    ADC1->CR1 = ADC_CR1_SCAN;
    ADC1->CR2 = ADC_CR2_CONT;
    // ADC1->CR2 &= ~ADC_CR2_ALIGN;
    ADC1->CR2 |= ADC_CR2_ALIGN;
    ADC1->CR2 |= ADC_CR2_EXTTRIG;    // ext trg enable
    ADC1->CR2 |= ADC_CR2_EXTSEL_Msk; // swstart
    ADC1->SR &= ~ADC_SR_STRT;

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL)
        ;
    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_CAL))
        ;
    while ((ADC1->SR & ADC_SR_STRT))
    {
        ADC1->SR &= ~ADC_SR_STRT;
    }
    // ADC1->CR1 |= ADC_CR1_EOCIE;
    // NVIC_EnableIRQ(ADC1_2_IRQn);
    // NVIC_SetPriority(ADC1_2_IRQn, 2);

    ADC1->CR2 |= ADC_CR2_ADON;
    // eusart_send((uint8_t)(ADC1->SR & 0X0FF));
}
void lcd_getAdc()
{
    // ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SR &= ~ADC_SR_STRT;
    while (!(ADC1->SR & ADC_SR_STRT))
        ;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_CR2_SWSTART))
    {
    }
    while (!(ADC1->SR & ADC_SR_EOC))
        ;
    uint16_t val = (ADC1->DR);
    lcd_char2hex(val);
}
/////////////

void ats_init()
{
    lcd_adcInit();
    lcd4_init(BAUD_FCLK_64); // Adc Inpus
    lcd_4_init();
    lcd_adc_config(); //
}
void func_choke()
{
    GPIOB->ODR |= CHOKE;
    ats_delay(10);
    GPIOB->ODR &= ~CHOKE;
    ats_delay(5);
}
uint8_t gen_Start()
{
    uint8_t attempts = 4;
    write4DataHigh("GEN STARTING...");
    ats_delay(Atsdel);
    func_choke();
    while (!(GPIOB->IDR & GEN_PW) || (attempts >= 1))
    {
        GPIOB->ODR |= GEN_IGN;
        ats_delay(4);
        GPIOB->ODR &= ~GEN_IGN;
        ats_delay(Atsdel);
        attempts--;
        write4DataHigh("Attempt ");
        write4Char(attempts | 0x30);
        ats_delay(10);
        if ((GPIOB->IDR & GEN_PW))
        {
            goto gen_on;
        }
        if (attempts <= 1)
        {
            goto genError;
        }
        ats_delay(Atsdel);
    }
gen_on:
    write4DataHigh("--GEN-RUNNING--");
    ats_delay(Atsdel);
    return 0;
genError:
    write4Data("GEN-START ERROR", "ATTEMPTS EXCEEDS");
    ats_delay(Atsdel);
    return 0;
}

uint8_t func_kplc()
{
    ats_delay(Atsdel);
    write4DataHigh("POWER MODE SETUP ");
    ats_delay(Atsdel);
    if (!(GPIOB->IDR & AUTO_PIN) && !(GPIOB->IDR & MAN_PIN))
    {
        GPIOB->ODR &= ~K1;
        GPIOB->ODR &= ~K2;
        write4DataHigh("SET MAN/AUTO ");
        ats_delay(Atsdel);
        status = STATUS;
        return 0;
    }

    while (1)
    {
        // AUTO
        if ((GPIOB->IDR & AUTO_PIN) && !(GPIOB->IDR & MAN_PIN)) // AUTO
        {
            if (!(GPIOB->IDR & KPLC_PW) && !(GPIOB->IDR & GEN_PW)) // no kplc/gen
            {
                GPIOB->ODR &= ~K1;
                GPIOB->ODR &= ~K2;
                ats_delay(Atsdel);
                gen_Start();
                ats_delay(10);
                if (!(GPIOB->IDR & GEN_PW))
                {
                    status = STATUS;
                    break;
                }
                GPIOB->ODR |= K2;
                write4DataHigh("....GEN ON 0...");
                ats_delay(Atsdel);
                status = STATUS;
                break;
            }
            if ((GPIOB->IDR & KPLC_PW) && !(GPIOB->IDR & GEN_PW)) // kplc only
            {
                GPIOB->ODR &= ~K2;
                // GPIOB->ODR &= K1;
                ats_delay(Atsdel);
                GPIOB->ODR |= K1;
                write4DataHigh("....POWER ON 0....");
                ats_delay(Atsdel);
                status = STATUS;
                break;
            }
            if (!(GPIOB->IDR & KPLC_PW) && (GPIOB->IDR & GEN_PW)) // gen only
            {
                // gen_Start();
                GPIOB->ODR &= ~K1;
                ats_delay(Atsdel);
                GPIOB->ODR |= K2;
                write4DataHigh("....GEN ON 1....");
                ats_delay(Atsdel);
                status = STATUS;
                break;
            }
            if ((GPIOB->IDR & KPLC_PW) && (GPIOB->IDR & GEN_PW)) // kplc &= gen
            {
                ats_delay(Atsdel);
                GPIOB->ODR &= ~K2;
                // ats_delay(Atsdel);
                GPIOB->ODR |= K1;
                ats_delay(Atsdel);                
                write4DataHigh("....POWER ON 1....");
                ats_delay(20);
                func_choke();
                write4DataHigh("....GEN OFF....");
                ats_delay(Atsdel);
                write4DataHigh("....POWER ON 1....");
                status = STATUS;
                break;
            }
        }
        // MANUAL
        if (!(GPIOB->IDR & AUTO_PIN) && (GPIOB->IDR & MAN_PIN)) // MANUAL
        {
            if (!(GPIOB->IDR & KPLC_PW) && !(GPIOB->IDR & GEN_PW)) // no kplc/gen
            {
                if ((GPIOB->IDR & MAN_STRT_PIN))
                {
                    GPIOB->ODR &= ~K1;
                    GPIOB->ODR &= ~K2;
                    ats_delay(Atsdel);
                    gen_Start(); // ****
                    ats_delay(Atsdel);
                    if (!(GPIOB->IDR & GEN_PW))
                    {
                        status = STATUS;
                        break;
                    }
                    GPIOB->ODR |= K2;
                    write4DataHigh("..GEN MAN START..");
                    ats_delay(Atsdel);
                    status = STATUS;
                    break;
                }
            }
            if ((GPIOB->IDR & KPLC_PW) && !(GPIOB->IDR & GEN_PW)) // kplc only
            {
                if ((GPIOB->IDR & MAN_STRT_PIN))
                {
                    GPIOB->ODR &= ~K2;
                    ats_delay(Atsdel);
                    GPIOB->ODR |= K1;
                    ats_delay(Atsdel);
                    gen_Start(); // ****
                    ats_delay(Atsdel);
                    if (!(GPIOB->IDR & GEN_PW))
                    {
                        status = STATUS;
                        break;
                    }
                    write4DataHigh("..POWER/GEN ON...");
                    ats_delay(Atsdel);
                    status = STATUS;
                    break;
                }
            }
            if (!(GPIOB->IDR & KPLC_PW) && (GPIOB->IDR & GEN_PW)) // gen only
            {
                GPIOB->ODR &= ~K1;
                ats_delay(Atsdel);
                GPIOB->ODR |= K2;
                write4DataHigh("GEN RUNNING");
                ats_delay(Atsdel);
                status = STATUS;
                break;
            }
            if ((GPIOB->IDR & KPLC_PW) && (GPIOB->IDR & GEN_PW)) // kplc & gen
            {
                GPIOB->ODR &= ~K2;
                ats_delay(Atsdel);
                GPIOB->ODR |= K1;
                ats_delay(Atsdel);
                write4DataHigh("GEN MAN RUNNING");
                ats_delay(Atsdel);
                status = STATUS;
                break;
            }
        }
        // CONFIGs

        status = STATUS;
        break;
    }
    return 0;
}

void run_ats()
{
    if ((status != STATUS))
    {
        if (((STATUS & 0X01) != 1))
        {
            func_kplc();
            ats_delay(1);
        }
    }

    // lcd_getAdc();
    // ats_delay(1);
}
#endif // _ATS
