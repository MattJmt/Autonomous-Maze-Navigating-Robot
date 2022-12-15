#ifndef _color_H
#define _color_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);

typedef struct RGB { //definition of DC_motor structure
    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int C;
} RGB;

void getColor(RGB *v);
void ambientCal(RGB *v);
void whiteCal(RGB *v);
unsigned int colorDetect (double clearRef, RGB *ambientRGBVal ,RGB *whiteRGBVal, DC_motor *mL, DC_motor *mR);
void return_home_turns(unsigned int *turn_history, unsigned int *counter_history, unsigned int index, DC_motor *mL, DC_motor *mR);
#endif