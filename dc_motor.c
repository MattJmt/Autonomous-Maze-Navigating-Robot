#include <xc.h>
#include "dc_motor.h"

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
    
    LRmotorsInit();
 
}


void LRmotorsInit(){
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
void stop(DC_motor *mL, DC_motor *mR)
{
    mL->power = 0;
    mR->power = 0;     
    
    mL->direction = 1;
    mR->direction = 1;
    
    mL->brakemode = 0;   
    mL->brakemode = 0; 
    
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    mL->power = 50;
    mR->power = 50;     
    
    mL->direction = 0;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn right 
void turnRight(DC_motor *mL, DC_motor *mR)
{
    mL->power = 50;
    mR->power = 50;     
    
    mL->direction = 1;
    mR->direction = 0;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{

    mL->power = 50;
    mR->power = 50;     
    
    mL->direction = 1;
    mR->direction = 1;
    
    mL->brakemode = 1;   
    mL->brakemode = 1; 
    
    setMotorPWM(mL);
    setMotorPWM(mR);
}

