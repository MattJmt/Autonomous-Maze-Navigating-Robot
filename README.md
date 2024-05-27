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

[Easy](https://www.youtube.com/shorts/ZVt_KgNvorw)

[Medium](https://youtube.com/shorts/gXaqp8xVOqs?feature=share)

[Hard](https://www.youtube.com/watch?v=QHPx16F_wd8)

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

It initialises the color click, serial and DC motor functions. It starts by calibrating both the ambient light and the colour of a white card. It then moves forward until it detects a wall, at which points it pulls the assigned instruction from the 'colorDetect' function. The 'turn_history[index]' array stores the previous movement. The amount of times it loops through the 'forward' function in-between movements is counted by the 'counter_history[index]' array. In the case where no colour card is detected for 20 seconds, the 'return_home' function is triggered. 

### DC Motor Navigation

The buggy's navigation is driven by the DC motors, their movements described in dc_motor.c. The DC motor's structure is defined in dc_motor.h to group relevant parameters such as speed (power), direction, brakemode. 

### Colour Calibration and Detection
Colour Calibration and Detection are coded in the color.c/.h and i2cc./h. files. 

I2C is a serial communication bus that enables communication between many devices over a simple 2 wire interface. One wire is the data line (SDA) and is used for both transmission and receiving. The second wire (SCL) is used for a clock signal to ensure all devices are synchronous. 

The 'colorDetect' function detects the RGB values measured by the color click, associates it to a colour and movement, and returns a numerical value associated to that movement so it can be stored in the 'turn_history' Array for the backtrace.

### Return Journey Memory Storage

The function 'return_home_turns', situated in colors.c, matches a forward movement to its reverse movement by reading the colour reference number ('colour_ref') and assigning it its opposite movement. 

## Future Improvements

Our buggy could be further improved in accuracy, speed and reliability by doing the following:

1. For the ease of testing and programming, a rotational calibration system could have been implemented for the robot to recoginise three of its four surroundings walls. 
2. An optimised memory storage system that maps and follows the shortest path from the white card to the starting position.
3. Implementing machine learning to the detection of the colour cards would result in a more accurate analysis prone to less mistakes and offsets.
4. Change the type of tyres used to prevent any slipping that might impede the robot's path and linear trajectory.
