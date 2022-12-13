#include <xc.h>
#include "interrupts.h"
#include "timers.h"
/************************************
 * Function to turn on interrupts and set if priority is used
 * individual interrupt sources are enabled in their respective module init functions
************************************/
void Interrupts_init(void)
{
    PIE0bits.TMR0IE=1;  //interrupt on timer0 overflow (or match in 8bit mode)
    INTCONbits.PEIE=1;  //peripheral interrupts enabled (controls anything in PIE1+)
    INTCONbits.IPEN=0;  //high priority only
    INTCONbits.GIE=1;   //global interrupt enabled
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF)
    {
        if(LATCbits.LATC5){ //if output pin currently high
            write16bitTMR0val(65535-off_period); //set new off_period
            LATCbits.LATC5=0; //turn your output pin off here
        } else {
            write16bitTMR0val(65535-on_period);  //set new on_period
            LATCbits.LATC5=1; //turn your output pin on here
        }
    }
    PIR0bits.TMR0IF=0; 
}