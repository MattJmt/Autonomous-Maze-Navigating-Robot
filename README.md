# Course project - Mine navigation search and rescue

## Contents
- [Challenge Brief](#challenge-brief)
- ["Mine" environment specification](#mine-environment-specification)
- [Buggy Performance](#buggy-performance)
- [Initial Setup](#initial-setup)
- [Code Structure](#code-structure)
	- [Main Code](#main-code)
	- [Colour Calibration and Sensing](#colour-calibration-and-sensing)
	- [DC Motor Navigation](#dc-motor-navigation)
	- [Return Journey](#return-journey)
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

[Easy](https://imperiallondon-my.sharepoint.com/:v:/g/personal/ywc19_ic_ac_uk/EVad5Vj5tUxGnKjwEe3ywaEBDxlz3bQ9RxnZfVTA16CrmQ?e=TUmPaU)

[Medium](link)

[Hard](link)

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
serial.c/.h  | Debugging of the Color Clicker and I2C codes. 

## Main Code
