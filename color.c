#include <xc.h>
#include "color.h"
#include "i2c.h"



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


void getColor (RGB *v){
    v->R = color_read_Red();
    v->G = color_read_Green();
    v->B = color_read_Blue(); 
    v->C = color_read_Clear();
}


void ambientCal (RGB *v){
   
    
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


void whiteCal (RGB *v){
    
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


void colorDetect (double clearRef, RGB *ambientRGBVal ,RGB *whiteRGBVal, DC_motor *mL, DC_motor *mR){
        
    
        RGB RGBVal;
        getColor(&RGBVal);

        unsigned int ambientR = ambientRGBVal->R;
        unsigned int ambientG = ambientRGBVal->G;
        unsigned int ambientB = ambientRGBVal->B;

        float whiteR = whiteRGBVal->R;
        float whiteG = whiteRGBVal->G;
        float whiteB = whiteRGBVal->B;
        float whiteC = whiteRGBVal->C;
        
        float redPrint = (RGBVal.R-ambientR)/((whiteR-(float)ambientR)*(clearRef));
        float greenPrint = (RGBVal.G-ambientG)/((whiteG-(float)ambientG)*(clearRef));
        float bluePrint = (RGBVal.B-ambientB)/((whiteB-(float)ambientB)*(clearRef));
        
        if ((redPrint < 0) | (redPrint > 2)) { redPrint = 0.0;}
        if ((greenPrint < 0) | (greenPrint > 2)) {greenPrint = 0.0;}
        if ((bluePrint < 0) | (bluePrint > 2)){ bluePrint = 0.0;}            
             
        //white    
        if ((redPrint > 0.9) & (greenPrint > 0.9) & (bluePrint >  0.9)){     
        __delay_ms(2);
        fullSpeedAhead(mL,mR);
        __delay_ms(2);
        }    
         
        //red
        if ((redPrint > 1.5) & (redPrint - greenPrint > 0.8) & (redPrint -bluePrint >  0.8)){ 
            right45(mL,mR);
            __delay_ms(2);
        }
        
        //Orange
        if ((redPrint > 1.3) & (greenPrint > 0.5) & (bluePrint >  0.5)){
        __delay_ms(2);
        }    
        
        //Yellow
        if ((redPrint > 1.0) & (greenPrint  > 0.8) & (bluePrint < 0.8)){
        __delay_ms(2);
        }    
        
        //Blue
        if ((bluePrint - redPrint > 0.7) & (bluePrint - greenPrint > 0.3) & (bluePrint > 0.7 )){
        __delay_ms(2);
        }
        
        //Green
        if ((greenPrint - redPrint > 0.4 ) & (greenPrint > 1) & (greenPrint - bluePrint > 0.4 )){
        __delay_ms(2);
        }
        
        //Light Blue
        if ((redPrint < 0.7) & (greenPrint >  1.0)& (bluePrint > 1.0)){
        __delay_ms(2);
        }          
        
        //Pink
        if ((redPrint > 0.95) & (greenPrint > 0.8 & greenPrint < 0.9) & (bluePrint >  0.8 & bluePrint < 0.95)){
        __delay_ms(2);
        } 
        
    
    
}