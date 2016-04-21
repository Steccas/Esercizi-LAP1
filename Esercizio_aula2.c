/* 
 * File:   main.c
 * Author: Steccas
 *
 * Created on 28 ottobre 2015, 9.20
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>

/*
 * 
 */

void eroga(const int k){
   int timer=0;
   while (timer<k){
       unsigned int t;
       t = TMR0;
       if (t >= 31250) { // equivalent of 500 ms
           TMR0 = 0;
           timer++;
       }
    }
 }
    
int main(int argc, char** argv) {
    TRISAbits.TRISA3 = 1; //scelta caffè
    TRISAbits.TRISA2 = 1; //scelta macchiato
    TRISCbits.TRISC2 = 1; //scelta zucchero
    TRISBbits.TRISB0 = 0; //erogazione caffè
    TRISBbits.TRISB1 = 0; //erogazione latte
    TRISBbits.TRISB2 = 0; //erogazione zucchero
    TRISBbits.TRISB4 = 0; //led z1
    TRISBbits.TRISB5 = 0; //led z2
    
    T0CONbits.TMR0ON = 0; // stop the timer
    T0CONbits.T08BIT = 0; // timer configured as 16-bit
    T0CONbits.T0CS = 0; // use system clock
    T0CONbits.PSA = 0; // use prescaler
    T0CONbits.T0PS = 0b111; // prescaler 1:256 ('0b' is a prefix for binary)
    TMR0 = 0; // clear timer value
    T0CONbits.TMR0ON = 1; // start the timer
    
    const int caffe=20;
    const int latte=10;
    int statusZ=0;
    bool premuto=false;
    
    
    
    for (;;){
        if(PORTCbits.RC2 == 0 && !premuto){
            premuto=true;
            switch(statusZ){
                case 0:
                    LATBbits.LATB4 = 0;  
                    statusZ++;
                    break;
                case 1:
                    LATBbits.LATB5 = 0;  
                    statusZ++;
                    break;
                case 2:
                    LATBbits.LATB4 = 1;
                    LATBbits.LATB5 = 1;
                    statusZ=0;
                    break;
            }
        }
        if(PORTCbits.RC2 == 1 && premuto) premuto=false;
        if(PORTAbits.RA3 == 0 && !premuto){
            premuto=true;    
            LATBbits.LATB2 =! LATBbits.LATB2;
                eroga(statusZ);
                LATBbits.LATB2 =! LATBbits.LATB2;
                LATBbits.LATB0 =! LATBbits.LATB0;
                eroga(caffe);
                //aspetta 10 secondi,spegni il led e metti premuto a falso
                LATBbits.LATB0 =! LATBbits.LATB0;
                premuto=false;
        }
        
        if(PORTAbits.RA2 == 0 && !premuto){
                premuto=true;
                LATBbits.LATB2 =! LATBbits.LATB2;
                eroga(statusZ);
                LATBbits.LATB2 =! LATBbits.LATB2;
                LATBbits.LATB0 =! LATBbits.LATB0;
                //aspetta 10 secondi, spegni il led e metti premuto a falso
                eroga(caffe);
                LATBbits.LATB0 =! LATBbits.LATB0;
                LATBbits.LATB1 =! LATBbits.LATB1;
                //aspetta 5 sec...
                eroga(latte);
                LATBbits.LATB1 =! LATBbits.LATB1;
                premuto=false;
        }
    }
    return (EXIT_SUCCESS);
}