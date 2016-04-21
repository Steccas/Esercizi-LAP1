/* 
 * File:   main.c
 * Author: op
 * Created on 30 novembre 2015, 9.18
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <xc.h>
#include <ctype.h>

/*
 * 
 */
int stato=0; //0=disattivato 1=pre/attivato 2=preallarme 3=allarme 4=setup

char psw[5]="1234\0";

void init(){
    TRISAbits.TRISA3 = 1; //puls anttivazione
    TRISAbits.TRISA2 = 1; //p disattivazione
    TRISCbits.TRISC2 = 1; //pulsate sensore intrusione
    TRISBbits.TRISB0 = 0; //LED Disattivazione
    TRISBbits.TRISB1 = 0; //LED Attivazione
    TRISBbits.TRISB2 = 0; //LED Intrusione
    TRISBbits.TRISB3 = 0; //led uscita/ingresso
    TRISBbits.TRISB4 = 0; //led sirena
    TRISBbits.TRISB5 = 0; //led debug
    
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
    TMR0 = -12500; // clear timer value
    T0CONbits.TMR0ON = 1; // start the timer
    
    //interrupt
    INTCONbits.T0IF = 0; // reset timer interrupt flag 
    INTCONbits.T0IE = 1; // enable timer interrupts
    RCONbits.IPEN = 0; // do not use priorities 
    INTCONbits.PEIE = 1; // enable peripheral interrupts 
    INTCONbits.GIE = 1; // enable interrupts globally
    
    PIE1bits.TX1IE = 0; // disable TX hardware interrupt
    PIE1bits.RC1IE = 0; // disable RX hardware interrupt
}

char readChar() 
{ 
    while (PIR1bits.RC1IF == 0) 
    { // wait for char 
        if (RCSTA1bits.OERR == 1) 
        {
            RCSTA1bits.OERR = 0; // clear overrun if it occurs 
            RCSTA1bits.CREN = 0; 
            RCSTA1bits.CREN = 1; 
        } 
    } 
    return RCREG1; 
}

char tempPW[5];

void putch(char c) {
    while (TXSTA1bits.TRMT == 0) {};
    TXREG = c;
}

void read_line(char * s, int max_len)
{
    int i = 0;
    for(;;) {
        char c = readChar();
        if (c == 13) {
            putchar(c);
            putchar(10);
            s[i] = 0;
            return;
        }
        else if (c == 8) {
            if (i > 0) {
                putchar(c);
                putchar(' ');
                putchar(c);
                --i;
            }
        }
        else if (c >= 32) {
            if (i < max_len) {
                putchar(c);
                s[i] = c;
                ++i;
            }
        }
    }
}

void inserisciPW(){
    printf("Insert Password... \n\r");
    for (int i=0; i<4; i++){
        tempPW[i]=readChar();
    }
    tempPW[4]='\0';
}

inline bool verificaPW(){
    return !strcmp(psw, tempPW);
}

void setupStart(){
    printf("Press Return to continue or insert ! for SETUP...\n\r")
    if(readChar()=='!') stato=4;
    return;
}

void timer(){
    int timer = 0;
    while (timer < 25) {
        unsigned int t;
        t = TMR0;
        if (t >= 0) { // equivalent of 200 ms
            TMR0 = -12500;
            PORTBbits.RB3 = !PORTBbits.RB3;
            timer++;
        }  
    }
    PORTBbits.RB3 = 1;
}

int timerS5 = 0;
int timerS2 = 0;

bool cicloPW(){
    for (int tentativi=0; tentativi < 3; tentativi++){
        inserisciPW();
        if (verificaPW()){
            return true;
       } 
    }
    return false;
}

void interrupt isr(){
    if (INTCONbits.T0IF == 1) { 
        TMR0 = -12500; // reload timer value
        if (stato == 5){
            PORTBbits.RB3 = !PORTBbits.RB3;
            timerS5++;
            if(timerS5 > 25) {
                stato = 1;
                PORTBbits.RB3 = 1;
                }
        }
        if (stato == 2) { 
            PORTBbits.RB3 = !PORTBbits.RB3;
            timerS2++;
            if(timerS2 > 100) {
                PORTBbits.RB3 = 1;
                stato=3;
            }
        }
        INTCONbits.T0IF = 0;
    }
}

bool digitsCheck(char* s){
    int elem=(sizeof(s)/sizeof(char))
    for (int i=0; i<elem; i++){
        if (!isdigit(s[i])) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    init();
    LATBbits.LATB5 = 0;
    bool premuto=false;
    while(true){
        switch (stato){
            case 0: //disattivato
                LATBbits.LATB1 = 1;
                printf("UnEngaged\n\r");
                while(stato==0){
                    if (PORTAbits.RA3 == 0 && !premuto){
                        premuto=true;
                        inserisciPW();
                        if (verificaPW()) {
                            setupStart();
                            if(stato==0) stato=5;
                        }
                        premuto = false;
                    }
                }
                break;
            case 1: //attivato
                //timer(); stato5
                printf("Engaged\n\r");
                LATBbits.LATB1 = 0;
                while(stato==1){
                    if (PORTAbits.RA2 == 0 && !premuto){
                        premuto=true;
                        if(!cicloPW()){
                            stato=3;
                        }
                        else{stato= 0;}
                        premuto = false;
                    }
                    if (PORTCbits.RC2 == 0 && !premuto){
                        premuto=true;
                        stato=2;
                        premuto=false;
                    }
                }
                break;
            case 2: //preallarme
                printf("preAlert\n\r");
                while(stato==2){
                    if (cicloPW()) stato=0;
                    else stato=3;
                }
            case 3: //allarme
                printf("Alert!\n\r");
                LATBbits.LATB4 = 0;
                while (stato==3){
                    inserisciPW();
                    if (verificaPW())stato=0;
                }
                LATBbits.LATB4 = 1;
                break;
            case 4:
                printf("SetupMode\n\r");
                char line[81];
                int num_of_tokens;
                char *tokens[MAX_TOKENS];
                while (status==4){
                    printf("> ");
                    read_line(line, 80);
                    num_of_tokens = 0;
                    tokens[num_of_tokens] = strtok(line, " ");
                    while (tokens[num_of_tokens] != NULL) {
                        ++num_of_tokens;
                        tokens[num_of_tokens] = strtok(NULL, " ");
                    }
            
                    if (num_of_tokens == 0) continue;
                    if(!strcmp(tokens[0], "PASSWORD")){
                        if(num_of_tokens>=2){
                            if(sizeof(tokens[1])/sizeof(char)==4){
                                if(digitsCheck()) {
                                    strcpy(psw, tokens[1]);
                                    psw[4]="\0";
                                    printf("New password saved\n\r");
                                }
                                else printf("These arent digits!\n\r");
                            }
                            else printf("Password must be composed by 4 digits!\n\r");
                        }
                        else printf("PASSWORD: Missing new password!\n\r");
                    }
                    if(!strcmp(tokens[0], "EXIT")) stato=0;
                }
            case 5:
                printf("PreActivation\n\r");
        }
    }
    return (EXIT_SUCCESS);
}