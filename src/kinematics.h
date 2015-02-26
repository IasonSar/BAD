#include <cmath>
#include <vector>
#include "BarrettHand.h"


using namespace std;

#define HEIGHT 4
#define WIDTH 4

class Fingertip {
public:
	int finger1[3];
	int finger2[3];
	int finger3[3];
};

class Joints {
public:
	int finger1[4];
	int finger2[4];
	int finger3[4];
};

class HandState {
	BHand *hand;
	
	vector<vector<double> > jointPos; //3 fingers 3 joints (is actually Theta_i angle in rads)
	int F3; // outer joint offset
	vector<double> strain;
	
	bool brokenAway;
	
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
