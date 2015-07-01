#include <iostream>
#include <fstream> //stream class to read/write on files
#include <boost/array.hpp>

#include <boost/numeric/odeint.hpp>

#include "HighLevelBAD.h"


using namespace std;
using namespace boost::numeric::odeint;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const double mass = 1;
const double damping = 1;
const double spring = 1;
double sg_feedback = 0;
ofstream data;

typedef boost::array< double, 2 > state_type;
state_type dstate;

void admittance(const state_type &x, state_type &dxdt, double t)
{
	dxdt[0] = x[1];
	dxdt[1] = (sg_feedback - damping*x[1] - spring*x[0])/mass;
}

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
									
/** Init the hand and move the finger 1 */										
	driver.initHand();
	hand.setProperty(FINGER1, M, 30000);

	usleep(2*1000*1000);

/** Read current position */
	int pos;
	hand.getProperty(FINGER1, P, &pos);

	struct timeval start, end;
	long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	double t0 = 0, time = 0, time_prev = 0, T=50, t_thres=0;
	double Ts = 0.02; //20 ms control cycle
	


/** Set the mode of the Bhand to PID Mode and set the PID gains*/
	hand.setProperty(FINGER1, KP, 300);
	hand.setProperty(FINGER1, KD, 2500);
	hand.setProperty(FINGER1, TSTOP, 0);
	hand.setProperty(FINGER1, MODE, MODE_PID);

	int x_ref = 100000;

	state_type x_init = {(pos - x_ref), 0};

/** Control Loop */
	while(true)
	{
		sg_feedback = driver.getSG(FINGER1, false) - 2000;
		gettimeofday(&end, NULL);
		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		
		mtime = ((seconds) * 1000.0 + useconds/1000.0) + 0.5;
		time = mtime/1000.0 - t0;
	
		if (time > T + 10) break;
		if (time > t_thres) {
			if (time < T) {
				integrate(admittance, x_init, time_prev, time, 0.001, write_adm);
				pos = x_ref + dstate[0];
				hand.setProperty(FINGER1, P, (int)pos);
				x_init = dstate;
			}
			
			t_thres = t_thres+Ts;
		}
			 
		time_prev = time;
	}


	data.open("data.txt");


	data.close();
}


