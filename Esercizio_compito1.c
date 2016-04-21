#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>
#include <string.h>

#define CICLE_X_LOW 7
#define CICLE_X_HIGH 14

bool status, premuto;
char cicleX, cicleY, cicleZ; //controlare cicli con potenziometro e uart
int countX, countYZ;
int ms;

void init();
void accensione();
void timer(int, char);
void timerDual(int, char, char);
void switcher();
void putch(char);
int nb_read_char(char*);

void putch(char c) { 
    // wait the end of transmission 
    while (TXSTA1bits.TRMT == 0) {}; 
    TXREG1 = c; // send the new byte 
}

int main(int argc, char** argv) {
    init();
    while(true){
        accensione();
        if (status == 1) switcher();
    }
    return (EXIT_SUCCESS);
}

void accensione(){
    //accendi spegni
    //printf("test\n\r");
    if(PORTAbits.RA3==0 && !premuto) {
        printf("accendospengo\n\r");
            LATBbits.LATB0 = !LATBbits.LATB0;
            status = !status;
            premuto=true;
    }
    if (PORTAbits.RA3==1 && premuto) premuto=false;
    //uart
    char c;
    if(nb_read_char(&c) && c=='+'){
        ms += 100;
        cicleY++;
        printf ("New Y value: %d\n\r", ms);
    }
    if(nb_read_char(&c) && c=='-'){
        if(ms>0){
            ms -= 100;
        cicleY--;
        printf ("New Y value: %d\n\r", ms);
        }
        else printf ("Sei già a 0ms!\n\r");
    }
}

void switcher(){
    printf("switcher\n\r");
    timer(countX, cicleX);
    timerDual(countYZ, cicleY, cicleZ);
    LATBbits.LATB0 = 1;
    LATBbits.LATB1 = 1;
    status = false;
}

void timer(int count, char cicle){
    char c=0;
    printf("timer1\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            if (ADCON0bits.GODONE == 0) { // conversion completed 
                cicleX = CICLE_X_LOW + ADRES * ((float)(CICLE_X_HIGH - CICLE_X_LOW) / 1024); // retrigger conversion
                printf("%d", cicleX);
                ADCON0bits.GODONE = 1; 
            }
            TMR0 = 0;
            c++;
        }
        if(PORTAbits.RA2==0 && !premuto) {
            c=0;
            printf("reset c\n\r");
        }
        if(PORTAbits.RA2==1 && premuto) premuto=false;
    }
}

void timerDual(int count, char cicle1, char cicle2){
    char c1=0;
    char c2=0;
    printf("timer2\n\r");
    while(c1 < cicle1){
        unsigned int t;
        t = TMR0;        
        if (t > count){
            TMR0 = 0;
            c1++;
            c2++;
            if(c2==cicle2){
                LATBbits.LATB1 = !LATBbits.LATB1;
                c2=0;
            }
        }
    }
}

void init(){
    //X = 10sec
    countX = 62500; //1000ms
    cicleX = 10;
    //Y = 3,3 sec
    countYZ = 6250; //100ms
    cicleY = 33;
    cicleZ = 5;
    
    ms=3300;
    
    status = false;
    premuto=false;
    
    //bottoni
    TRISAbits.TRISA3 = 1; //accensione/spegnimento
    TRISAbits.TRISA2 = 1; //fotocellula
    
    //LED
    TRISBbits.TRISB0 = 0; //interruttore
    TRISBbits.TRISB1 = 0; //warning
    TRISBbits.TRISB2 = 0;
    
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

int nb_read_char(char * c) //non bloccante
{
    if (RCSTA1bits.OERR == 1) {
        RCSTA1bits.OERR = 0;
        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;
    }
    if (PIR1bits.RC1IF == 1) {
        *c = RCREG1;
        return 1;
    }
    else
        return 0;
}