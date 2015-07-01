#include <iostream>
#include <fstream> //stream class to read/write on files
#include <boost/array.hpp>

#include <boost/numeric/odeint.hpp>

#include "HighLevelBAD.h"


using namespace std;
using namespace boost::numeric::odeint;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/** Define admittance parameters */
const double inertia = 1;
const double damping = 10;
const double stiffness = 0.0001;

double sg_feedback = 0;
ofstream data; //for writing data inside a file

typedef boost::array< double, 2 > state_type;
state_type dstate;


/** Method that defines our admittance system */
void admittance(const state_type &x, state_type &dxdt, double t)
{
	dxdt[0] = x[1];
	dxdt[1] = (sg_feedback - damping*x[1] - stiffness*x[0])/inertia;
}

/** Observer that writes the solution of the differential equation
	somewhere */
void write_adm(const state_type &x, const double t)
{
	dstate[0] = x[0];
	dstate[1] = x[1];
	//data << t << "," << x[0] << endl;
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
									
/** Init the hand and move the finger 1 to an initial position.
	Then wait for 2 sec. */										
	driver.initHand();
	hand.setProperty(FINGER1, M, 30000);

	usleep(2*1000*1000);

/** Read current position (initial position) */
	int x_cur;
	hand.getProperty(FINGER1, P, &x_cur);

/** 
	t0: the initial time in sec
	time: current time
	time_prev: the time in the previous step
	T: the total time running the controller 
	t_thres: a threashold time to keep the control cycle fixed
	Ts = the duration of the control cycle */
	double t0 = 0, time = 0, time_prev = 0, T=50, t_thres=0, Ts = 0.05;

/** Define structs and variables for measuring time. */
	struct timeval start, end;
	long mtime, seconds, useconds;

/** Start t = 0 */
	gettimeofday(&start, NULL);
		
/** Set the mode of the Bhand to PID Mode and set the PID gains*/
	hand.setProperty(FINGER1, KP, 300);
	hand.setProperty(FINGER1, KD, 2500);
	hand.setProperty(FINGER1, TSTOP, 0);
	hand.setProperty(FINGER1, MODE, MODE_PID);

/* A nominal reference position x[0] = e = x_cur - x_ref **/
	int x_ref = 100000;

	state_type x_init = {(x_cur - x_ref), 0};


/** Read current time as t0 */
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	t0= mtime/1000;




/** Control Loop */
	while(true)
	{
		gettimeofday(&end, NULL);
		sg_feedback = driver.getSG(FINGER1, false) - 2000;
		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000.0 + useconds/1000.0) + 0.5;
		time = mtime/1000.0 - t0;
	
		if (time > T + 10) break;
		if (time > t_thres) {
			if (time < T) {

				integrate(admittance, x_init, time_prev, time, 0.01, write_adm);
				x_cur = x_ref + dstate[0];
				cout << time << "\t" << sg_feedback << "\t" << x_init[0] << "," << x_init[1] << "\t" << time_prev << "," << time << "\t"<< x_cur << endl;
				hand.setProperty(FINGER1, P, (int)x_cur);
				x_init = dstate;
				t_thres = t_thres+Ts;
				time_prev = time;
			}
			
		}
			 
	}


	data.open("data.txt");


	data.close();
}


