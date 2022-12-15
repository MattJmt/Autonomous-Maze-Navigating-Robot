# Course project - Mine navigation search and rescue

## Contents
- [Challenge Brief](#challenge-brief)
- ["Mine" environment specification](#mine-environment-specification)
- [Buggy Performance](#buggy-performance)
- [Initial Setup](#initial-setup)
- [Code Structure](#code-structure)
	- [Main Code](#main-code)
	- [DC Motor Navigation](dc-motor-navigation)
	- [Colour Calibration and Detection](#colour-calibration-and-detection)
	- [Return Journey Memory Storage](#return-journey-memory-storage)
- [Future Improvements](#future-improvements)

## Challenge brief

Our task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Our robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## "Mine" environment specification

A "mine" is constructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Colour | Instruction
---------|---------
Red | Turn Right 90
Green | Turn Left 90
Blue | Turn 180
Yellow | Reverse 1 square and turn right 90
Pink | Reverse 1 square and turn left 90
Orange | Turn Right 135
Light blue | Turn Left 135 
White | Finish (return home)
Black | Maze wall colour

Mine courses will vary in difficulty, with the simplest requiring 4 basic moves to navigate. More advanced courses may require 10 or moves to navigate. The mines may have features such as dead ends but colour cards will always direct we to the end of the maze. Once the end of the maze has been reached, we must return to the starting position. An example course to navigate is shown below. We do not know in advance which colours will be in the course or how many.

![Navi Diagram](gifs/maze.gif)

## Buggy Performance

We tested our buggy during the testing session occuring at the end of the term. We were able to successfully complete the easy, medium and hard maze. The following videos demonstrate our buggy's performance:

[Easy](https://imperiallondon-my.sharepoint.com/:v:/r/personal/mjj20_ic_ac_uk/Documents/ECM%20Maze%20Runs/Maze%20Easy.mov?csf=1&web=1&e=84Udba)

[Medium](https://imperiallondon-my.sharepoint.com/:v:/r/personal/mjj20_ic_ac_uk/Documents/ECM%20Maze%20Runs/Maze%20Medium.mov?csf=1&web=1&e=wk7yMX)

[Hard](https://imperiallondon-my.sharepoint.com/:v:/r/personal/mjj20_ic_ac_uk/Documents/ECM%20Maze%20Runs/Maze%20Hard.MOV?csf=1&web=1&e=3cwmPK)

## Initial Setup

The buggy is placed at its starting position in the maze. Upon being switched on, it automatically calibrates to the ambient light. An additional re-calibration to the ambient light is possible by pressing the 'RF3' button on the buggy. It is then calibrated to the white colour by placing a white card in front of the colour click and pressing the 'RF2' button. Once happy with the calibration, pressing simultaneously on the 'RF2' and 'RF3' buttons will initialise the launch sequence and the buggy will navigate the maze.

## Code Structure

Our code is organised in a main file that calls secondary code files describing the motion, colour detection and interrupt routines.

Code file  | Description
------------- | -------------
main.c  | Main code: it calls all initialisation functions, 
color.c/.h  | Colour Detection, White card calibration, Ambient Light Calibration, Return Path function. 
dc_motor.c/.h  | Sets and calibrates the different turning instructions and movements.
i2c.c/.h  | Communication from the Color Clicker to sampling dataand read the 16 bit digital values for each of the RGBC channels.
serial.c/.h  | Debugging of the Color Clicker and i2c codes. 

### Main Code

The main code is present in main.c

It initialises the color click, serial and DC motor functions. It starts by calibrating both the ambient light and the colour of a white card. It then moves forward until it detects a wall, at which points it pulls the assigned instruction from the 'colorDetect' function. The 'turn_histroy[index] array stores the previous movement. The amount of times it loops through the 'forward' function in-between movements is counted by the counter_history[index] array. In the case where no colour card is detected for 20 seconds, the 'return_home' function is triggered. 

```
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
        
```
### DC Motor Navigation

The buggy's navigation is driven by the DC motors, their movements described in dc_motor.c. The DC motor's structure is defined in dc_motor.h to group relevant parameters such as speed (power), direction, brakemode. 

```
typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;
    
 ```
By creating a structure for the motor, we are able to implement it to our movement functions such as the 'forward' function presented below.

```
void forward(DC_motor *mL, DC_motor *mR)
{
    mL->direction = 1;          //set left motors direction to forward
    mR->direction = 1;          //set right motors direction to forward
    
    mL->brakemode = 1;          //  set left motors brakemode to slow decay
    mL->brakemode = 1;          //  set right motors brakemode to slow decay
    
    while ((mL->power)<40 && (mR->power)<40){       // limit max forward speed to 40 %
        if ((mL->power) < 40){
            mL->power += 5;                         // gradually increase left motor speed to 40%
        }
        if ((mR->power) < 40){
            mR->power += 5;                         // gradually increase right motor speed to 40%  
        } 

    }
    setMotorPWM(mL);       //set CCP PWM output from the values in the motor structure
    setMotorPWM(mR);      //set CCP PWM output from the values in the motor structure
    }
    
```

### Colour Calibration and Detection
Colour Calibration and Detection are coded in the color.c/.h and i2cc./h. files. 

I2C is a serial communication bus that enables communication between many devices over a simple 2 wire interface. One wire is the data line (SDA) and is used for both transmission and receiving. The second wire (SCL) is used for a clock signal to ensure all devices are synchronous. 

The 'colorDetect' function detects the RGB values measured by the color click, associates it to a colour and movement, and returns a numerical value associated to that movement so it can be stored in the 'turn_history' Array for the backtrace.

```
unsigned int colorDetect (double clearRef, RGB *ambientRGBVal ,RGB *whiteRGBVal, DC_motor *mL, DC_motor *mR){       // main color detection function
        
        RGB RGBVal;         // gets current color reading
        getColor(&RGBVal);

        unsigned int ambientR = ambientRGBVal->R;   // sets ambient red green and blue values
        unsigned int ambientG = ambientRGBVal->G;
        unsigned int ambientB = ambientRGBVal->B;

        float whiteR = whiteRGBVal->R;          // set white card red green and blue values
        float whiteG = whiteRGBVal->G;
        float whiteB = whiteRGBVal->B;
        float whiteC = whiteRGBVal->C;
        
        float redPrint = (RGBVal.R-ambientR)/((whiteR-(float)ambientR)*(clearRef));         //function to normalise the current red green and blue values depending on ambient conditions
        float greenPrint = (RGBVal.G-ambientG)/((whiteG-(float)ambientG)*(clearRef));       // allows for accurate color calibration in any conditions
        float bluePrint = (RGBVal.B-ambientB)/((whiteB-(float)ambientB)*(clearRef));        //requires only ambient and white card light for calibration sequence
        
        unsigned int colour_ref = 11;       // set defaul value of color outside of expected range
        
        if ((redPrint < 0) | (redPrint > 2)) { redPrint = 0.0;}             // ensure that the red green and blue values fall within expected range, usualy between 0 and 1
        if ((greenPrint < 0) | (greenPrint > 2)) {greenPrint = 0.0;}        // this adds extra security to color detection function
        if ((bluePrint < 0) | (bluePrint > 2)){ bluePrint = 0.0;}            
             
        //white    
        if ((redPrint > 0.9) & (greenPrint > 0.9) & (bluePrint >  0.9)){            // rbg values for each color taken from initial color sensor calibration, but DONT require change for new environment  
        colour_ref = 8;
        }    
         
        //red
        else if ((redPrint > 1.5) & (redPrint - greenPrint > 0.8) & (redPrint -bluePrint >  0.8)){ 
        __delay_ms(2);
        turnRight_90(mL,mR);
        __delay_ms(2);
        colour_ref = 1;

        }
        
        //Green
        else if ((greenPrint - redPrint > 0.4 ) & (greenPrint > 1) & (greenPrint - bluePrint > 0.4 )){
        __delay_ms(2);
        turnLeft_90(mL,mR);
        __delay_ms(2);
        colour_ref = 2;
        }
        
        //Blue
        else if ((bluePrint - redPrint > 0.7) & (bluePrint - greenPrint > 0.3) & (bluePrint > 0.7 )){
        __delay_ms(2);
        turn_180(mL,mR);
        __delay_ms(2);
        colour_ref = 3;
        }
        
        //Yellow
        else if ((redPrint > 1.0) & (greenPrint  > 0.8) & (bluePrint < 0.8)){
            
        __delay_ms(2);
        reverseSquareRight(mL,mR);
        __delay_ms(2);
        colour_ref = 4;
        }    
        
        //Pink
        else if ((redPrint > 0.95) & (greenPrint > 0.8 & greenPrint < 0.9) & (bluePrint >  0.8 & bluePrint < 0.95)){
        __delay_ms(2);
        reverseSquareLeft(mL,mR);
        __delay_ms(2);
        colour_ref = 5;
        } 
        
        //Orange
        else if ((redPrint > 1.3) & (greenPrint > 0.5) & (bluePrint >  0.5)){
        __delay_ms(2);
        turnRight_135(mL,mR);
        __delay_ms(2);
        colour_ref = 6;
        
        }            
        
        //Light Blue
        else if ((redPrint < 0.7) & (greenPrint >  1.0)& (bluePrint > 1.0)){
        __delay_ms(2);
        turnLeft_135(mL,mR);
        __delay_ms(2);
        colour_ref = 7;
        
        }          
        
        return colour_ref;
}
    
```

### Return Journey Memory Storage

The function 'return_home_turns', situated in colors.c, matches a forward movement to its reverse movement by reading the colour reference number ('colour_ref') and assigning it its opposite movement. 

```
// function to reverse the turn and count the number of times the forward function was looped 
void return_home_turns(unsigned int *turn_history, unsigned int *counter_history, unsigned int index, DC_motor *mL, DC_motor *mR) 
{

    
    for (int k = (index); k >= 0; k--){    // set up a loop in the range of the amount of instructions recorded (index)
        
            switch (turn_history[k]){                       // switch case to read the instruction at a specificed index (k)
                case 1:                             //Red
                    turnLeft_90(mL,mR);
                    break;
                case 2:                             //Green
                    turnRight_90(mL,mR);
                    break;
                case 3:                             //Blue
                    turn_180(mL,mR);
                    break;
                case 4:                             //Yellow
                    turnLeft_90(mL,mR);
                    break;
                case 5:                             //pink
                    turnRight_90(mL,mR);
                    break;
                case 6:                             //orange
                    turnLeft_135(mL,mR);
                    break;
                case 7:                             // light blue
                    turnRight_135(mL,mR);
                    break;
                case 0:                             //forward
                    forward(mL,mR);
                    int i = (counter_history[k]-2); // read the "time" value of the forward instruction and run same amount of iterations as recorder in initial traversal of maze
                    while(i>0){
                    i--;
                    __delay_ms(50);
                    }
                    break;
                default:                            // in erronious cases stop the robot temporarily
                    stop(mL,mR);
                    break;
               
            }
            __delay_ms(50);
        }
        
    }
    
```

## Future Improvements

Our buggy could be further improved in accuracy, speed and reliability by doing the following:

1. For the ease of testing and programming, a rotational calibration system could have been implemented for the robot to recoginise three of its four surroundings walls. 
2. An optimised memory storage system that maps and follows the shortest path from the white card to the starting position.
3. Implementing machine learning to the detection of the colour cards would result in a more accurate analysis prone to less mistakes and offsets.
4. Change the type of tyres used to prevent any slipping that might impede the robot's path and linear trajectory.
