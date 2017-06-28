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

#include <bad/BarrettHand.h>
#include <bad/kinematics.h>
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
    
	/****** Basic functionality of BarretHand */

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
	* The function that move the fingers to a humanoid relaxed position.
	*/
	void goToRelaxedPos();


	/**
	* The function that initializes the SG values. Initially it reads the
	* SG values with no load. Then the user will be asked to push the 
	* fingers to measure the maximum value (about 2kg of force).
	* @param pressing. If true the user has to apply a force at the
	* fingertips. If false a default value (4000) is being noted as
	* the maximum value.
	*/
	void initSG(bool);

	/**
	* Function that returns the SG value of one puck.
	* @param finger The ID of the puck whose SG is needed.
	* @param perc If true the returned value is a percentage, with 
	* zero having the minimum SG (as the initSG() measured) and with
	* 100% the maximum SG. If false the true value of the sensor is
	* being returned, but first is being filtered from noise.
	* @return The SG value.
	* @see BAD::initSG()
	*/
	double getSG(int finger, bool);

	/**
	* Function that converts SG values from percentages to raw sensor
	* values and write them into the pucks.
	* @param finger The ID of the puck.
	* @param HSGperc The value of HSG as a percentage. If -1 it will not
	* write anything.
	* @param LSGperc The value of LSGperc as a percentage. If -1 it will not
	* write anything.
	*/
	void setHLSG(int finger, double HSGperc, double LSGperc);

	/****** Testing and Benchmarking Tools */

	/**
	* Function that has to run as an asychronous thread in parallel. It
	* creates a log file in the log directory (/logFiles) and asks the 
	* robot for data for 30 seconds. Data is position and strain gause.
	* The log files has the form:
	* time p1 p2 p3 p_spread sg1 sg2 sg3
	* Note, that running logger in parralel with the application might
	* reduce performance, as the logger uses bandwidth.
	*/
	void logger();
	
	/****** Applications and uses of BarrettHand */

	  
	/**
	* Function tha perfoms a simple grasp, whole or precision with with a 
	* zero spread.
	* @param HSG_value The threshold value of strain gause that above it, 
	* the hand will stop, considering that the grasp has been completed.
	*/
	void simpleGrasp(int);

	/**
	* Function that hold the grasp implementing a position control.
	* @param force An int that represents the SG value that the user
	* wants to maintain at the fingertips.
	*/
	void holdGrasp(int);

	/**
	* Function to release an object with a human-friendly way. The robot
	* will feel the forces of the top finger, and if it feels that the 
	* human is trying to take the object it will release it gently.
	* This function is using a threshold critirion for make this 
	* decision.
	* @param topFinger The ID of the finger's puck that is on top.
	*/
	void release(int);
	
	/**
	* Function that uses a grasping function (eg simplegrasp())
	* and a release function (eg release1, or release2()) in order
	* to perform a simple handover interaction.
	*/
	void handover();
	
	/**
	* Function that make the non-backdrivable fingers more friendly
	* towards a human during a handshake. It makes the fingers
	* more compliant.
	* @param HandState A Handstate object.
	*/
	void handShake(HandState);

	/**
	* Function that performs a rudimental precision grasp.
	*/	
	void precisionGrasp();

	/**
	* Function that performs an example of distortion control of an object.
	* @param distorion A double with a value in the range 0-1. If equals 0
	* the robot will not distort the object. If equals 1 the robot will
	* apply the maximum force in order to fully distort the object.
	*/
	void distortionControl(double distortion);

	/****** Auxiliary functions */

	/**
	* Function that calculate the variance of a matrix's elements.
	* @param matrix[] The actual matrix.
	* @return The variance as a double.
	*/
	double calculateVariance(int matrix[]);

	
	/** Under development and TODOs. This code is NOT properly documented 
		because it does not consist solid implementations. Be caution
		if you want to use it. */	
	void release2(int topFinger);
	void fetchAndRelease(int);
	void fetchAndRelease2(int);
	void detectBreakaway(HandState* state);
	void precisionGrasp2();
	void staple();
	void simpleCylinderGrasp();
	void touchAndGrab(HandState state);
};


