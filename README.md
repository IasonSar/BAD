# BAD

About
-----
BAD (**B**arrettHand **A**UTh **D**river) is a basic driver, that provides the necessary functionalities in order to control the robotic hand BarrettHand under Linux. This driver does not provide real time capabilites yet. BAD has been developed during a diploma thesis in Aristotle University of Thessaloniki (AUTh) and is open source software.

Note that the driver implements a BHand having the Fingertip Torque Sensor option, but not the tactile sensors.

Dependencies
------------
- Required:
 - PCAN Linux Driver (for Peak-Systems hardware)
 - PCAN-Basic API
- Optional
  - Xenomai

## Installing Dependencies on Ubuntu Linux 12.04
### 1. Installing PCAN Linux Driver
    todo
### 2. Installing PCAN-Basic API
    todo

Changelist
----------
* 0.4 - Additional changes to higher level.
 * Adding more functionality in Higher BAD Level, with higher level functions for grasping and signal filtering.
 * Bug fixes
 * Additional executable that use 2 threads, 1 for the application and 1 for logging data to file.
* 0.3 - Adding the kinematics of the robot
* 0.2 - Changing the layered structure of the program. 
 * The first tier (CANbus.cpp) is now independent from upper levels.
* 0.1 - initial release


