#include <iostream>
#include <fstream> //stream class to read/write on files
#include <boost/array.hpp>

#include <boost/numeric/odeint.hpp>

#include "HighLevelBAD.h"


using namespace std;
using namespace boost::numeric::odeint;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const double mass = 100;
const double damping = 1;
const double spring = 2;
double sg_feedback = 2000;
ofstream data;

typedef boost::array< double, 2 > state_type;

void admittance(const state_type &x, state_type &dxdt, double t)
{
	dxdt[0] = x[1];
	dxdt[1] = (sg_feedback - damping*x[1] - spring*x[0])/mass;
}

void write_adm(const state_type &x, const double t)
{
	data << t << "," << x[0] << endl;
}

int main(int argc, char **argv)
{

	string hand_type = "BH8-282";
			
/* Create the CAN bus and the hand objects */
	CANbus bus;
	BHand hand(&bus, hand_type, &mutex);
	HandState state(&hand);
	BAD driver(&hand);
	Kinematics kin;
									
/** Now you can write your own code using BAD API */	
										
	driver.initHand();



	data.open("data.txt");

	state_type x_init = {0, 0};
	integrate(admittance, x_init, 0.0, 25.0, 0.001, write_adm);

	data.close();
}


