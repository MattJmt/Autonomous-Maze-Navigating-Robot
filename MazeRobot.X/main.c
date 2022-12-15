// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#pragma once

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "serialTest.h"
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
    
    RGB RGBVal;
    RGB ambientRGBVal;
    RGB whiteRGBVal;
    double clearRef = 0.0;
    double whiteC = 19000.0;
    unsigned char carGo = 0;
    
    DC_motor motorLeft,motorRight;
    DCmotorsInit(&motorLeft,&motorRight);
    
    unsigned int turn_history[32];
    unsigned int counter_history[32];
    unsigned int index = 0;
    unsigned int forwardCount = 0;
    unsigned int colorNum = 0;
    
    ambientCal(&ambientRGBVal);
    
    while(1){
        getColor(&RGBVal);  //could be messing it
        
        if(!PORTFbits.RF2 && !PORTFbits.RF3){                // turn the car on and off
            LATDbits.LATD7 = 1 , LATHbits.LATH3 = 1;            
            __delay_ms(500);
            LATDbits.LATD7 = 0 , LATHbits.LATH3 = 0;
            carGo = !carGo;
        }
        
        //add something here to calibrate ambient
        if (!PORTFbits.RF2 & PORTFbits.RF3){        // manually calibrate ambient
            ambientCal (&ambientRGBVal);            
        }
        
        if (!PORTFbits.RF3 & PORTFbits.RF2){            // manually calibrate white
            whiteCal (&whiteRGBVal);           
        }            
        
        LATHbits.LATH3=!LATHbits.LATH3;         //light to let user know car is running
        
        whiteC = whiteRGBVal.C;
        clearRef = RGBVal.C/whiteC;     // this forces it to be a float
        
        if ((clearRef > 0.12) && carGo){         //waits till car gets close to wall
                  
            colorNum = colorDetect(clearRef,&ambientRGBVal,&whiteRGBVal,&motorLeft,&motorRight);
            
            if (colorNum < 9){
            
            turn_history[index] = 0;
            counter_history[index] = forwardCount;  
                    
            index +=1;
            forwardCount = 0;

            if (colorNum == 8){
                turn_180(&motorLeft,&motorRight);
                LATDbits.LATD7 = 1 , LATHbits.LATH3 = 1; 
                return_home_turns(&turn_history,&counter_history, (index), &motorLeft, &motorRight);
                carGo = 0;                     
            }
            turn_history[index] = colorNum;  
            counter_history[index] = 1;
            
            index += 1;
            __delay_ms(500);
            }
            
        }
        
        if (carGo){
            forward(&motorLeft,&motorRight);
            LATDbits.LATD4 = !LATDbits.LATD4;
            forwardCount +=1;
            
        }
        
        else{stop(&motorLeft,&motorRight);}
        
        __delay_ms(50);
    
    }
}

