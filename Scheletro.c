#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

void init();
void accensione();
void switcher();
void putch(char);
char read_char(void);
int nb_read_char(char*);
void read_line(char*, int);

int main(int argc, char** argv) {
    init();
    while(true){
        accensione();
    }
    return (EXIT_SUCCESS);
}

void accensione(){
    
}

void switcher(){

}

void init(){
    
}

void putch(char c) { 
    // wait the end of transmission 
    while (TXSTA1bits.TRMT == 0) {}; 
    TXREG1 = c; // send the new byte 
}

char read_char(void)
{
    while (PIR1bits.RC1IF == 0) {
        if (RCSTA1bits.OERR == 1) {
            RCSTA1bits.OERR = 0;
            RCSTA1bits.CREN = 0;
            RCSTA1bits.CREN = 1;
        }
    }
    return RCREG1;
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

void read_line(char * s, int max_len)
{
    int i = 0;
    for(;;) {
        char c = read_char();
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