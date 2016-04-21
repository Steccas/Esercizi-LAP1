/* 
 * File:   main.c
 * Author: Steccas
 *
 * Created on 4 novembre 2015, 9.18
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>

/*
 
*/

#define PERIOD_LOW 312 
#define PERIOD_HIGH 31250

void inizializza();
void lampeggia();
void accensione();
bool potenza();
void timer();
void switcher();

int count = 0;
int c = 10;

int status = 4;
int statusStore = 0;

int luce = 0;
int luceStore = 0;

bool premutoA3 = false;
bool premutoC2 = false;
bool premutoA2 = false;

bool acceso = 0;

bool timer5 = false;

int period = PERIOD_HIGH;

int main(int argc, char** argv) {
    inizializza();

    for (;;) {
        accensione();
        potenza();
        lampeggia();
    }
    return (EXIT_SUCCESS);
}

void inizializza() {
    TRISAbits.TRISA3 = 1; //accensione
    TRISAbits.TRISA2 = 1; //power
    TRISCbits.TRISC2 = 1; //ciclo a/s
    
    TRISBbits.TRISB0 = 0; //ciclo led
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0; //debug
    TRISBbits.TRISB5 = 0; //led accensione

    T0CONbits.TMR0ON = 0; // stop the timer
    T0CONbits.T08BIT = 0; // timer configured as 16-bit
    T0CONbits.T0CS = 0; // use system clock
    T0CONbits.PSA = 0; // use prescaler
    T0CONbits.T0PS = 0b111; // prescaler 1:256 ('0b' is a prefix for binary)
	INTCONbits.T0IF = 0; // reset timer interrupt 
    INTCONbits.T0IE = 0; // no interrupts timer interrupts
    T0CONbits.TMR0ON = 1; // start the timer
	
	TMR0 = 0; // clear timer value
	
	ANSELAbits.ANSA0 = 1; // RA0 = analog input --> AN0
	ADCON1bits.PVCFG = 0b00; // Positive reference = +VDD 
    ADCON1bits.NVCFG = 0b00; // Negative reference = GND
	ADCON2bits.ADFM = 1; // format = right justified 
    ADCON2bits.ACQT = 0b111; // acquisition time = 20*TAD 
    ADCON2bits.ADCS = 0b110; // conversion clock = FOSC/64
	ADCON0bits.ADON = 1; // turn on ADC
    
    ADCON0bits.CHS = 0; // select channel 0 (AN0) 
    ADCON0bits.GODONE = 1;
}

void timer() {
    if (!timer5) {
        //c = 10;
        //eq = 12500;
        LATBbits.LATB4 = 1;
    } else {
        //c = 4;
        //eq = 12500;
        luce = 25;
        LATBbits.LATB4 = 0;
    }
    
    int timer = 0;
    int timerStore = 0;
    
    while (timer < luce) {
        unsigned int t;
        t = TMR0;
        bool premuto=potenza();
        if (t >= period) { // equivalent of 200/500 ms
           // LATBbits.LATB4 = !LATBbits.LATB4;
			if (ADCON0bits.GODONE == 0) { // conversion completed 
                period = PERIOD_LOW + ADRES * ((float)(PERIOD_HIGH - PERIOD_LOW) / 1024); // retrigger conversion 
                ADCON0bits.GODONE = 1; 
            }
			TMR0 = 0;
            //timerStore=timer;
            timer++;
            if(premuto) count++;
        }  
        accensione();
        if (count>=c) timer=luce;
    }
}

void lampeggia() {
    while (acceso) {
        if (count>=c){
            
            if(timer5) {luce=luceStore; status=statusStore;}
            else {luceStore=luce; statusStore=status;}
            timer5 = !timer5;
            count=0;
        }
        timer();
        LATBbits.LATB5 = !LATBbits.LATB5;
    }
    if (!acceso) LATBbits.LATB5 = 1;
}

void accensione() {
    if (PORTAbits.RA3 == 0 && !premutoA3 && luce != 0) { //accensione     
        premutoA3 = true;
        if (!acceso) {
            acceso = true;
        } else {
            acceso = false;
        }
        premutoA3 = false;
        count=0;
    }
}

void switcher() {
    switch (status) {
        case 4:
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            status--;
            if (luce != 25) luce = 4;
            break;
        case 3:
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 0;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            status--;
            if (luce != 25) luce = 3;
            break;
        case 2:
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 0;
            LATBbits.LATB2 = 0;
            LATBbits.LATB3 = 1;
            status--;
            if (luce != 25) luce = 2;
            break;
        case 1:
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 0;
            LATBbits.LATB2 = 0;
            LATBbits.LATB3 = 0;
            status = 4;
            if (luce != 25) luce = 1;
            break;
    }
}

bool potenza() {
    if (PORTAbits.RA2 == 0 && !premutoA2) {
        premutoA2 = true;
        if(luce != 25) switcher();
    }
    if (PORTAbits.RA2 == 1 && premutoA2) premutoA2 = false;
    if (PORTCbits.RC2 == 0 /*&& !premutoC2*/) {
        premutoC2 = true;
        //count++; lo faccio nel timer
        return true;
    }
    if (PORTCbits.RC2 == 1 /*&& premutoC2*/) {
        premutoC2 = false;
        count=0;
    }
    return false;
    /*if (premutoC2) return true;
    else return false;*/
}