#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
int turning_time45;
int reverse_time;

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void DCmotorsInit(DC_motor *mL, DC_motor *mR);
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void forward(DC_motor *mL, DC_motor *mR);
void reverse(DC_motor *mL, DC_motor *mR);
void turnLeft_90(DC_motor *mL, DC_motor *mR);           //Green
void turnRight_90(DC_motor *mL, DC_motor *mR);          //Red
void turn_180(DC_motor *mL, DC_motor *mR);              //Blue
void turnRight_135(DC_motor *mL, DC_motor *mR);         //Orange
void turnLeft_135(DC_motor *mL, DC_motor *mR);          //Light Blue
void reverseSquareRight(DC_motor *mL, DC_motor *mR);    //Yellow
void reverseSquareLeft(DC_motor *mL, DC_motor *mR);     //Pink


#endif
