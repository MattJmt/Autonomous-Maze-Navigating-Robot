#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b111; // need to work out prescaler to produce a timer tick corresponding to 1 deg angle change
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // it's a good idea to initialise the timer so that it initially overflows after 20 ms
    TMR0H=(65535-T_PERIOD)>>8;            
    TMR0L=(unsigned char)(65535-T_PERIOD); // casting to unsigned char here to suppress warning
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to write a full 16bit timer value
 * Note TMR0L and TMR0H must be written in the correct order, or TMR0H will not contain the correct value
************************************/
void write16bitTMR0val(unsigned int tmp)
{
    TMR0H=tmp>>8; //MSB written
    TMR0L=tmp; //LSB written and timer updated
}