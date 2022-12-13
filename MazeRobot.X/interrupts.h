#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

unsigned int on_period,off_period;

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif