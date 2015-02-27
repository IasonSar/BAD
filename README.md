# BAD

About
-----
BAD (**B**arrettHand **A**UTh **D**river) is a basic driver, written in C++, that provides the necessary functionalities in order to control the robotic hand BarrettHand under Linux. This driver does not provide real time capabilites yet. BAD has been developed during a diploma thesis in Aristotle University of Thessaloniki (AUTh) and is open source software.

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
Download from [here](http://www.peak-system.com/fileadmin/media/linux/index.htm) the latest version of the driver.
```Shell
$ sudo apt-get install libpopt-dev		#Install libpopt-dev
$ sudo apt-get install g++			#Install g++
$ tar -xzf peak-linux-driver-7.XX.tar.gz	#Unpack driver
$ cd peak-linux-driver-7.XX
$ make clean
$ make NET=NO RT=NO_RT PCC=NO ISA=NO DNG=NO
$ sudo make install
$ sudo modprobe pcan
```

If everything is installed you with:
    $ cat /proc/pcan
You will se something like that:

```
*------------ PEAK-Systems CAN interfaces (http://www.peak-system.com)-------
*-------------------------- Release_20110912_n (7.4.0) ----------------------
*---------------- [mod] [isa] [pci] [dng] [par] [usb] [pcc] -----------------
*--------------------- 1 interfaces @ major 250 found -----------------------
*n -type- ndev --base-- irq --btr- --read-- --write- --irqs-- -errors- status
0    pci -NA- fa010000 018 0x001c 00000000 00000000 00000000 00000000 0x0000
```
### 2. Installing PCAN-Basic API
    todo

## Compiling and using BAD
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


