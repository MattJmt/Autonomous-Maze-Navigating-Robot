#include <xc.h>
#include <stdio.h>
#include "color.h"
#include "i2c.h"
#include "serialTest.h"


void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
   
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at Green = 0x18 low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at Blue = 0x1a low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at Blue = 0x1a low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


void getColor (RGB *v){                 // sets the color values for red green blue and clear
    v->R = color_read_Red();
    v->G = color_read_Green();
    v->B = color_read_Blue(); 
    v->C = color_read_Clear();
}


void ambientCal (RGB *v){           // function to calibrate the ambient light
   
    
    for(int i = 0;i<20;i++){
            LATDbits.LATD7 = !LATDbits.LATD7;
            __delay_ms(100);
            }
            LATDbits.LATD7 = !LATDbits.LATD7; 
            __delay_ms(500);
            getColor(v);
            __delay_ms(500);
            LATDbits.LATD7 = !LATDbits.LATD7;
    
}


void whiteCal (RGB *v){             // function to calibrate the light from white card
    
    for(int i = 0;i<20;i++){
            LATDbits.LATD7 = !LATDbits.LATD7;
            __delay_ms(100);
            }
            LATDbits.LATD7 = !LATDbits.LATD7; 
            __delay_ms(500);
            getColor(v);
            __delay_ms(500);
            LATDbits.LATD7 = !LATDbits.LATD7;      
            
}


unsigned int colorDetect (double clearRef, RGB *ambientRGBVal ,RGB *whiteRGBVal, DC_motor *mL, DC_motor *mR){       // main color detection function
        
        RGB RGBVal;         // gets currnet color reading
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
                                    
