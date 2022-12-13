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
#include "interrupts.h"
#include "timers.h"
//#include "interupts2.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    //initialisation functions
    color_click_init();
    initUSART4();
    initDCmotorsPWM(199);       //change this to a variable
    //Interrupts_init();
    
    
    
    // intialisation variables
    
    LATDbits.LATD7=0;   //set initial output state for left LED
    TRISDbits.TRISD7=0; //set TRIS value for pin (output) for the left LED
    
    LATHbits.LATH3=0;   //set initial output state for right LED
    TRISHbits.TRISH3=0; //set TRIS value for pin (output) for the right LED
    
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
    
    
    
    
    LATGbits.LATG0=1;   //set initial output state  RED
    LATEbits.LATE7=1;   //set initial output state  GREEN
    LATAbits.LATA3=1;   //set initial output state  BLUE
    
    TRISGbits.TRISG0=0; //set TRIS value for pin (output)   (output = 0)
    TRISEbits.TRISE7=0; //set TRIS value for pin (output)   (output = 0)
    TRISAbits.TRISA3=0; //set TRIS value for pin (output)   (output = 0)
    
    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
    
    char testString[20];
    char string1[20];
    char string2[20];
    char string3[20];
    char string4[20];
    char string5[20];
    
    
    RGB RGBVal;
    RGB ambientRGBVal;
    RGB whiteRGBVal;
    double clearRef = 0.0;
    double whiteC = 0.0;
    
    DC_motor motorLeft,motorRight;
    DCmotorsInit(&motorLeft,&motorRight);
    
    
    /*
    unsigned int ambientR = 30;
    unsigned int ambientG = 12;
    unsigned int ambientB = 10;
    float ambientC = 56.0;
    
    float whiteR = 68.0;
    float whiteG = 57.0;
    float whiteB = 54.0;
    float whiteC = 195.0;
    
    
    float redPrint = 0.0;
    float bluePrint = 0.0;
    float greenPrint = 0.0;
    */
    
    while(1){
        getColor(&RGBVal);
        
        //add something here to calibrate ambient
        if (!PORTFbits.RF2){
            ambientCal (&ambientRGBVal);            
        }
        
        if (!PORTFbits.RF3){
            whiteCal (&whiteRGBVal);           
        }
            
        
        LATHbits.LATH3=!LATHbits.LATH3;
        
        
        whiteC = whiteRGBVal.C;
        clearRef = RGBVal.C/whiteC;
        
        if (clearRef > 0.12){
            colorDetect (clearRef,&ambientRGBVal,&whiteRGBVal,&motorLeft,&motorRight);  
            
            __delay_ms(500);
        }
        else{stop(&motorLeft,&motorRight);}
        
        __delay_ms(100);
        
        
        sprintf(string4,"  C: %d  %d  %f \r",RGBVal.C, whiteRGBVal.C, clearRef);
        TxBufferedString(string4);   // send to pc
        sendTxBuf();
        __delay_ms(2);

        /*    
     
        FOR DEBUGGING
          
                
        redPrint = (RGBVal.R-ambientR)/((whiteR-(float)ambientR)*(clearRef));
        greenPrint = (RGBVal.G-ambientG)/((whiteG-(float)ambientG)*(clearRef));
        bluePrint = (RGBVal.B-ambientB)/((whiteB-(float)ambientB)*(clearRef));
        
        if ((redPrint < 0) | (redPrint > 2)) { redPrint = 0.0;}
        if ((greenPrint < 0) | (greenPrint > 2)) {greenPrint = 0.0;}
        if ((bluePrint < 0) | (bluePrint > 2)){ bluePrint = 0.0;}
        
        
        
        if (clearRef > 0.12){
            
        if ((redPrint > 0.9) & (greenPrint > 0.9) & (bluePrint >  0.9)){
        sprintf(string5,"White");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }    
            
        if ((redPrint > 1.5) & (redPrint - greenPrint > 0.8) & (redPrint -bluePrint >  0.8)){
        sprintf(string5,"Red");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }
        
        if ((redPrint > 1.3) & (greenPrint > 0.5) & (bluePrint >  0.5)){
        sprintf(string5,"Orange");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }    
        
        if ((redPrint > 1.0) & (greenPrint  > 0.8) & (bluePrint < 0.8)){
        sprintf(string5,"Yellow");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }    
        
        if ((bluePrint - redPrint > 0.7) & (bluePrint - greenPrint > 0.3) & (bluePrint > 0.7 )){
        sprintf(string5,"Blue");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }
        
        if ((greenPrint - redPrint > 0.4 ) & (greenPrint > 1) & (greenPrint - bluePrint > 0.4 )){
        sprintf(string5,"Green");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }
        
        if ((redPrint < 0.7) & (greenPrint >  1.0)& (bluePrint > 1.0)){
        sprintf(string5,"Light Blue");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        }          
        
        if ((redPrint > 0.95) & (greenPrint > 0.8 & greenPrint < 0.9) & (bluePrint >  0.8 & bluePrint < 0.95)){
        sprintf(string5,"Pink");
        TxBufferedString(string5);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        } 
        
        }
        
        sprintf(string4,"  C:  %f ",clearRef);
        TxBufferedString(string4);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        sprintf(string1,"Red: %f ",(redPrint));
        TxBufferedString(string1);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        sprintf(string2,"Green: %f ",(greenPrint));
        TxBufferedString(string2);   // send to pc
        sendTxBuf();
        __delay_ms(2);
            
        sprintf(string3,"Blue:  %f \r",(bluePrint));
        TxBufferedString(string3);   // send to pc
        sendTxBuf();
        __delay_ms(2);
        
        */
        
        
            
        
        
    }
}

