/*
 * File:   Lab9.c
 * Author: stalbot8
 *
 * Created on November 14, 2018, 3:07 PM
 */


#include "xc.h"
#pragma config FNOSC = FRC
#define FCY 4000000UL
#include <libpic30.h>

void configSPI() {
    _SPIEN = 0;
    _MODE16 = 0;
    _CKP = 0;
    _CKE = 1;
    _MSTEN = 1;
    _SPRE = 0b111;
    _PPRE = 0b10;
    _SPIEN = 1;
    
}

int main(void) {
    
    configSPI();
    while(1) {
        /*
        SPI1BUF = 0x0;  //reset
        __delay_us(200);
        SPI1BUF = 0xFF;         //all green
        __delay_us(200);
        SPI1BUF = 0x80;         //no red
        __delay_us(200);
        SPI1BUF = 0x80;         //no blue
        __delay_us(200);
        
        int i = 0;
        int j = 0;
        for (i = 0; i < 32; i++) {
            if (i == 0) {
                SPI1BUF = 0x0;  //reset
                __delay_us(200);
            }
        SPI1BUF = 0x80;         // green
        __delay_us(200);
        SPI1BUF = 0xC0;         // red
        __delay_us(200);
        SPI1BUF = 0xC0;         // blue
        __delay_us(200);
        } */
        
        int i = 0;
        int j = 0;
        int k = 0;
        int a = 0;
        int b = 0;
        for (i = 1; i <= 32; i++) {
            SPI1BUF = 0x0;  //reset
            __delay_us(200);
            for (j = 1; j <= 32; j++) {
                if (j <= i) {               //turn on
                    if (j == 32) {
                        SPI1BUF = 0x9B;         // green
                        __delay_us(200);
                        SPI1BUF = 0xC0;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                        for (k = 0; k < 6; k++) {
                            if (k%2 == 0) {
                                ///////////////////////////////////////////////////////////////////////////
                                for (a = 1; a <= 32; a++) {
                                    SPI1BUF = 0x0;  //reset
                                    __delay_us(1);
                                    for (b = 1; b <= 32; b++) {
                                        if (b <= a) {               //turn on
                                            if (b == 32) {
                                                SPI1BUF = 0x9B;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0xC0;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(200000);

                                            }
                                            else if (b%2 == 1) {             //is odd
                                                SPI1BUF = 0xFF;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(5);
                                            }
                                            else {
                                                SPI1BUF = 0x80;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0xFF;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(5);
                                            }
                                        }
                                        else {                      //turn off
                                            SPI1BUF = 0x80;         // green
                                            __delay_us(5);
                                            SPI1BUF = 0x80;         // red
                                            __delay_us(5);
                                            SPI1BUF = 0x80;         // blue
                                            __delay_us(5);
                                        }
                                    }
                                }
                                ////////////////////////////////////////////////////////////////////////////
                            } else {
                                for (a = 1; a <= 32; a++) {
                                    SPI1BUF = 0x0;  //reset
                                    __delay_us(1);
                                    for (b = 1; b <= 32; b++) {
                                        if (b <= a) {               //turn on
                                            if (b == 32) {
                                                SPI1BUF = 0x80;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(200000);

                                            }
                                            else if (b%2 == 1) {             //is odd
                                                SPI1BUF = 0xFF;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(5);
                                            }
                                            else {
                                                SPI1BUF = 0x80;         // green
                                                __delay_us(5);
                                                SPI1BUF = 0xFF;         // red
                                                __delay_us(5);
                                                SPI1BUF = 0x80;         // blue
                                                __delay_us(5);
                                            }
                                        }
                                        else {                      //turn off
                                            SPI1BUF = 0x80;         // green
                                            __delay_us(5);
                                            SPI1BUF = 0x80;         // red
                                            __delay_us(5);
                                            SPI1BUF = 0x80;         // blue
                                            __delay_us(5);
                                        }
                                    }
                                }
                            }
                            ////////////////////////////////////////////////////////////////////////////////////////
                        }
                    }
                    else if (j%2 == 1) {             //is odd
                        SPI1BUF = 0xFF;         // green
                        __delay_us(200);
                        SPI1BUF = 0x80;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                    }
                    else {
                        SPI1BUF = 0x80;         // green
                        __delay_us(200);
                        SPI1BUF = 0xFF;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                    }
                }
                else {                      //turn off
                    SPI1BUF = 0x80;         // green
                    __delay_us(200);
                    SPI1BUF = 0x80;         // red
                    __delay_us(200);
                    SPI1BUF = 0x80;         // blue
                    __delay_us(200);
                }
            }
        }
        
        for (i = 32; i >= 1; i--) {
            SPI1BUF = 0x0;  //reset
            __delay_us(200);
            for (j = 1; j <= 32; j++) {
                if (j <= i) {               //turn on
                    if (j == 32) {
                        SPI1BUF = 0x9B;         // green
                        __delay_us(200);
                        SPI1BUF = 0xC0;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                    }
                    else if (j%2 == 1) {             //is odd
                        SPI1BUF = 0xFF;         // green
                        __delay_us(200);
                        SPI1BUF = 0x80;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                    }
                    else {
                        SPI1BUF = 0x80;         // green
                        __delay_us(200);
                        SPI1BUF = 0xFF;         // red
                        __delay_us(200);
                        SPI1BUF = 0x80;         // blue
                        __delay_us(200);
                    }
                }
                else {                      //turn off
                    SPI1BUF = 0x80;         // green
                    __delay_us(200);
                    SPI1BUF = 0x80;         // red
                    __delay_us(200);
                    SPI1BUF = 0x80;         // blue
                    __delay_us(200);
                }
            }
        }
    }
    
    return 0;
}
