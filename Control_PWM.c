/* 
 * File:   Control_PWM.c
 * Author: kevin
 *
 * Created on 11 de enero de 2023, 10:46
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <pic16f873a.h>
#include <math.h>
#define XTAL_FREQ 4000000

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include "lcd.h"


//Prototipos
void Timer2_init(int);
void ADC_init(void);
int Read_ADC(void);
/*
 * 
 */
void main() {
    char buffer[30];
    int dc;
    unsigned int freq;
    int pr2=249;
    TRISCbits.TRISC2=0;
    Timer2_init(pr2);
    ADC_init();
    Lcd_Init();
    CCP1CONbits.CCP1M=0b1111; //Selecciono el modo PWM en registro CCP
    
    freq = 4000000/((pr2+1)*4);
    
    while(1){
        Lcd_Clear();
        unsigned int duty_cycle = Read_ADC();       //1000 = 100%
        dc = duty_cycle/10;
        if(duty_cycle>1000){
            dc = 100;
        }
        sprintf(buffer, "Freq= %d Hz", freq);
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String(buffer);
        sprintf(buffer, "Duty Cycle= %d%%", dc);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(buffer);
        CCP1CONbits.CCP1X = duty_cycle & (1<<0);    //Paso el bit 10 al registro
        CCP1CONbits.CCP1Y = duty_cycle & (1<<1);    //Paso el bit 9 al registro
        CCPR1L = duty_cycle>>2; //Recorro la variable 2 bits para obtener los 8 bits
        __delay_ms(100);
    }
}

void Timer2_init(int pr2){
    TMR2 = 0;
    PR2 = pr2;     //PWM Period  
    T2CONbits.T2CKPS = 0b00;  //Prescaler 1:1
    T2CONbits.TMR2ON = 1; //TMR2 on
}

void ADC_init(void){
    ADCON1bits.ADFM = 1;        // 1 derecha, 0 izquierda
    ADCON1bits.PCFG = 0b1110;   // AN0 analogico las demas digitales
    ADCON1bits.ADCS2 = 0;       // Seleccion de reloj para ADC
    ADCON0bits.ADCS1 = 0;       // debe ser mayor a 1.6us
    ADCON0bits.ADCS0 = 1;   
    ADCON0bits.ADON = 1;        //ADC on
}

int Read_ADC(){
    ADCON0bits.CHS=0b000;       //Canal analogico seleccionado
    __delay_us(25);             //Tiempo de adquisicion
    ADCON0bits.GO=1;            //ADC start
    while(ADCON0bits.GO_DONE);  //Espera termine la conversion
    return (int)((ADRESH<<8)+ADRESL);    //Retorna el valor del ADC
}


