void init(){
    //bottoni
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA2 = 1;
    TRISCbits.TRISC2 = 1;
    
    //LED
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
    
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
    
    INTCONbits.T0IF = 0; // reset timer interrupt flag 
    INTCONbits.T0IE = 1; // enable timer interrupts
    
    //timer2
    T2CONbits.TMR2ON = 0; // stop the timer
    T2CONbits.T2CKPS = 0b10; // 1:16 prescaler
    T2CONbits.T2OUTPS = 0b0000; // no postscaler
    TMR2 = 0; // reset timer
    PR2 = 250; // load period register (0,25ms)
    PIR1bits.TMR2IF = 0; // reset interrupt flag
    T2CONbits.TMR2ON = 1; // start the timer
    
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
    
    //interrupt
    RCONbits.IPEN = 0; // do not use priorities 
    INTCONbits.PEIE = 1; // enable peripheral interrupts 
    INTCONbits.GIE = 1; // enable interrupts globally
    
    PIE1bits.TX1IE = 0; // disable TX hardware interrupt
    PIE1bits.RC1IE = 0; // disable RX hardware interrupt
}

void timer(int count, char cicle){
    char c=0;
    printf("timer1\n\r");
    while (c<cicle){
        unsigned int t;
        t = TMR0;
        if (t >= count) {
            TMR0 = 0;
            c++;
        }
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
                //cose da fare se finisce c2
                c2=0;
            }
        }
    }
}

void timerDualCount(int count, int count2, char cicle1){
    char c1=0;
    printf("timer2\n\r");
    while(c1 < cicle1){
        unsigned int t;
        t = TMR0;        
        if (t > count){
            TMR0 = 0;
            c1++;
        }
    }
}

void pulsantiera(){
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

int verify_password(void)
{
    int i;
    char pwd[5];
    printf("PASSWORD:");

    for (i = 0; i < 4;i++) {
        char c;
        do {
            c = read_char();
        }
        while (!isdigit(c));
        putchar('*');
        pwd[i] = c;
    }
    pwd[4] = 0;
    if (strcmp(pwd, current_password) == 0) {
        printf("-OK\n\r");
        return PASSWORD_OK;
    }
    else {
        printf("-FAIL!\n\r");
        return PASSWORD_BAD;
    }
}

void variatorePortAnalogica(){
   if (ADCON0bits.GODONE == 0) { // conversion completed 
        period = PERIOD_LOW + ADRES * ((float)(PERIOD_HIGH - PERIOD_LOW) / 1024); // retrigger conversion 
        ADCON0bits.GODONE = 1; 
   } 
}