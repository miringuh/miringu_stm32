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
/////// INPUT MAN SEL SWITCH //////
#define MAN_STRT_PIN GPIO_IDR_IDR0
#define MAN_PIN GPIO_IDR_IDR1
#define AUTO_PIN GPIO_IDR_IDR2
/////// INPUT 240V SSD ////////
#define GEN_PW GPIO_IDR_IDR3
#define KPLC_PW GPIO_IDR_IDR4
/////  OUTPUT 12/24V DC RELAY/SSD /////
#define K1 GPIO_ODR_ODR5 // kplc
#define K2 GPIO_ODR_ODR6 // gen
#define CHOKE GPIO_ODR_ODR7
#define GEN_IGN GPIO_ODR_ODR8 //***CRH
///// ADC INPUT  //////////
#define OIL GPIO_IDR_IDR9
#define COOLANT GPIO_IDR_IDR10
#define FUEL GPIO_IDR_IDR11
///////////////
#define STATUS (GPIOA->IDR & 0X001F)
//////
uint8_t error_status = 0;
uint8_t status = 0;
/////////////

/////

/////////////
void ats_delay(uint16_t cyc)
{
    for (uint16_t i = 0; i < cyc; i++)
    {
        timer1_del(_500ms);
        timer1_del(_500ms);
    }
}
void adc_char2hex()
{
    uint16_t data = get_ADC();
    lcd_command(CLEAR_DISP);
    write4Char((uint8_t)(data & 0x00F) | 0x30);
    write4Char((uint8_t)((data & 0x0F0) >> 4) | 0x30);
    write4Char((uint8_t)((data & 0xF00) >> 8)|  0x30);
    ats_delay(2);
}
void ats_init()
{                         // INPUTS
    setPinA(P_P2MHZ, 0);  // strt
    setPinA(P_P2MHZ, 1);  // man
    setPinA(P_P2MHZ, 2);  // auto
    setPinA(P_P2MHZ, 3);  // gen
    setPinA(P_P2MHZ, 4);  // kplc
                          // OUTPUTS
    setPinA(P_P50MHZ, 5); // k1
    setPinA(P_P50MHZ, 6); // k2
    setPinA(P_P50MHZ, 7); // choke
    setPinA(P_P50MHZ, 8); // gen ign

    setPinA(FLOAT_INP, 9);
    setPinA(FLOAT_INP, 10);
    setPinA(FLOAT_INP, 11);

    setPinB(ANALOG, 0);
    // ADC_Init(REG_B, ANALOG, 0);
    // GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    //
    lcd4_init(BAUD_FCLK_64); // Adc Inpus
    lcd_4_init();
}
void func_choke()
{
    GPIOA->ODR |= CHOKE;
    ats_delay(2);
    GPIOA->ODR &= ~CHOKE;
    ats_delay(2);
}
uint8_t gen_Start()
{
    uint8_t attempts = 4;
    write4DataHigh("GEN STARTING...");
    ats_delay(2);
    func_choke();
    while (!(GPIOA->IDR & GEN_PW) || (attempts >= 1))
    {
        GPIOA->ODR |= GEN_IGN;
        ats_delay(1);
        GPIOA->ODR &= ~GEN_IGN;
        ats_delay(1);
        attempts--;
        write4DataHigh("Attempt ");
        write4Char(attempts | 0x30);

        ats_delay(1);
        if ((GPIOA->IDR & GEN_PW))
        {
            goto gen_on;
        }
        if (attempts <= 1 && !(GPIOA->IDR & GEN_PW))
        {
            goto ends;
        }
        ats_delay(1);
    }
gen_on:
    write4DataHigh("--GEN-RUNNING--");
    ats_delay(1);
    status = STATUS;
    return 0;
ends:
    write4Data("GEN-START ERROR", "ATTEMPTS EXCEEDS");
    ats_delay(2);
    status = STATUS;
    return 0;
}
uint8_t func_kplc()
{
    write4DataHigh("POWER MODE SETUP ");
    ats_delay(2);

    if (!(GPIOA->IDR & AUTO_PIN) && !(GPIOA->IDR & MAN_PIN))
    {
        GPIOA->ODR &= ~K1;
        GPIOA->ODR &= ~K2;
        write4DataHigh("SET MAN/AUTO ");
        ats_delay(2);
        status = STATUS;
        return 0;
    }
    while (1)
    {
        // AUTO
        if ((GPIOA->IDR & AUTO_PIN) && !(GPIOA->IDR & MAN_PIN)) // AUTO
        {
            if (!(GPIOA->IDR & KPLC_PW) && !(GPIOA->IDR & GEN_PW)) // no kplc/gen
            {
                GPIOA->ODR &= ~K1;
                GPIOA->ODR &= ~K2;
                ats_delay(1);
                gen_Start();
                ats_delay(1);
                if (!(GPIOA->IDR & GEN_PW))
                {
                    break;
                }

                GPIOA->ODR |= K2;
                write4DataHigh("....GEN ON 0...");
                ats_delay(2);
                status = STATUS;
                break;
            }
            if ((GPIOA->IDR & KPLC_PW) && !(GPIOA->IDR & GEN_PW)) // kplc only
            {
                GPIOA->ODR &= ~K2;
                // GPIOA->ODR &= ~K1;
                ats_delay(2);
                GPIOA->ODR |= K1;
                write4DataHigh("....POWER ON 0....");
                ats_delay(2);
                status = STATUS;
                break;
            }
            if (!(GPIOA->IDR & KPLC_PW) && (GPIOA->IDR & GEN_PW)) // gen only
            {
                GPIOA->ODR |= K2;
                write4DataHigh("....GEN ON 1....");
                ats_delay(2);
                status = STATUS;
                break;
            }
            if ((GPIOA->IDR & KPLC_PW) && (GPIOA->IDR & GEN_PW)) // kplc &= gen
            {
                ats_delay(2);
                GPIOA->ODR |= K1;
                GPIOA->ODR &= ~K2;
                func_choke();
                write4DataHigh("....GEN OFF....");
                ats_delay(2);
                write4DataHigh("....POWER ON 1....");
                ats_delay(2);
                status = STATUS;
                break;
            }
        }
        // MANUAL
        if (!(GPIOA->IDR & AUTO_PIN) && (GPIOA->IDR & MAN_PIN)) // AUTO
        {

            if (!(GPIOA->IDR & KPLC_PW) && !(GPIOA->IDR & GEN_PW)) // no kplc/gen
            {
                if ((GPIOA->IDR & MAN_STRT_PIN))
                {
                    GPIOA->ODR &= ~K1;
                    GPIOA->ODR &= ~K2;
                    ats_delay(2);
                    gen_Start(); // ****
                    ats_delay(2);
                    GPIOA->ODR |= K2;
                    write4DataHigh("..GEN MAN START..");
                    ats_delay(2);
                    status = STATUS;
                    break;
                }
            }
            if ((GPIOA->IDR & KPLC_PW) && !(GPIOA->IDR & GEN_PW)) // kplc only
            {
                if ((GPIOA->IDR & MAN_STRT_PIN))
                {
                    // GPIOA->ODR &= ~K1;
                    GPIOA->ODR &= ~K2;
                    ats_delay(2);
                    gen_Start(); // ****
                    ats_delay(2);
                    write4DataHigh("..POWER/GEN ON...");
                    ats_delay(2);
                    status = STATUS;
                    break;
                }
            }
            if (!(GPIOA->IDR & KPLC_PW) && (GPIOA->IDR & GEN_PW)) // gen only
            {
                GPIOA->ODR &= ~K1;
                GPIOA->ODR |= K2;
                write4DataHigh("GEN RUNNING");
                ats_delay(2);
                status = STATUS;
                break;
            }
            if ((GPIOA->IDR & KPLC_PW) && (GPIOA->IDR & GEN_PW)) // kplc & gen
            {
                write4DataHigh("GEN MAN RUNNING");
                ats_delay(2);
                status = STATUS;
                break;
            }
        }
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
            // func_kplc();
        }
        ats_delay(1);
    }
    adc_char2hex();
}
#endif // _ATS
