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

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    //initialisation functions
    color_click_init();
    initUSART4();
    initDCmotorsPWM(199);       //change this to a variable
    
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
    
    LATDbits.LATD4 = 1;     //turn on all lights on the car 
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
    
    RGB RGBVal;             // initialise three RGB struct variables
    RGB ambientRGBVal;
    RGB whiteRGBVal;
    double clearRef = 0.0;      // initialise clear ref which will be the normalised clear value
    double whiteC = 19000.0;
    unsigned char carGo = 0;       // initialise car go which will act as a soft switch
    
    DC_motor motorLeft,motorRight;          // initialise motors
    DCmotorsInit(&motorLeft,&motorRight);
    
    unsigned int turn_history[100];             // initialise arrays for return to home
    unsigned int counter_history[100];
    unsigned int index = 0;                     // index will be used to keep track of the index of the instructions
    unsigned int forwardCount = 0;              // forward count will be used as a timer for the amount of forward movement
    unsigned int colorNum = 0;                  // color num will be used as an integer representation for each color
    
    ambientCal(&ambientRGBVal);         // calibrate ambient light on startup
    
    while(1){
        getColor(&RGBVal);  //get the colour value
        
        if(!PORTFbits.RF2 && !PORTFbits.RF3){                // turn the car on and off by pressing both buttons
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
        
        LATHbits.LATH3=!LATHbits.LATH3;         //blink the light to let user know car is running
        
        whiteC = whiteRGBVal.C;         
        clearRef = RGBVal.C/whiteC;     // calculate normalised clear value and force it to be a float
        
        if ((clearRef > 0.12) && carGo){         //waits till car gets close to wall before detection
                  
            colorNum = colorDetect(clearRef,&ambientRGBVal,&whiteRGBVal,&motorLeft,&motorRight);        // detect the color 
            
            if (colorNum < 9){      //ensure that a valid color has been detected
            
            turn_history[index] = 0;                // add the previous forward instruction to the turn history
            counter_history[index] = forwardCount;     //add the time the robot moved forward for
                    
            index +=1;              // incrament the index of both turn and couter history
            forwardCount = 0;       // reset the forward movement counter

            if (colorNum == 8){     // check if white has been detected
                turn_180(&motorLeft,&motorRight);       // turn around
                LATDbits.LATD7 = 1 , LATHbits.LATH3 = 1;        // turn on lights to show used white is detected
                return_home_turns(&turn_history,&counter_history, (index), &motorLeft, &motorRight);        // run return home function
                carGo = 0;                      // stop the car
            }
            turn_history[index] = colorNum;  // if white not detected add the detected color to the turn history
            counter_history[index] = 1;            // add a time of 1 for the color as the movement is done once
            index += 1;                         // incrament index
            __delay_ms(500);
            }  
        }
        
        if (carGo){                             // make the car go forward when turned on
            forward(&motorLeft,&motorRight);
            LATDbits.LATD4 = !LATDbits.LATD4;
            forwardCount +=1;                   // keep track of the time car is moving forward
        }
        
        else{stop(&motorLeft,&motorRight);}     // stops the car when turned off
        
        __delay_ms(50);
        
        if (forwardCount > 2000){return_home_turns(&turn_history,&counter_history, (index), &motorLeft, &motorRight);}      // if no color detected for 20 seconds, return to home
    }
}

