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

#include <cmath>
#include <vector>
#include "BarrettHand.h"

using namespace std;

#define HEIGHT 4
#define WIDTH 4

/**
* A class for the current state of the hand which includes data, as finger position
* and strain gauges values.
*/
class HandState {
	BHand *hand; /**< A pointer to the object of the hand in use */	
	vector<vector<double> > jointPos; /**< 3x3 matrix. 3 fingers with 3 joints each. (elements are the angle Theta_i in rads)  */ 
	int F3; /**< The outer link offset, about 40 degrees.  */
	vector<double> strain; /**< 3x1 vector for the SG values. 3 fingers with one sensor each.  */
	
	bool brokenAway; /**< If true the Torque Switch mechanism has been engaged. */
	
public:

	HandState(BHand* myHand);
	~HandState();
	
	void setJointPos(vector<vector<double> > newJointPos);
	vector<vector<double> > getJointPos();
	
	void setStrain(vector<double> newStrain);
	vector<double> getStrain();
	
	void update();
	void printOut();
	
	double innerLinkJointAngle(int JP);
	double outerLinkJointAngle(int P, int JP);
};

class Kinematics {
	// Aw, A1, A2, A3, Dw, D3: distances in mm
	// F2, F3: angles in degrees
	int r[3], j[3];
	double Aw, A1, A2, A3, Dw, D3, F2, F3;
	double A[3][4], a[3][4], D[3][4], theta[3][4];
	
public:
	Kinematics();
	~Kinematics();
	
//	forward();
	vector<vector<double> > multiplyMatrices(vector<vector<double> > T1, vector<vector<double> > T2);
	vector<vector<double> > HTMatrix(double theta, double A, double alpha, double d);
	vector<vector<double> > findHTmatrix(int finger, HandState state);
	vector<vector<double> > homogeneousTF(int finger, HandState state);
	vector<vector<double> > inverseKinematics(int finger, double *p);

	
	//int init();
	//int reset();
};
