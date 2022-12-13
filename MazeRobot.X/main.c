// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "interrupts.h"
#include "timers.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Timer0_init();
    Interrupts_init();
    initDCmotorsPWM(199);
	//don't forget TRIS for your output!
    TRISCbits.TRISC5 = 0;

    
    DC_motor motorLeft,motorRight;
    
    motorLeft.power = 0;      //zero power to start
    motorLeft.direction = 1;    //set default motor direction
    motorLeft.brakemode = 1;    //brake mode (slow decay)
    motorLeft.posDutyHighByte = (unsigned char *)(&CCPR1H);     //store address of CCP1 duty high byte
    motorLeft.negDutyHighByte = (unsigned char *)(&CCPR2H);     //store address of CCP2 duty high byte
    motorLeft.PWMperiod = 199 ;        // store PWMperiod for motor (value of T2PR in this case)
    
    motorRight.power = 0;      //zero power to start
    motorRight.direction = 1;    //set default motor direction
    motorRight.brakemode = 1;    //brake mode (slow decay)
    motorRight.posDutyHighByte = (unsigned char *)(&CCPR3H);     //store address of CCP1 duty high byte
    motorRight.negDutyHighByte = (unsigned char *)(&CCPR4H);     //store address of CCP2 duty high byte
    motorRight.PWMperiod = 199 ;        // store PWMperiod for motor (value of T2PR in this case)
    
    char count = 0;
 
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin  
    
    if (!PORTFbits.RF2){
         while(1){        
            
            if (count  < 4 ){
            reverse(&motorLeft,&motorRight);
            __delay_ms(500);
            
            count++;
            
            stop(&motorLeft,&motorRight);
            __delay_ms(300);
            
            forward(&motorLeft,&motorRight);
            __delay_ms(500);
            
            turnLeft(&motorLeft,&motorRight);
            __delay_ms(200);
            
            count = 0;
            }
        
            //if (count == 4){
            
        
            //__delay_ms(1000);
        
            
        }
        
        /*
        if (count  < 4 ){
        fullSpeedAhead(&motorLeft,&motorRight);
        
        __delay_ms(400);
        
        turnLeft(&motorLeft,&motorRight);
        
        __delay_ms(240);
        
        count++;
        }
  
        if (count == 4){
        stop(&motorLeft,&motorRight);
 
        
        __delay_ms(1000);
        
             
        turnLeft(&motorLeft,&motorRight);
        
        __delay_ms(300);
        
        count++;
        }
     
        if ((count > 4) & (count < 9) ){
        
        fullSpeedAhead(&motorLeft,&motorRight);
        
        __delay_ms(400);
        
        turnRight(&motorLeft,&motorRight);
        
        __delay_ms(240);        
        count++;
        }
        
        if (count>= 9){
        stop(&motorLeft,&motorRight);

        
        
        
        __delay_ms(1000);
 

        turnRight(&motorLeft,&motorRight);
        
        __delay_ms(300);
        
        //count = 0;
        }
       */
        
    }
}
