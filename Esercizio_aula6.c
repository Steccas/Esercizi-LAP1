#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>
#include <string.h>

#define PERIOD_LOW 0
#define PERIOD_HIGH 60

int hh,mm,ss;
int hhS, mmS;
char status; //0: normale; 1: setup orario;
bool isSEnabled;
bool premuto;
char cRA3;
char cRA2;

void init();
void timer(int, char);
void timerAlt(int, char);
void putch(char);
void switcher();
bool RA3isPressed();
bool RA2isPressed();
bool alarm;
void incrementa();
void setNewTime();
void setNewAlarm();
void checkAlarm();

int x;
bool hhChanged;
bool mmChanged;
bool hhSisChanged;
bool mmSisChanged;

int main(int argc, char** argv) {
    init();
    printf("<<<<<<<<<<<<<<SteccasClock!>>>>>>>>>>>>>>>\n\r");
    while(true){
        switcher();
    }
    return (EXIT_SUCCESS);
}

void incrementa(){
    ss++;
    if (ss==60) {mm++; ss=0;}
    if (mm==60) {hh++; mm=0;}
    if (hh==24) {hh=0;}
}

void setNewTime(){
    ADCON0bits.CHS = !ADCON0bits.CHS;
            
    while (ADCON0bits.GODONE == 1); // conversion complete
    if(ADCON0bits.CHS == 1) {
        hh = (ADRES * 24) / 1024; // retrigger conversion
        hhChanged=true;
    }
    if(ADCON0bits.CHS == 0) {
        mm = (ADRES * 60) / 1024; // retrigger conversion 
        mmChanged=true;
    }
    /*if (hhChanged){printf("New h: %d\n\r", hh); hhChanged=false;}
    if (mmChanged){printf("New m: %d\n\r", mm); mmChanged=false;}*/
    ADCON0bits.GODONE = 1; 
}

void setNewAlarm(){
    ADCON0bits.CHS = !ADCON0bits.CHS;
            
    while (ADCON0bits.GODONE == 1); // conversion complete
    if(ADCON0bits.CHS == 1) {
        hhS = (ADRES * 24) / 1024; // retrigger conversion
        hhSisChanged=true;
    }
    if(ADCON0bits.CHS == 0) {
        mmS = (ADRES * 60) / 1024; // retrigger conversion 
        mmSisChanged=true;
    }
    /*if (hhSisChanged){printf("New alarm h: %d\n\r", hhS); hhSisChanged=false;}
    if (mmSisChanged){printf("New alarm m: %d\n\r", mmS); mmSisChanged=false;}*/
    ADCON0bits.GODONE = 1; 
}

void checkAlarm(){
   if(hh==hhS && mm==mmS) status=3;
}

void switcher(){
    switch(status){
        case 0:
            timer(31250, 2);
            incrementa();
            printf("%d : %d : %d\n\r", hh, mm, ss);
            if (isSEnabled) checkAlarm();
            if(PORTCbits.RC2==0 && !premuto) {
                premuto=true;
                printf("Sveglia attiva\n\r");
                LATBbits.LATB2 = !LATBbits.LATB2;
                isSEnabled=!isSEnabled;
            }
            if(PORTCbits.RC2==1 && premuto) premuto=false;
            if(cRA3 >= 4) {status=1; cRA3=0; premuto=true;}
            if(cRA2 >= 4) {status=2; cRA2=0; premuto=true;}
            break;
        case 1:
            //printf("Setting clock...\n\r");
            timer(31250, 2);
            LATBbits.LATB0 = 0;
            
            setNewTime();
            
            printf("%d : %d\n\r", hh, mm);
            
            if(PORTAbits.RA3==0 && !premuto) {
                premuto = true;
                LATBbits.LATB0 = 1;
                status = 0;
            }
            if(PORTAbits.RA3==1 && premuto) premuto=false;
            break;
        case 2:
            timer(31250, 2);
            //printf("Setting alarm...\n\r");
            LATBbits.LATB1 = 0;
            
            setNewAlarm();
            printf("%d : %d\n\r", hhS, mmS);
            
            if(PORTAbits.RA2==0 && !premuto) {
                premuto=true;
                LATBbits.LATB1 = 1;
                status = 0;
            }
            if(PORTAbits.RA2==1 && premuto) premuto=false;
            break;
        case 3:
            LATBbits.LATB3 = 0;
            if(PORTCbits.RC2==0 && !premuto) {
                premuto=true;
                LATBbits.LATB3 = 1;
                LATBbits.LATB2 = !LATBbits.LATB2;
                isSEnabled=!isSEnabled;
                status = 0;
            }
            if(PORTCbits.RC2==1 && premuto) premuto=false;
    }
}

void timerAlt(int count, char cicle){
    char c=0;    
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            
            TMR0 = 0;
            c++;
        }
    }
}

void timer(int count, char cicle){
    char c=0;    
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            
            TMR0 = 0;
            c++;
            if(RA3isPressed()) cRA3++;
            if(RA2isPressed()) cRA2++;
        }
        if(cRA3 >= 4) {
            cRA3=0;
            premuto=true;
            status=1;
            break;
        }
        if(cRA2 >= 4) {
            cRA2=0;
            premuto=true;
            status=2;
            break;
        }
    }
}

bool RA3isPressed(){
    if(PORTAbits.RA3==0) {
        return true;
    }
    return false;
}

bool RA2isPressed(){
    if(PORTAbits.RA2==0) {
        return true;
    }
    return false;
}

void init(){
    hh=mm=ss=0;
    status = 0;
    cRA3=0;
    cRA2=0;
    
    x=0;
    hhChanged=false;
    mmChanged=false;
    
    hhSisChanged=false;
    mmSisChanged=false;
    
    hhS=mmS=1;
    isSEnabled=false;
    
    //bottoni
    TRISAbits.TRISA3 = 1; //impostazione orario
    TRISAbits.TRISA2 = 1; //impostazione sveglia
    TRISCbits.TRISC2 = 1; //snooze
    
    //LED
    TRISBbits.TRISB0 = 0; //setup orario
    TRISBbits.TRISB1 = 0; //setup sveglia
    TRISBbits.TRISB2 = 0; //sveglia attivata
    TRISBbits.TRISB3 = 0; //suono sveglia
    TRISBbits.TRISB4 = 0; //DEBUG1
    TRISBbits.TRISB5 = 0; //DEBUG2
    
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
    
    //timer
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