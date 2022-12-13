#include <xc.h>
#include <stdio.h>
#include "serial.h"

void initUSART4(void) {

	//code to set up USART4 for Reception and Transmission =
	//see readme for detials
    RC0PPS = 0x12; // Map EUSART4 TX to RC0
    RX4PPS = 0x11; // RX is RC1
    TRISCbits.TRISC1=1; //set TRIS value for pin (input)
    
    BAUD4CONbits.BRG16 = 0; 	//set baud rate scaling
    TX4STAbits.BRGH = 0; 		//high baud rate select bit
    SP4BRGL = 51; 			//set baud rate to 103 = 9600bps
    SP4BRGH = 0;			//not used

    RC4STAbits.CREN = 1; 		//enable continuos reception
    TX4STAbits.TXEN = 1; 		//enable transmitter
    RC4STAbits.SPEN = 1; 		//enable serial port
    
    
}

//function to wait for a byte to arrive on serial port and read it once it does 
char getCharSerial4(void) {
    while (!PIR4bits.RC4IF);//wait for the data to arrive
    return RC4REG; //return byte in RCREG
}

//function to check the TX reg is free and send a byte
void sendCharSerial4(char charToSend) {
    while (!PIR4bits.TX4IF); // wait for flag to be set
    TX4REG = charToSend; //transfer char to transmitter
}

//function to send a string over the serial interface
void sendStringSerial4(char *string){
	//Hint: look at how you did this for the LCD lab 
    
    while(*string != 0) {sendCharSerial4(*string++);}
}

//functions below are for Ex3 and 4 (optional)

// circular buffer functions for RX
// retrieve a byte from the buffer
char getCharFromRxBuf(void){
    if (RxBufReadCnt>=RX_BUF_SIZE) {RxBufReadCnt=0;} 
    return EUSART4RXbuf[RxBufReadCnt++];
}

// add a byte to the buffer
void putCharToRxBuf(char byte){
    if (RxBufWriteCnt>=RX_BUF_SIZE) {RxBufWriteCnt=0;}
    EUSART4RXbuf[RxBufWriteCnt++]=byte;
}

// function to check if there is data in the RX buffer
// 1: there is data in the buffer
// 0: nothing in the buffer
char isDataInRxBuf (void){
    return (RxBufWriteCnt!=RxBufReadCnt);
}


// circular buffer functions for TX
// retrieve a byte from the buffer
char getCharFromTxBuf(void){
    if (TxBufReadCnt>=TX_BUF_SIZE) {TxBufReadCnt=0;} 
    return EUSART4TXbuf[TxBufReadCnt++];
}

// add a byte to the buffer
void putCharToTxBuf(char byte){
    if (TxBufWriteCnt>=TX_BUF_SIZE) {TxBufWriteCnt=0;}
    EUSART4TXbuf[TxBufWriteCnt++]=byte;
}

// function to check if there is data in the TX buffer
// 1: there is data in the buffer
// 0: nothing in the buffer
char isDataInTxBuf (void){
    return (TxBufWriteCnt!=TxBufReadCnt);
}

//add a string to the buffer
void TxBufferedString(char *string){
	//Hint: look at how you did this for the LCD lab 
    while(*string != 0){   
		putCharToTxBuf(*string++);}
}

//initialise interrupt driven transmission of the Tx buf
//your ISR needs to be setup to turn this interrupt off once the buffer is empty
void sendTxBuf(void){
    if (isDataInTxBuf()) {PIE4bits.TX4IE=1;} //enable the TX interrupt to send data
}






void Interrupts_init(void)
{


INTCONbits.IPEN = 1; // enables priority levels on interrupts
INTCONbits.PEIE = 1; //turns on peripheral interrupts 

PIE4bits.RC4IE=1;	//receive interrupt
PIE4bits.TX4IE=0;	//transmit interrupt (only turn on when you have more than one byte to send)

IPR4bits.RC4IP = 1; //high priority interrupt bit
IPR4bits.TX4IP = 1; //high priority interrupt bit
    
INTCONbits.GIE=1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)

}

void __interrupt(high_priority) HighISR()
{
    if(PIR4bits.RC4IF == 1){              // check the interrupt source
        putCharToRxBuf(RC4REG);    
    }
    
    if(PIR4bits.TX4IF == 1){        // check the interrupt source
        if(isDataInTxBuf()){
            TX4REG = getCharFromTxBuf();
            
        }
        else{
    PIE4bits.TX4IE = 0;
        }
	}
  
}
