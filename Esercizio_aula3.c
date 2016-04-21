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
    TMR0 = 0; // clear timer value
    T0CONbits.TMR0ON = 1; // start the timer
}

void timer() {
    int eq = 12500;
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
        if (t >= eq) { // equivalent of 200/500 ms
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
        acceso = !acceso;
        /*if (!acceso) {
            acceso = true;
        } else {
            acceso = false;
        }*/
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