/**
	bad.cpp - version 0.4

	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 10/02/2015
	
	bad.cpp is part of BAD (Barretthand AUTh Driver). BAD is a 
	basic driver to control a BarrettHand under Linux. This driver does not 
	provide real-time capabilities.
	
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

#include "HighLevelBAD.h"

#define NUM_THREADS     5

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int rc;

/** Define a struct to pass multiple objects to POSIX create thread function. */
struct arg_struct {
    BAD*	 object;
    HandState* myState;
    double myDouble;
};



/** Create wrapper functions in order to call C++ object member functions
	as a POSIX thread.  */
void *callLogger(void *object)
{
	((BAD *)object)->logger();
	return NULL;
}

void *callFetchAndRelease(void *object)
{
	((BAD *)object)->fetchAndRelease(FINGER1);
	return NULL;
}

void *callPrecisionGrasp(void *object)
{
	((BAD *)object)->precisionGrasp();
	return NULL;
}

void *callHandshake(void *arguments)
{
	struct arg_struct *args = (struct arg_struct *)arguments;
	
	((BAD *)args->object)->handShake(*args->myState);
	return NULL;
}

void *callDistortionControl(void *arguments)
{
	struct arg_struct *args = (struct arg_struct *)arguments;
	
	((BAD *)args->object)->distortionControl(args->myDouble);
	return NULL;
}

int main(int argc, char* argv[])
{
/** Create BAD objects and init the bus */
	string err_type = "[BAD] Error. ";
	printf("[BAD Logger] Starting...\n");
	string hand_type = "BH8-282";
	CANbus bus;
	BHand hand(&bus, hand_type, &mutex);
	HandState state(&hand);
	BAD driver(&hand);
	Kinematics kin;
	
	struct arg_struct args;
	args.object = &driver;
	 args.myState = &state;
	args.myDouble = 1;

	//driver.initHand();

/** Start threads */
	int i = 0;
	void *status;
	pthread_t threads[NUM_THREADS];
	
	driver.initHand();

	/*
	rc = pthread_create(&threads[0], NULL, &callTest, &driver);
	if (rc){
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	*/
	rc = pthread_create(&threads[0], NULL, &callLogger, &driver);
	if (rc){
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	
	/*
	rc = pthread_create(&threads[1], NULL, &callHandshake, (void*)&args);
	if (rc){
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	*/
	
	rc = pthread_create(&threads[1], NULL, &callDistortionControl, (void*)&args);
	if (rc){
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	
/** Wait the two threads to finish, before you finish main. */
	rc = pthread_join(threads[0], &status);
	rc = pthread_join(threads[1], &status);
	pthread_exit(NULL);
	
	return 0;
}




