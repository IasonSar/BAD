/**
	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 28/02/2015
	
	This file is part of BAD (Barretthand AUTh Driver). BAD is a 
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

#include "BarrettHand.h"
#include "kinematics.h"
#include <omp.h> // for benchmarking and timing

/**
* The driver class. It implements the basic functionality of BAD. It contains
* a number of member functions that uses the BarrettHand Protocol level in
* order to implement basic functions for BarrettHand applications. 
*/
class BAD {
	BHand *hand; /**< Pointer object to the BarrettHand*/
	HandState *state; /**< Pointer object to the hand's state */
	string msg_type; /**< The type of messages inside this class. */
	string err_type; /**< The type of errors. */
	
	int min_sg[3]; /**< 3x1 vector that contains the no load SG value. */
	int max_sg[3]; /**< 3x1 vector that contains the max SG value (2kg) */
	
	bool sg_inited; /**< Flag to inform if the sg has been initialized. */
	
public:
	/**
	* The constructor of BADriver.
	*/
	BAD(BHand*);

	/**
	* The destructor of BADriver.
	*/
	~BAD();
    
/** Basic functionality of BarretHand */

	/**
	* The function for opening the fingers. CAUTION: This function will
	* open alla the fingers simultaneously if puck=15.
	* @param puck The ID of the puck we want to open. With puck=11-14
	* fingers 1-3 and spread will open, and with puck=15 all of them
	* will open.
	* @return An error code. Returning 0 means we have no error.
	*/
	int open(int puck);


	/**
	* The function that initiliaze the bus and the hand. Actually is 
	* a combination of CANBus::init() and BarrettHand::init(). It also
	* sets the properties to their initial values.
	*/
	void initHand();

	/**
	* The function that terminates the hand. It actually call the
	* BarrettHand::close() and the BarrettHand::initValues().
	* @see BarrettHand::close()
	* @see BarrettHand::initValues()
	*/
	void terminate();

	/**
	* The function that checks if one puck is moving or not.
	* @param puck The ID of the puck we want to check.
	* @return A boolean. If true the puck is not moving. If false
	* the puck is still moving.
	*/
	bool doneMoving(int);

	/**
	* The function that waits as long as the puck is moving.
	* @param puck The ID of the puck we want to wait until it stops.
	*/
	void waitDoneMoving(int);

	/**
	* The function
	* @param arg
	* @return An error code.
	*/
	void goToRelaxedPos();
	void initSG(bool);
	double getSG(int finger, bool);
	void setHLSG(int finger, double HSGperc, double LSGperc);

/** Testing and Benchmarking Tools */
	void testTiming();
	void test();
	void testingGetPosition();
	void logger();
	
/** Applications and uses of BarrettHand */
	void fetchAndRelease(int);
	void fetchAndRelease2(int);
	void handShake(HandState);
	void precisionGrasp();
	void precisionGrasp2();
	void distortionControl(double distortion);
	void simpleGrasp(int);
	void holdGrasp(int);

/** Under development and TODOs */	
	void release(int);
	void release2(int topFinger);
	void detectBreakaway(HandState* state);
	void staple();
	void simpleCylinderGrasp();
	void syringe();
	void touchAndGrab(HandState state);

/** Auxiliary functions */
	double calculateVariance(int matrix[]);
};


