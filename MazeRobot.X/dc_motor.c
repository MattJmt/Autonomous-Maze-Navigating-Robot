#include <xc.h>
#include "dc_motor.h"

//Calibration 
int turning_time45 = 187; // time period elapsed to achieve a 45 degree rotation
int reverse_time = 700; //time period elapsed when reversing after detecting a colour (this is different to square reverse)


// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    
    //set the lat registers for the appropriate pins
    LATEbits.LATE2 = 0;
    LATEbits.LATE4 = 0;
    LATCbits.LATC7 = 0;
    LATGbits.LATG6 = 0;
    
    //set the tris registers for the appropriate pins
    TRISEbits.TRISE2 = 0 ;
    TRISEbits.TRISE4 = 0 ;
    TRISCbits.TRISC7 = 0 ;
    TRISGbits.TRISG6 = 0 ;
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=199; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
   
}

void DCmotorsInit(DC_motor *mL, DC_motor *mR){
    
    mL->power = 0;      //zero power to start
    mL->direction = 1;    //set default motor direction
    mL->brakemode = 1;    //brake mode (slow decay)
    mL->posDutyHighByte = (unsigned char *)(&CCPR1H);     //store address of CCP1 duty high byte
    mL->negDutyHighByte = (unsigned char *)(&CCPR2H);     //store address of CCP2 duty high byte
    mL->PWMperiod = 199 ;        // store PWMperiod for motor (value of T2PR in this case)
    
    mR->power = 0;      //zero power to start
    mR->direction = 1;    //set default motor direction
    mR->brakemode = 1;    //brake mode (slow decay)
    mR->posDutyHighByte = (unsigned char *)(&CCPR3H);     //store address of CCP1 duty high byte
    mR->negDutyHighByte = (unsigned char *)(&CCPR4H);     //store address of CCP2 duty high byte
    mR->PWMperiod = 199 ;        // store PWMperiod for motor (value of T2PR in this case)
}


// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually 
void stop(DC_motor *mL, DC_motor *mR){
    
    while ((mL->power)>0 && (mR->power)>0){
        if ((mL->power) >0){
            mL->power -= 5;
        }
        if ((mR->power) >0){
            mR->power -= 5;     
        }
       
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
    __delay_ms(100);
}

//function to make the robot gradually move forward
void forward(DC_motor *mL, DC_motor *mR)
{
    mL->direction = 1;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    while ((mL->power)<25 && (mR->power)<25){
        if ((mL->power) < 25){
            mL->power += 5;
        }
        if ((mR->power) <25){
            mR->power += 5;     
        } 

    }
    setMotorPWM(mL);
    setMotorPWM(mR);
    //__delay_ms(time_period);
}

//function to make the robot gradually reverse
void reverse(DC_motor *mL, DC_motor *mR)
{
    mL->direction = 0;
    mR->direction = 0;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    while ((mL->power)<25 && (mR->power)<25){
        if ((mL->power) < 25){
            mL->power += 5;
        }
        if ((mR->power) <25){
            mR->power += 5;     
        } 

    }
    setMotorPWM(mL);
    setMotorPWM(mR);
    
  // __delay_ms(time_period);
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{    
   
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(turning_time45);
    
    stop(mL, mR);
    

}

void turnRight(DC_motor *mL, DC_motor *mR)
{    
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 1;
    mR->direction = 0;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
  
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(turning_time45);
    
    stop(mL, mR);
    
}

/***GREEN***/    
void turnLeft_90(DC_motor *mL, DC_motor *mR)            //Green
{
   
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(turning_time45);
    stop(mL, mR);
}


/***RED***/    
void turnRight_90(DC_motor *mL, DC_motor *mR) //Red
{    
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 1;
    mR->direction = 0;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
  
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(turning_time45);
    
    stop(mL, mR);
}

/***BLUE***/    
void turn_180(DC_motor *mL, DC_motor *mR)              
{
   
    turnRight(mL,mR);
    turnRight(mL,mR);
    stop(mL, mR);
}

/***ORANGE***/    
void turnRight_135(DC_motor *mL, DC_motor *mR) //Orange
{    
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 1;
    mR->direction = 0;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
  
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(1.5*turning_time45);
    
    stop(mL, mR);
}


/***LIGHTBLUE***/    
void turnLeft_135(DC_motor *mL, DC_motor *mR)          //Light Blue
{
   
    stop(mL, mR);       // buggy must me at rest for a more accurate turn.
    __delay_ms(100);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time);
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    while((mL->power < 80) || (mR->power < 80)){                 // Limit the motor power to 70%
        mL->power += 10;                   // Gradually increase the left motor power
        mR->power += 10;                   // Gradually increase the right motor power
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
   __delay_ms(3*turning_time45);
    stop(mL, mR);
}

/***YELLOW***/    
void reverseSquareRight(DC_motor *mL, DC_motor *mR)     //Yellow
{
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time); // to position away from wall, in centre of square
    
    stop(mL, mR);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(2*reverse_time); // to position away from wall, in centre of square
    
    turnRight_90(mL,mR);  //90 degree rotation to the right
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
}


/***PINK***/    
void reverseSquareLeft(DC_motor *mL, DC_motor *mR)      //Pink
{
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(reverse_time); // to position away from wall, in centre of square
    
    stop(mL, mR);
    
    reverse(mL, mR);    // reverse from the wall to avoid collision when turning
    __delay_ms(2*reverse_time); // to position away from wall, in centre of square
    
    turnLeft_90(mL,mR);  //90 degree rotation to the right
    
    stop(mL, mR);   // buggy must me at rest for a more accurate turn.
}



    