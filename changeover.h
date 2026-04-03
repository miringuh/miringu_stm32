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
#include "spiLcd.h"
/////// MAN SEL SWITCH //////
#define MAN_STRT_PIN 0X01
#define MAN_PIN 0X02
#define AUTO_PIN 0X04
/////// INPUT 240V SSD ////////
#define GEN_PW 0X08
#define KPLC_PW 0X10
/////  OUTPUT 12/24V DC RELAY/SSD /////
#define K1 0x80
#define K2 0x40
#define CHOKE 0x20
#define GEN_IGN 0x10
///// ADC //////////
#define OIL 0x11
#define COOLANT 0x12
#define FUEL 0x13
///////////////

void func_delay(uint16_t cyc)
{
    timer1_del(cyc);
}
void ats_init()
{
    timer1_del_init();
    

}
void func_choke() {}
void gen_Start() {}
void func_kplc() {}

#endif // _ATS
