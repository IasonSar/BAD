#include "kinematics.h"


Kinematics::Kinematics()
{
    Aw = 25;
    A1 = 50;
    A2 = 70;
    A3 = 50;
    Dw = 84;
    D3 = 9.5;
    F2 = 0; //to 0.4
    F3 = 42;
    
    r[0] = -1; r[1] = 1; r[2] =  0;
    j[0] =  1; j[1] = 1; j[2] = -1;
    // D-H Link parameters
    // note: finger = = meaning finger1 and so on
    /*
    for (int finger=0; finger<3; finger++) {
    	A[finger][0] = r[finger]*Aw;
    	A[finger][1] = A1;
    	A[finger][2] = A2;
    	A[finger][3] = A3;
    	
    	a[finger][0] = 0;
    	a[finger][1] = PI/2;
    	a[finger][2] = 0;
    	a[finger][3] = -PI/2;
    	
    	D[finger][0] = Dw;
    	D[finger][1] = 0;
    	D[finger][2] = 0;
    	D[finger][3] = D3;
    }
    */
}
Kinematics::~Kinematics(){}

vector<vector<double> > Kinematics::multiplyMatrices(vector<vector<double> > T1, vector<vector<double> > T2)
{
	vector<vector<double> > result;
	result.resize(HEIGHT);
  	for (int i = 0; i < HEIGHT; i++)
    	result[i].resize(WIDTH);
    //TODO: Check if T1, T2 is 4x4 matrices or expand functionality for any dimensions
    	
    for (int i = 0; i< HEIGHT; i++)
    	for (int j = 0; j < WIDTH; j++) {
    		result[i][j] = 0;
    		for (int k = 0; k < HEIGHT; k++)
    			result[i][j] += T1[i][k]*T2[k][j];
    	}
        
    return result;
}

vector<vector<double> > Kinematics::HTMatrix(double theta, double A, double alpha, double d)
{
	vector<vector<double> > g;
	g.resize(HEIGHT);
  	for (int i = 0; i < HEIGHT; i++)
    	g[i].resize(WIDTH);
    
    g[0][0] = cos(theta); 
    g[0][1] = -sin(theta); 
    g[0][2] = 0; 
    g[0][3] = A;
    
    g[1][0] = sin(theta)*cos(alpha); 
    g[1][1] = cos(theta)*cos(alpha);
    g[1][2] = -sin(alpha);
    g[1][3] = -sin(alpha)*d;
        
    g[2][0] = sin(theta)*sin(alpha);  
    g[2][1] = cos(theta)*sin(alpha); 
    g[2][2] = cos(alpha);
    g[2][3] = cos(alpha)*d;
        
    g[3][0] = 0;
    g[3][1] = 0;
    g[3][2] = 0;
    g[3][3] = 1;
    
    return g;
    
}


/*
//TODO find the D-H modified homogeneous transformation matrix 
vector<vector<double> > Kinematics::findHTmatrix(int finger, HandState state)
{
	int index = finger - 11;
	
	// A vector Theta that contains the angles theta of D-H kinematics
	double theta[4];
	
	//2x2 array that contains the current values of the joints angles according to current state
	vector<vector<double> > currentJoints;
	currentJoints.resize(3);
  	for (int i = 0; i < 3; i++)
    	currentJoints[i].resize(3);
    	
   	currentJoints = state.getJointPos();
   	
   	theta[0] = r[index]*currentJoints[index][0] - (PI/2)*j[index];
   	theta[1] = currentJoints[index][1];
   	theta[2] = currentJoints[index][2];
   	theta[3] = 0;
	
	/*
	theta[0] = r[finger]*Theta[0] - (PI/2)*j[finger];
    theta[1] = Theta[1] + F2;
    theta[2] = Theta[2] + F3;
    theta[3] = 0;
    */
    
    /*
    // The matrix g, the actual homogeneous transformation matrix
    vector<vector<double> > g;
	g.resize(HEIGHT);
  	for (int i = 0; i < HEIGHT; i++)
    	g[i].resize(WIDTH);
    
    vector<vector<double> > temp;
	temp.resize(HEIGHT);
  	for (int i = 0; i < HEIGHT; i++)
    	temp[i].resize(WIDTH);
    
    // find g01 (i=1)	
    g = HTMatrix(theta[1], A[index][0], a[index][0], D[1]);
    
    for (int i = 2; i < 4; i++) {
    	temp = HTMatrix(theta[i], A[index][i-1], a[index][i-1], D[i]);
    	g = multiplyMatrices(g, temp);    	
    }
    
    
    
      
	
}

*/



vector<vector<double> > Kinematics::homogeneousTF(int finger, HandState state)
{

	int index = finger - 11;
	double _a[4], _d[4], _theta[4];
	
	
	// create a 3x3 matrix to put the current angles of the joint according to HandState
	vector<vector<double> > currentJoints;
	currentJoints.resize(3);
  	for (int i = 0; i < 3; i++)
    	currentJoints[i].resize(3);
    
    // Create a matrix to put the homogeneous transformation
    vector<vector<double> > matrix;
	matrix.resize(HEIGHT);
  	for (int i = 0; i < HEIGHT; i++)
    	matrix[i].resize(WIDTH);
    	
    currentJoints = state.getJointPos();
    
    
   	_a[0] = r[index]*Aw;
    _a[1] = A1;
    _a[2] = A2;
    _a[3] = A3;
    
   	_d[0] = Dw;
    _d[1] = 0;
    _d[2] = 0;
    _d[3] = D3;
    
   	_theta[0] = r[index]*currentJoints[index][0] - (PI/2)*j[index];
    _theta[1] = currentJoints[index][1];
    _theta[2] = currentJoints[index][2];
    _theta[3] = 0;
    
    
    double c1, c2, c3, s1, s2, s3, c23, s23; 
    c1 = cos(_theta[0]);
    c2 = cos(_theta[1]);
    c3 = cos(_theta[2]);
    s1 = sin(_theta[0]);
    s2 = sin(_theta[1]);
    s3 = sin(_theta[2]);
    
    c23 = cos(_theta[1]+_theta[2]);
    s23 = sin(_theta[1]+_theta[2]);
    
    matrix[0][0] = c1*c23;
    matrix[0][1] = -s1;
    matrix[0][2] = (-c1)*s23;
    matrix[0][3] = _a[3]*c1*c23 - _d[3]*c1*s23 + _a[2]*c1*c2 + _a[1]*c1 + _a[0];
    
    matrix[1][0] = s1*c23;
    matrix[1][1] = c1;
    matrix[1][2] = -s1*s23;
    matrix[1][3] = _a[3]*s1*c23 - _d[3]*s1*s23 + _a[2]*s1*c2 + _a[1]*s1;
    
    matrix[2][0] = s23;
    matrix[2][1] = 0;
    matrix[2][2] = c23;
    matrix[2][3] = _a[3]*s23 + _d[3]*c23 + _a[2]*s2 + _d[1];
    
    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;
    
    return matrix;
}






/******** HandState Functions ********/
HandState::HandState(BHand* myHand)
{	
	hand = myHand; 
	
	// Initialize the jointPos to a 3x3 matrix. 3 fingers with 3 joints
	jointPos.resize(3);
  	for (int i = 0; i < 3; i++)
    	jointPos[i].resize(3);
    
    // Initialize strain to a 3x1 vector. 3 fingers with one torque sensor each
    strain.resize(3);
    
    brokenAway = false;
    F3 = 42; 
}


HandState::~HandState() {}

void HandState::setJointPos(vector<vector<double> > newJointPos)
{	
	
	
	//Check if newJointPos is a 3x3 matrix
	bool flag = false;	
	
	for (int i = 0; i < newJointPos.size(); i++)
    	if (newJointPos[i].size() != 3)
    		flag = true;
    
    //If not throw an error and return
	if (newJointPos.size() != 3 | flag) {
		cout << "Error in HandState::SetJointPos" << endl;
		return;
	}
	
	/*
	cout << newJointPos[0][0] << " " << newJointPos[0][1] << " " << newJointPos[0][2] << endl;
	cout << newJointPos[1][0] << " " << newJointPos[1][1] << " " << newJointPos[1][2] << endl;
	cout << newJointPos[2][0] << " " << newJointPos[2][1] << " " << newJointPos[2][2] << endl;
	*/
	
	for (int i = 0; i < 3; i++)
		for (int ii = 0; ii < 3; ii++)
			this->jointPos[i][ii] = newJointPos[i][ii];
	
}


vector<vector<double> > HandState::getJointPos()
{	
	
	vector<vector<double> > pos;
	pos.resize(3);
  	for (int i = 0; i < 3; i++)
    	pos[i].resize(3);
    	
	
	for (int i = 0; i < 3; i++)
		for (int ii = 0; ii < 3; ii++)
			pos[i][ii] = jointPos[i][ii];
	
	return pos;
	
}


void HandState::setStrain(vector<double> newStrain)
{	

	//Check if newStrain is a 3x1 vector    
    //If not throw an error and return
	if (newStrain.size() != 3) {
		cout << "Error in HandState::SetStrain" << endl;
		return;
	}
	
	for (int i = 0; i < 3; i++)
			strain[i] = newStrain[i];
	
}


vector<double> HandState::getStrain()
{	
	
	vector<double> s;
	s.resize(3);
    	
	
	for (int i = 0; i < 3; i++)
			s[i] = strain[i];
	
			
	return s;
}

//update the state of the hand.
void HandState::update()
{	
	int temp1, temp2;
	double p[3], jp[3];
	int spread;
	
	vector<vector<double> > Theta;
	Theta.resize(3);
  	for (int i = 0; i < 3; i++)
    	Theta[i].resize(3);
    
    vector<double> strain;
	strain.resize(3);
	
	
	// Ask the hand to give you position properties
	for (int puck=FINGER1; puck <= FINGER3; puck++) {
		hand->getDualPackedPosition(puck, &temp1, &temp2);
		p[puck-11] = temp1;
		jp[puck-11] = temp2;
		
		hand->getProperty(puck, SG, &temp1);
		strain[puck-11] = temp1;
		
	}
	
	hand->getProperty(SPREAD, P, &temp1);
	spread = temp1;
	
	
	for (int finger=FINGER1; finger <= FINGER3; finger++) {
		if (finger!=FINGER3) {
			Theta[finger-11][0] = ((double)spread/MAX_TICKS_SPREAD)*PI;
		}
		else
			Theta[finger-11][0] = 0;
		
		
		Theta[finger-11][1] = innerLinkJointAngle(jp[finger-11]);
		
		
		//cout << "p=" << p[finger-11] << ", 
		Theta[finger-11][2] = outerLinkJointAngle(p[finger-11], jp[finger-11]);
		
		//cout << "Finger" << finger-10 << ": " << Theta[finger-11][0] << " " << Theta[finger-11][1] << " " << Theta[finger-11][2] << endl;
	}
	
	setJointPos(Theta);
	setStrain(strain);
	

}

void HandState::printOut()
{
	
	cout << "== FINGER1 ==" << endl;
	cout << "Joint Positions rads: [" << this->jointPos[0][0] << ", " << this->jointPos[0][1] << ", " << this->jointPos[0][2] << "]" << endl;
	cout << "Joint Positions deg: [" << this->jointPos[0][0]*(180/PI) << ", " << this->jointPos[0][1]*(180/PI) << ", " << this->jointPos[0][2]*(180/PI) << "]" << endl;
	cout << "Strain: " << this->strain[0] << endl;
	
	cout << "== FINGER2 ==" << endl;
	cout << "Joint Positions rads: [" << this->jointPos[1][0]<< ", " << this->jointPos[1][1] << ", " << this->jointPos[1][2] << "]" << endl;
	cout << "Joint Positions deg: [" << this->jointPos[1][0]*(180/PI) << ", " << this->jointPos[1][1]*(180/PI) << ", " << this->jointPos[1][2]*(180/PI) << "]" << endl;
	cout << "Strain: " << this->strain[1] << endl;
	
	cout << "== FINGER3 ==" << endl;
	cout << "Joint Positions rads: [" << this->jointPos[2][0] << ", " << this->jointPos[2][1] << ", " << this->jointPos[2][2] << "]" << endl;
	cout << "Joint Positions deg: [" << this->jointPos[2][0]*(180/PI) << ", " << this->jointPos[2][1]*(180/PI) << ", " << this->jointPos[2][2]*(180/PI) << "]" << endl;
	cout << "Strain: " << this->strain[2] << endl;

}

double HandState::innerLinkJointAngle(int JP)
{
	return (double)JP*((2*PI)/(4096*50));
}

double HandState::outerLinkJointAngle(int P, int JP)
{
	double k;
	k = innerLinkJointAngle(JP);
	
	return (double)P*((16*2*PI)/(4096*30*50)) + 2*PI/9 - k;
}


//TODO
vector<double> inverseKinematics(int finger, double *p)
{
/*
	vector<double> theta;
	theta.resize(3);
	xd = p[0];
	yd = p[1];
	zd = p[2];
	
	theta[0] = atan2(yd,xd);
	
	
	return theta;
*/
}
























