#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
//15625

#define MSPEED_L 2
#define MSPEED_H 17

#define DSPEED_L 2
#define DSPEED_H 13

void init();
void switcher();
void putch(char);
void setup();
void timer (int, char);
void timerP (int, char);
void timerMu (int, char);
void timerMd (int, char);
void move();
void keyboard();
void led(int);

char piano;
char pianoD;

char mSpeed;
char dSpeed;

int l=4;

bool d=false;

bool premuto1=false;
bool premuto2=false;
bool premuto3=false;

int main(int argc, char** argv) {
    init();
    while(true){
        switcher();
    }
    return (EXIT_SUCCESS);
}

void switcher(){
    switch(piano){
        case 0:
            l=4;
            led(l);
            keyboard();
            setup();
            if (piano != pianoD) move();
            break;
        case 1:
            l=2;
            led(l);
            keyboard();
            setup();
            if (piano != pianoD) move();
            break;
        case 2:
            l=0;
            led(l);
            keyboard();
            setup();
            if (piano != pianoD) move();
            break;
    }
}

void keyboard(){
    if(PORTAbits.RA3 == 0 && !premuto1) {
        premuto1=true;
        pianoD=2;
    }
    if(PORTAbits.RA3 == 1 && premuto1) premuto1=false;
    if(PORTAbits.RA2 == 0 && !premuto2) {
        premuto2=true;
        pianoD=1;
    }
    if(PORTAbits.RA2 == 1 && premuto2) premuto2=false;
    if(PORTCbits.RC2 == 0 && !premuto3){
        premuto3=true;
        pianoD=0;
    }
    if(PORTCbits.RC2 == 1 && premuto3) premuto3=false;
}

void move(){
    timer(15625, dSpeed);
    printf("Chiusura porta\n\r");
    timerP(15625, 6);
    LATBbits.LATB5 = 0;
    d=true;
    if(piano < pianoD){
        while(piano != pianoD){
            timerMu(15625, mSpeed);
            piano++;
            printf("Piano: %d\n\r", piano);
            l--;
            led(l);
        }
    }
    if(piano > pianoD){
        while(piano != pianoD){
            timerMd(15625, mSpeed);
            piano--;
            printf("Piano: %d\n\r", piano);
            l++;
            led(l);
        }
    }
    printf("apertura porta\n\r");
    timerP(15625, 6);
    LATBbits.LATB5 = 1;
    d=false;
}

void setup(){
    ADCON0bits.CHS = !ADCON0bits.CHS;
    char dSpeedO=dSpeed;       
    char mSpeedO=mSpeed; 
    if (ADCON0bits.GODONE == 0) { // conversion complete
        if(ADCON0bits.CHS == 0) {
            dSpeed = DSPEED_L + ADRES * ((float)(DSPEED_H - DSPEED_L) / 1024);
            if(dSpeedO != dSpeed) printf("dS: %d\n\r", dSpeed);
        }
        if(ADCON0bits.CHS == 1) {
            mSpeed = MSPEED_L + ADRES * ((float)(MSPEED_H - MSPEED_L) / 1024);
            if(mSpeedO != mSpeed) printf("mS: %d\n\r", mSpeed);
        }
        ADCON0bits.GODONE = 1; 
    }
}

void led(int l){
    switch (l){
        case 0:
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB4 = 1;
            if (!d) LATBbits.LATB5 = 1;
            break;
        case 1:
            LATBbits.LATB0 = 1;
            LATBbits.LATB1 = 0;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB4 = 1;
            if (!d) LATBbits.LATB5 = 1;
            break;
        case 2:
            LATBbits.LATB0 = 1;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 0;
            LATBbits.LATB3 = 1;
            LATBbits.LATB4 = 1;
            if (!d) LATBbits.LATB5 = 1;
            break;
        case 3:
            LATBbits.LATB0 = 1;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 0;
            LATBbits.LATB4 = 1;
            if (!d) LATBbits.LATB5 = 1;
            break;
        case 4:
            LATBbits.LATB0 = 1;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB4 = 0;
            if (!d) LATBbits.LATB5 = 1;
            break;
        case 5:
            LATBbits.LATB0 = 1;
            LATBbits.LATB1 = 1;
            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB4 = 1;
            LATBbits.LATB5 = 0;
            break;
    }
}

void timer(int count, char cicle){
    char c=0;
    printf("timer\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            
            TMR0 = 0;
            c++;
        }
    }
}

void timerP(int count, char cicle){
    char c=0;
    printf("timerP\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            TMR0 = 0;
            c++;
            LATBbits.LATB5 = !LATBbits.LATB5;
        }
    }
}

void timerMu(int count, char cicle){
    char c=0;
    bool t=false;
    printf("timerMu\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (c==cicle/2 && t==false) {
            l--;
            printf("transizione\n\r");
            led(l);
            t=true;
        }
        if (t >= count) {
            TMR0 = 0;
            c++;
        }
    }
}

void timerMd(int count, char cicle){
    char c=0;
    bool t=false;
    printf("timerMd\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (c==cicle/2 && t==false) {
            l++;
            led(l);
            t=true;
        }
        if (t >= count) {
            TMR0 = 0;
            c++;
        }
    }
}

void init(){
    piano=0;
    pianoD=0;
    
    mSpeed=0;
    dSpeed=0;
    
    //bottoni
    TRISAbits.TRISA3 = 1; // piano 2
    TRISAbits.TRISA2 = 1; // 1
    TRISCbits.TRISC2 = 1; // 0
    
    //LED
    TRISBbits.TRISB0 = 0; // piano 2
    TRISBbits.TRISB1 = 0; // fra 2 e 1
    TRISBbits.TRISB2 = 0; // 1
    TRISBbits.TRISB3 = 0; // 1 e 0
    TRISBbits.TRISB4 = 0; // 0
    TRISBbits.TRISB5 = 0; // porte chiuse
    
    // setup UART 
    TRISCbits.TRISC6 = 0; // TX as output 
    TRISCbits.TRISC7 = 1; // RX as input
    
    TXSTA1bits.SYNC = 0; // Async operation 
    TXSTA1bits.TX9 = 0; // No tx of 9th bit 
    TXSTA1bits.TXEN = 1; // Enable transmitter
            
    RCSTA1bits.RX9 = 0; // No rx of 9th bit
    RCSTA1bits.CREN = 1; // Enable receiver
    RCSTA1bits.SPEN = 1; // Enable serial port
    
    // Setting for 19200 BPS 
    BAUDCON1bits.BRG16 = 0; // Divisor at 8 bit 
    TXSTA1bits.BRGH = 0; // No high-speed baudrate 
    SPBRG1 = 51; // divisor value for 19200
    
    //timer0
    T0CONbits.TMR0ON = 0; // stop the timer
    T0CONbits.T08BIT = 0; // timer configured as 16-bit
    T0CONbits.T0CS = 0; // use system clock
    T0CONbits.PSA = 0; // use prescaler
    T0CONbits.T0PS = 0b111; // prescaler 1:256 ('0b' is a prefix for binary)
    TMR0 = 0; // clear timer value (ms*1000/16)
    T0CONbits.TMR0ON = 1; // start the timer
    
    //input analogico
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

void putch(char c) { 
    // wait the end of transmission 
    while (TXSTA1bits.TRMT == 0) {}; 
    TXREG1 = c; // send the new byte 
}