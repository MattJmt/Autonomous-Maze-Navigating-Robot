# Course project - Mine navigation search and rescue

## Contents
- [Challenge Brief](#challenge-brief)
- ["Mine" environment specification](#mine-environment-specification)
- [Buggy Performance](#buggy-performance)
- [Initial Setup](#initial-setup)
- [Code Structure](#code-structure)
	- [Colour Calibration and Sensing](#colour-calibration-and-sensing)
	- [DC Motor Navigation](#dc-motor-navigation)
	- [Return Journey](#return-journey)
	- [Main Code](#main-code)
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
