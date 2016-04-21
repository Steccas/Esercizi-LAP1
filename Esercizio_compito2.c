#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define CICLE_T_LOW 1
#define CICLE_T_HIGH 6

#define SPEED_LOW 0
#define SPEED_HIGH 161

char status; //1: terg; 2:terg. t; 3: cc;
bool premuto1;
bool premuto2;
bool premuto3;

bool isStatusChanged;

char cicleT;
char speed;
char f;

void init();
void switcher();
void buttons();
void putch(char);
void timerT(int, char);
void timerCC(int, char);
void setToff();
void setSpeed();

int main(int argc, char** argv) {
    init();
    while(true){
        switcher();
    }
    return (EXIT_SUCCESS);
}

void buttons(){
    if(PORTAbits.RA3==0 && !premuto1) {
        premuto1=true;
        if(status!=1) {status=1;} //terg continuo
        else {status=0;}
        isStatusChanged=true;
    }
    if(PORTAbits.RA3==1 && premuto1) premuto1=false;
    if(PORTAbits.RA2==0 && !premuto2) {
        premuto2=true;
        if(status!=2) {status=2;} //terg temp
        else {status=0;}
        isStatusChanged=true;
    }
    if(PORTAbits.RA2==1 && premuto2) premuto2=false;
    if(PORTCbits.RC2==0 && !premuto3) {
        premuto3=true;
        status=3; //cc
        isStatusChanged=true;
    }
    if(PORTCbits.RC2==1 && premuto3) premuto3=false;
}

void setToff(){
    ADCON0bits.CHS = 0;
    if (ADCON0bits.GODONE == 0) { // conversion completed 
        cicleT = CICLE_T_LOW + ADRES * ((float)(CICLE_T_HIGH - CICLE_T_LOW) / 1024); // retrigger conversion
        printf("%d\n\r", cicleT);
        ADCON0bits.GODONE = 1; 
    }
}

void setSpeed(){
    ADCON0bits.CHS = 1;
    if (ADCON0bits.GODONE == 0) { // conversion completed 
        speed = SPEED_LOW + ADRES * ((float)(SPEED_HIGH - SPEED_LOW) / 1024); // retrigger conversion
        printf("%d\n\r", speed);
        ADCON0bits.GODONE = 1; 
    }
    f=(float)(120-speed)*0.1 + 5;
    printf("%d\n\r", f);
}

void switcher(){
    switch(status){
        case 0:
            LATB = 0xff;
            isStatusChanged=false;
            buttons();
            break;
        case 1: 
            LATBbits.LATB4 = 1;
            LATBbits.LATB2 = 1;
            isStatusChanged=false;
            LATBbits.LATB0 = 0;
            timerT(31250, 1);
            LATBbits.LATB0 = 1;
            timerT(31250, 2);
            break;
        case 2:
            LATBbits.LATB4 = 1;
            LATBbits.LATB2 = 1;
            isStatusChanged=false;
            setToff();
            LATBbits.LATB0 = 0;
            timerT(31250, 1);
            LATBbits.LATB0 = 1;
            timerT(31250, cicleT);
            break;
        case 3:
            LATBbits.LATB0 = 1;
            isStatusChanged=false;
            setSpeed();
            LATBbits.LATB4 = 0;
            LATBbits.LATB2 = !LATBbits.LATB2;
            timerCC(6250,f);
            break;
    }
}

void timerCC(int count, char cicle){
    char c=0;
    printf("timerCC\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        buttons();
        setSpeed();
        if(isStatusChanged) break;
        if (t >= count) {
            TMR0 = 0;
            c++;
        }
    }
}

void timerT(int count, char cicle){
    char c=0;
    printf("timerT\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        buttons();
        setToff();
        if(isStatusChanged) break;
        if (t >= count) {
            TMR0 = 0;
            c++;
        }
    }
}

void init(){
    //variabili
    status=0;
    premuto1=false;
    premuto2=false;
    premuto3=false;

    isStatusChanged=false;
    
    cicleT=2;
    speed=120;
    f=(float)(120-speed)*0.1 + 5;
    
    //bottoni
    TRISAbits.TRISA3 = 1;   //tergicristallo
    TRISAbits.TRISA2 = 1;   //terg. temp.
    TRISCbits.TRISC2 = 1;   //cc
    
    //LED
    TRISBbits.TRISB0 = 0;   //m. tergicristallo
    TRISBbits.TRISB1 = 0;   //LED DEBUG1
    TRISBbits.TRISB2 = 0;   //accerelatore
    TRISBbits.TRISB3 = 0;   //LED DEBUG2
    TRISBbits.TRISB4 = 0;   //setup cc
    TRISBbits.TRISB5 = 0;   //LED DEBUG3
    
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
    ANSELAbits.ANSA0 = 1; // RA0 = analog input --> AN1
    ANSELAbits.ANSA1 = 1; // RA1 = analog input --> AN1
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