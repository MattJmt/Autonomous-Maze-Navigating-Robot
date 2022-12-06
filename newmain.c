// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#pragma once

#include <xc.h>
#include <stdio.h>
#include "i2c.h"
#include "dc_motor.h"
#include "serialTest.h"
#include "color.h"
//#include "interupts2.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    //initialisation functions
    color_click_init();
    initUSART4();
    initDCmotorsPWM(199);       //change this to a variable
    //Interrupts_init();
    
    
    
    // intialisation variables
    
    
    
    LATDbits.LATD7=0;   //set initial output state
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)   (output = 0)
    
    LATDbits.LATD4 = 0;     //iniitalise brake light
    TRISDbits.TRISD4 = 0;

    LATHbits.LATH0 = 0;     //iniitalise right indicator
    TRISHbits.TRISH0 = 0;
    
    LATFbits.LATF0 = 0;     //iniitalise left indicator
    TRISFbits.TRISF0 = 0;
    
    LATHbits.LATH1 = 0;     //iniitalise head light
    TRISHbits.TRISH1 = 0;
    
    LATDbits.LATD3 = 0;     //iniitalise m beam
    TRISDbits.TRISD3 = 0;
    
    
    LATDbits.LATD4 = 1;
    LATFbits.LATF0 = 1; 
    LATHbits.LATH0 = 1;
    LATFbits.LATF0 = 1; 
    LATDbits.LATD3 = 1;
    
    
    
    
    LATGbits.LATG0=0;   //set initial output state  RED
    LATEbits.LATE7=0;   //set initial output state  GREEN
    LATAbits.LATA3=0;   //set initial output state  BLUE
    
    TRISGbits.TRISG0=0; //set TRIS value for pin (output)   (output = 0)
    TRISEbits.TRISE7=0; //set TRIS value for pin (output)   (output = 0)
    TRISAbits.TRISA3=0; //set TRIS value for pin (output)   (output = 0)
    
    char testString[20];
    char string1[20];
    char string2[20];
    char string3[20];
    char string4[20];
    
    
    RGB RGBVal;
    RGBVal.R = 0;
    RGBVal.G = 0;
    RGBVal.B = 0;
    RGBVal.C = 0;
    double clearRef = 0.0;
    
    unsigned int ambientR = 30;
    unsigned int ambientG = 12;
    unsigned int ambientB = 10;
    float ambientC = 56;
    
    float whiteR = 68.0;
    float whiteG = 57.0;
    float whiteB = 54.0;
    float whiteC = 195.0;
    
    
    while(1){
        
        //add something here to calibrate ambient
        
        
        getColor(&RGBVal);
        LATDbits.LATD7=!LATDbits.LATD7;
        
        clearRef = RGBVal.C/whiteC;
        
        
        sprintf(string4,"C:  %f ",clearRef);
        TxBufferedString(string4);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        sprintf(string1,"Red: %f %d ",(RGBVal.R-ambientR)/((whiteR-(float)ambientR)*(clearRef)), RGBVal.R-ambientR);
        TxBufferedString(string1);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        sprintf(string2,"Green: %f ",(RGBVal.G-ambientG)/((whiteG-(float)ambientG)*(clearRef)));
        TxBufferedString(string2);   // send to pc
        sendTxBuf();
        __delay_ms(2);
            
        sprintf(string3,"Blue:  %f \r",(RGBVal.B-ambientB)/((whiteB-(float)ambientB)*(clearRef)));
        TxBufferedString(string3);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        
        
        __delay_ms(1000);
            
        
        
    }
}

