/* 
 * File:   main.c
 * Author: Steccas
 *
 * Created on 19 ottobre 2015, 9.23
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>

/*
 lat accende
 */
int main(int argc, char** argv) {
    TRISAbits.TRISA3 = 1; //bottoni
    TRISAbits.TRISA2 = 1;
    TRISCbits.TRISC2 = 1;
    TRISBbits.TRISB0 = 0; //led
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    int status=0;
    bool premuto1=false;
    bool premuto2=false;
    bool premuto3=false;
    for(;;){
        //LATBbits.LATB0 = PORTAbits.RA3;
        if(PORTAbits.RA3==0 && !premuto1) {
            LATBbits.LATB0 =! LATBbits.LATB0;
            premuto1=true;
        }
        if(PORTAbits.RA3==1 && premuto1) premuto1=false;
        if(PORTAbits.RA2==0 && !premuto2) {
            LATBbits.LATB1 =! LATBbits.LATB1;
            premuto2=true;
        }
        if(PORTAbits.RA2==1 && premuto2) premuto2=false;
        if(PORTCbits.RC2 == 0 && !premuto3){
            premuto3=true;
            switch(status){
                case 0:
                    LATBbits.LATB2 = 0;
                    LATBbits.LATB3 = 1;
                    LATBbits.LATB4 = 1;  
                    LATBbits.LATB5 = 1;
                    status++;
                    break;
                case 1:
                    LATBbits.LATB2 = 0;
                    LATBbits.LATB3 = 0;
                    LATBbits.LATB4 = 1;  
                    LATBbits.LATB5 = 1;
                    status++;
                    break;
                case 2:
                    LATBbits.LATB2 = 0;
                    LATBbits.LATB3 = 0;
                    LATBbits.LATB4 = 0;  
                    LATBbits.LATB5 = 1;
                    status++;
                    break;
                case 3:
                    LATBbits.LATB2 = 0;
                    LATBbits.LATB3 = 0;
                    LATBbits.LATB4 = 0;  
                    LATBbits.LATB5 = 0;
                    status=0;
                    break;   
            }
        }
        if(PORTCbits.RC2 == 1 && premuto3) premuto3=false;
    }
    return (EXIT_SUCCESS);
}

