/**
	bad.cpp - version 0.4

	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 20/03/2015
		
	bad.cpp is part of BAD (Barretthand AUTh Driver). BAD is a 
	basic driver to control a BarrettHand under Linux. This driver does not 
	provide real-time capabilities.
	
	Note that you can find the latest version of BAD in the following repo:
	<https://github.com/iSaran/BAD>
	
	BAD is free software; you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation; either version 3 of the License, or (at your
	option) any later version.

	BAD is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
 	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



//#include "kinematics.hh"
#include "HighLevelBAD.h"

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[])
{
    string err_type = "[BAD] Error. ";
	printf("[BAD] Starting...\n");
	
	string hand_type = "BH8-282";
	
/* Create the CAN bus and the hand objects */
	CANbus bus;
	BHand hand(&bus, hand_type, &mutex);
	HandState state(&hand);
	BAD driver(&hand);
	Kinematics kin;
	
/** Now you can write your own code using BAD API */	
	
	driver.initHand();
//	driver.initSG(false);
	
//	driver.precisionGrasp();	
//	driver.fetchAndRelease2(FINGER1);	
//	driver.handShake(state);
//	driver.distortionControl(1);
	
	//driver.simpleGrasp(2500);
	//driver.release(FINGER1);
	//driver.release2(FINGER1);
	
	//driver.holdGrasp(3100);

	driver.terminate();
	
	printf("[BAD] Shutting down...\n");	
	return 0;
}




