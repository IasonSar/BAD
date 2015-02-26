/**
	HighlevelBAD.cpp

	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 28/02/2015
	
	HighlevelBAD.cpp is part of BAD (Barretthand AUTh Driver). BAD is a 
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

/******** BAD Functions ********/
BAD::BAD(BHand* myHand)
{
    hand = myHand;
    msg_type = "[BAD] ";
    err_type = "[Error] BAD::";  
    sg_inited = false;  
}
BAD::~BAD(){};




int BAD::open(int puck)
{   
    if (puck == FINGER1 | puck == FINGER2 | puck == FINGER3 | puck == SPREAD) {
        hand->setProperty(puck, OT, 0);
        hand->setProperty(puck, CMD, CMD_OPEN);
    }
    else if (puck == HAND) {
        for (int myPuck=FINGER1; myPuck <= SPREAD; myPuck++) {
            hand->setProperty(myPuck, OT, 0);
            if (myPuck == SPREAD) usleep(1*1000000);
            hand->setProperty(myPuck, CMD, CMD_OPEN);
        }
    }
    else {
        cerr << err_type << __FUNCTION__ << ": Puck no." << puck << " wasn't found. Try 11-15." << endl;
        return MD_ERROR_ARGS;
    }
    
    usleep(2*1000000);
    hand->setProperty(SPREAD, MODE, MODE_IDLE);
    return MD_ERROR_OK;
}

void BAD::initHand()
{
	if (hand->init() != MD_ERROR_OK) {
	    cerr << err_type << __FUNCTION__ << ": hand.init() failed to return" << endl;
		return;
	}
	
	if (hand->initPropValues() != MD_ERROR_OK) {
	    cerr << err_type << __FUNCTION__ << ": hand.initPropValues() failed to return" << endl;
	    return;
	}
}

void BAD::terminate() 
{
    hand->close();
    hand->initPropValues();
}

//TODO read position and decide when to open fingers more fast
void BAD::fetchAndRelease(int topFinger)
{	

	int rc;
/** wait 1 second and check if hand is initialized */
	usleep(1*1000000);
    if (!hand->initiliazed)
        hand->init();
    else
        open(HAND);
    

    
    cout << msg_type << "I will close the fingers in order to grab the bottle." << endl;
    
/** Read SG absence of force, and set velocity of the fingers */
	int sg[3];
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->getProperty(puck, SG, &sg[puck-FINGER1]);

	usleep(500*1000);
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }


/** Start grasping. When every fingers feels the object stop and hold the grasp */
	bool flag[3] = {true, true, true};
	int current_sg;
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->getProperty(puck, SG, &current_sg);
			if (current_sg - sg[puck-FINGER1] > 500) {
				hand->setProperty(puck, MODE, MODE_IDLE);
				flag[puck-FINGER1] = false;
			}
		}

	}

	hand->setProperty(SPREAD, MODE, MODE_IDLE);

	
	cout << msg_type << "I grabbed the bottle. You can move the KUKA arm towards a person and the the person can receive the bottle" << endl;
	
/** Check if we have a force in the top finger and release the grasp */
	int init_position;
	int position;
	

	hand->getProperty(topFinger, SG, &sg[0]);
	hand->getProperty(topFinger, P, &init_position);

	
	while(true) {
		hand->getProperty(topFinger, SG, &current_sg);
		hand->getProperty(topFinger, P, &position);		
		if (current_sg - sg[0] > 150)
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -20);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}	
		if ( (double)(init_position-position) / (double)(init_position) > 0.2 ) 
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -60);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}

		if ( position < 40000 )
			break;
	}
	

	for (int puck=FINGER1; puck<=FINGER3; puck++)
		hand->setProperty(puck, MODE, MODE_IDLE);

	
	cout << msg_type << "Always a pleasure!" << endl;
	

}

bool BAD::doneMoving(int puck)
{
	int mode, hold;
	if (puck != HAND) {
				
		hand->getProperty(puck, MODE, &mode);
		hand->getProperty(puck, HOLD, &hold);
		//cout << "MODE: " << mode << ", hold: " << hold << endl;
		if (mode!=MODE_IDLE & (mode==MODE_PID | hold==0)) {
			//cerr << err_type << __FUNCTION__ << " I return false. is not moving\n";	
			return false;
		}
		return true;
	}
	else {
		//TODO: ulopoihsh ths done moving an dwthei puck=HAND=15, prepei na tsekarei ola ta xeria
		cout << "doneMoving has not been implemented for the whole hand yet" << endl;
	}
}

void BAD::waitDoneMoving(int puck)
{
	while (!doneMoving(puck))
		usleep(1*1000);
	
	//cout << " FINGER" << puck-FINGER1 << " stopped" << endl; 
}

void BAD::testingGetPosition()
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        hand->init();
    else
        open(HAND);
    
    cout << "Finger starts to moving.." << endl;    
    hand->setProperty(FINGER1, CMD, CMD_CLOSE);   
    
    int mode;
    
    while(!doneMoving(FINGER1)) {
    	int pos, jpos;
    	hand->getDualPackedPosition(FINGER1, &pos, &jpos);
    	cout << pos << ", " << jpos << endl;
    	
    	
    	//cout << "Kineitai";
    }
    cout << "Movement is over" << endl;
    
    
}

// TODO: pretty crappy function. needs fixing
void BAD::detectBreakaway(HandState* state)
{	
	//TODO the function must return even if no breakaway has happen
	vector<vector<double> > joints;
	joints.resize(3);	
	for (int i = 0; i < 3; i++)
    	joints[i].resize(3);
    	
    bool noBreak = true;
	int brokenFinger = 0;
	usleep(200*1000); // wait 200ms because in the beginning of the movement the secondary
					  // encoder gives garbage
	while(noBreak) {
		state->update();
		//state->printOut();
		joints = state->getJointPos();
		
		
		for (int finger = 0; finger < 3; finger++) {
			//cout << "\033[1;31mDEBUG\033[0m " << (joints[finger][1])/(joints[finger][2]-40*(PI/180)) << endl;
			if ( (joints[finger][1])/(joints[finger][2]-40*(PI/180)) < 2.8	 ) {
				noBreak = false;
				brokenFinger = finger + 11;
			}
		}
	}
	state->update();
	cout << "\033[1;31mBREAK AWAY at finger\033[0m " << brokenFinger << endl;
	//cout << "BREAK AWAY at finger " << brokenFinger << endl;
	//state->printOut();
	
}


void BAD::testTiming()
{

	
	
}


/** handShake()
*	Takes the strain as feedback and controls the velocity of the fingers.
**/
void BAD::handShake(HandState state)
{
	
	
	struct timeval start, end;
    double mtime, seconds, useconds;
    seconds = 0;
    gettimeofday(&start, NULL);
	
	vector<double> init_strain;
	init_strain.resize(3);
	
	
	state.update();
	init_strain = state.getStrain();
	usleep(100*1000);
	hand->setProperty(SPREAD, CMD, CMD_CLOSE);

	vector<double> strain;
	strain.resize(3);
	
	vector<vector<double> > pos;
	pos.resize(3);
  	for (int i = 0; i < 3; i++)
    	pos[i].resize(3);
	
	cout << "[BAD] HandShake: Press ENTER when you wanna shake hands." << endl;
    cin.ignore();
	hand->setProperty(FINGER1, M, 145000);
	hand->setProperty(FINGER2, M, 145000);
	hand->setProperty(FINGER3, M, 145000);
	
	usleep(1*1000000);
	
	cout << "HandShake: I close my fingers" << endl;
	int velocity;
	
	gettimeofday(&start, NULL);
	while(seconds < 10)
	{
		gettimeofday(&end, NULL);		
		seconds  = end.tv_sec  - start.tv_sec;
		double error;
		state.update();
		strain = state.getStrain();
		pos = state.getJointPos();
		
		for (int finger=FINGER1; finger <= FINGER3; finger++) {
			// just a fix for finger 1 until we calibrate it with the screwdriver
			if (finger == FINGER1 )
				strain[finger-11] = strain[finger-11] + 200;
				
			error = strain[finger-11] - init_strain[finger-11]-200;
			//cout << finger << ": " << error << endl;
				velocity = -(error/1000)*200;
			//cout << "Velocity of finger: " << finger << " " << velocity << endl;
		
        	hand->setProperty(finger, V, velocity);
    		hand->setProperty(finger, TSTOP, 0);
       		hand->setProperty(finger, MODE, MODE_VELOCITY);
       		
       		if (pos[finger-11][0] > 140000)
       			hand->setProperty(finger, MODE, MODE_IDLE);
		}
		
	}
	//waitDoneMoving(SPREAD);
	hand->setProperty(SPREAD, MODE, MODE_IDLE);
	for (int finger=FINGER1; finger <= FINGER3; finger++) 
		hand->setProperty(finger, MODE, MODE_IDLE);
}

void BAD::goToRelaxedPos()
{	
	for (int finger=FINGER1; finger <= FINGER3; finger++) {
		hand->setProperty(finger, M, 100000);
	}
}

void BAD::touchAndGrab(HandState state)
{
	goToRelaxedPos();

}



void BAD::test()
{
	while(true){
		cout << "I am test111" << endl;
		usleep(1*1000000);
	}
	pthread_exit(NULL);
}


/** logger()
*   creates a logfile in the log directory and ask the robot for data for 30 
*	sec, that writes in the log file. Data is position and strain gause. 
*	The log files has this form :
*	time p1 p2 p3 p_spread sg1 sg2 sg3
*   Note: You have to run it as an asychronous thread, in parallel with the
*	main application
**/
void BAD::logger()
{
	char ch;	
	
/** Define time parameters and record initial time as "start"*/
	struct timeval start, end;
    double mtime, seconds, useconds;
    gettimeofday(&start, NULL);

/** 1. Define time parameters to create an id for log file
	2. Define parameters for writing to file */
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int year, month, day, hour, minute, second;
	year = 1900+ltm->tm_year;
	month = 1 + ltm->tm_mon;
	day = ltm->tm_mday;
	hour = ltm->tm_hour;
	minute = ltm->tm_min;
	second = ltm->tm_sec;
	
	// The ID of the log file has the form YMDHMS
	string id;
	stringstream out;
	out << year << month << day << hour << minute << second;
	id = out.str();
	
	// Create the whole name of log file
	string file_name2 = "log_files/bad_" + id + ".log";
	
	// Convert name from string to char* in order to use it with ofstream::open
	const char * file_name = file_name2.c_str();	
	
	ofstream myfile;
	myfile.open (file_name);
	
	cout << "[BAD Logger] is starting to write at file " << file_name << "..." << endl;
	cout << "[BAD Logger] will terminate after 1 minute of collecting data..." << endl;
	//cout << "[BAD Logger] When the experiment is finished \033[1;31mtype ESC and press ENTER\033[0m to terminate BAD Logger." << endl;

	
/** Define matrices to read the Hand State */
	vector<vector<double> > joints;
	joints.resize(3);	
	for (int i = 0; i < 3; i++)
    	joints[i].resize(3);
    	
    vector<double> strain;
	strain.resize(3);
	int p1, p2, p3, p4, sg1, sg2, sg3;

	mtime = 0;
	
	int rc;
/** Begin the loop. Run for 60 seconds) */
	 while(mtime < 15) {		
	/** Record current time and calculate seconds from initial time */
	    gettimeofday(&end, NULL);		
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
    	mtime = ((seconds) * 1000 + useconds/1000.0);
    	mtime = mtime/1000;

	/** Read the data from robot and write them in log file */	
		hand->getProperty(FINGER1, SG, &sg1);
		hand->getProperty(FINGER2, SG, &sg2);
		hand->getProperty(FINGER3, SG, &sg3);
		hand->getProperty(FINGER1, P, &p1);
		hand->getProperty(FINGER2, P, &p2);
		hand->getProperty(FINGER3, P, &p3);
		hand->getProperty(SPREAD, P, &p4);
		myfile << mtime << " " << p1 << " " << p2 << " " << p3 << " " << p4  << " " << sg1-2000 << " "  << sg2-2000 << " " << sg3-2000 << endl;
		//cout << " " << p1 << " " << p2 << " " << p3 << " " << p4  << " " << sg1-2000 << " "  << sg2-2000 << " " << sg3-2000 << endl;
		//cout << sg1-2000 << " "  << sg2-2000 << " " << sg3-2000 << endl;
	/** Run the loop in a specified frequency */
		usleep(1*100);
	}	

/** Close file and terminate Logger. */
	myfile.close();	
	printf("[BAD Logger] Shutting down...\n");	
}


void BAD::precisionGrasp()
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        hand->init();
    else
        open(HAND);
    
    
    
    
/** Move to initial position */
	hand->setProperty(FINGER1, M, 80000);
	hand->setProperty(FINGER2, M, 80000);
	hand->setProperty(FINGER3, M, 80000);
	hand->setProperty(SPREAD, M, 0);

/** Read strain gauge without any force */
	int sg[3];
	for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->getProperty(puck, SG, &sg[puck-FINGER1]);
    usleep(300*1000);
        
/** Start moving to grab the phone. If yoyu feel it stop pushing */    
    cout << "[BAD] PrecisionGrasp: Press ENTER when you want start grasping" << endl;
    cin.ignore();
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }
    
    bool flag[3] = {true, true, true};
	int current_sg;
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			//hand->getProperty(puck,SG, &sg[puck-FINGER1]);
			//usleep(1*1000);
			hand->getProperty(puck, SG, &current_sg);
			if (flag[puck-FINGER1] & (current_sg - sg[puck-FINGER1] > 300)) {
				cout << "puck" << puck << "flag false" << endl;
				hand->setProperty(puck, MODE, MODE_IDLE);
				hand->setProperty(puck, TSTOP, 150);
				flag[puck-FINGER1] = false;
			}
			else
				hand->setProperty(puck, MODE, MODE_VELOCITY);
		}

	}
	//hand->setProperty(FINGER1, MODE, MODE_IDLE);
	//hand->setProperty(FINGER2, MODE, MODE_IDLE);
	hand->setProperty(HAND, MODE, MODE_IDLE);
	hand->setProperty(SPREAD, MODE, MODE_PID);
	
	cout << "[BAD] PrecisionGrasp: Press ENTER to release the object." << endl;
    cin.ignore();
    
    int p;
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
    	hand->getProperty(puck, P, &p);
    	hand->setProperty(puck, M, p-10000);
    }
    
    hand->setProperty(HAND, MODE, MODE_IDLE);
    hand->setProperty(SPREAD, MODE, MODE_IDLE);
}

void BAD::distortionControl(double distortion)
{

	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
    
    int sg[3];
    int myCT = 130000;
    
    cout << msg_type << "I will close the fingers in order to grab the bottle." << endl;
    
/** Read SG absence of force, and set velocity of the fingers */
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->getProperty(puck, SG, &sg[puck-FINGER1]);
    }
	usleep(1*1000*1000);
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 50);

/** Set a large max torque and start moving the fingers*/
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MT, 4000);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }
	
/** 1st loop: run until you encounter the object */	
	bool flag[3] = {true, true, true};
	int current_sg;
	int pos[3] = {0,0,0};
	int current_pos[3] = {0,0,0};
	int	dx[3];
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->getProperty(puck, SG, &current_sg);
			if (current_sg - sg[puck-FINGER1] > 350) {
				hand->setProperty(puck, MODE, MODE_IDLE);
				flag[puck-FINGER1] = false;
			}
		}
	}

/** Read the position of the fingers to know where the object is 
	and calculate dx */
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, MODE, MODE_IDLE);
        hand->getProperty(puck, P, &pos[puck-FINGER1]);
        dx[puck-FINGER1] = distortion*(myCT-pos[puck-FINGER1]);
        cout << dx[puck-FINGER1] << endl;
    }

	for (int i=0; i<3; i++)
		flag[i] = true;
	for (int puck=FINGER1; puck<=FINGER3; puck++)
		hand->setProperty(puck, V, 100);
	
/** 2nd Loop: Squeze the object until we reach initial position + dx */
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->getProperty(puck, P, &current_pos[puck-FINGER1]);
			if (current_pos[puck-FINGER1] < dx[puck-FINGER1] + pos[puck-FINGER1]) {
				hand->setProperty(puck, MODE, MODE_VELOCITY);
	        }
	        else {
	        	hand->setProperty(puck, MODE, MODE_IDLE);
	        	flag[puck-FINGER1] = false;
	        }
		}

	}

    usleep(2*1000*1000);
}


void BAD::staple()
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
    
    
    /** Read SG absence of force, and set velocity of the fingers */
	int sg[3];
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->getProperty(puck, SG, &sg[puck-FINGER1]);

	usleep(500*1000);
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }


/** Start grasping. When every fingers feels the object stop and hold the grasp */
	bool flag[3] = {true, true, true};
	int current_sg;
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->getProperty(puck, SG, &current_sg);
			if (current_sg - sg[puck-FINGER1] > 800) {
				hand->setProperty(puck, MODE, MODE_IDLE);
				flag[puck-FINGER1] = false;
			}
		}

	}

	hand->setProperty(SPREAD, MODE, MODE_IDLE);
	
	int pos[3];
	int k;
    hand->getProperty(FINGER2, P, &pos[1]);
	hand->getProperty(FINGER3, P, &pos[2]);
	
	
	
	cout << "[BAD] Staple: Press ENTER" << endl;
    cin.ignore();

    hand->setProperty(FINGER1, MT, 2000);    
    hand->setProperty(FINGER2, MT, 2000);
    hand->setProperty(FINGER3, MT, 2000);
    
    hand->setProperty(SPREAD, MODE, MODE_PID);
    /*
    for (int puck=FINGER2; puck<=FINGER3; puck++) {
  		hand->setProperty(puck, T, 2000);
   		hand->setProperty(puck, TSTOP, 0);
   	}
   	
    while(true) {
    	gettimeofday(&end, NULL);		
		seconds  = end.tv_sec  - start.tv_sec;
    	
   		for (int puck=FINGER2; puck<=FINGER3; puck++)
        	hand->setProperty(puck, MODE, MODE_TORQUE);
   	 	
   	 	if (seconds > 2)
   	 		break;
   	 }
   	 */

	hand->setProperty(FINGER1, CMD, CMD_TO);
	hand->setProperty(FINGER2, CMD, CMD_TO);
	hand->setProperty(FINGER3, CMD, CMD_TO);
	
	
	
	usleep(5*1000*1000);
	
	for (int puck=FINGER2; puck<=FINGER3; puck++)
    	hand->setProperty(puck, MODE, MODE_IDLE);
	
	
	
	hand->setProperty(FINGER2, M, pos[1]);
	hand->setProperty(FINGER3, M, pos[2]);
	
	
	
	
	usleep(30*1000*1000);
	
	usleep(3*1000000);
	//hand->init2();
	for (int puck=FINGER2; puck<=FINGER3; puck++)
        	hand->setProperty(puck, MODE, MODE_IDLE);
	/*
	int pos[3];
	int k;
    hand->getProperty(FINGER2, P, &k);
	hand->getProperty(FINGER3, P, &pos[2]);
	
	//usleep(3*1000*1000);
		
	cout << k << " " << pos[2] << endl;
	
	int new_pos = pos[1] - 10000;	
	hand->setProperty(FINGER2, M, new_pos);
	new_pos = pos[2] - 10000;
	hand->setProperty(FINGER3, M, new_pos);
	*/

    
    for (int puck=FINGER2; puck<=FINGER3; puck++) {
    	hand->setProperty(puck, V, -30);
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }
    usleep(1*1000);
    for (int puck=FINGER2; puck<=FINGER3; puck++) {
        hand->setProperty(puck, MODE, MODE_IDLE);
    }
	
    
    
}


void BAD::simpleCylinderGrasp()
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
        
        /** Read SG absence of force, and set velocity of the fingers */
	int sg[3];
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->getProperty(puck, SG, &sg[puck-FINGER1]);

	usleep(500*1000);
	
	hand->setProperty(SPREAD, M, 9000);
	
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }


/** Start grasping. When every fingers feels the object stop and hold the grasp */
	bool flag[3] = {true, true, true};
	int current_sg;
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->getProperty(puck, SG, &current_sg);
			if (current_sg - sg[puck-FINGER1] > 800) {
				hand->setProperty(puck, MODE, MODE_IDLE);
				flag[puck-FINGER1] = false;
			}
		}

	}


    
}


void BAD::syringe()
{

	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
        
    
    hand->setProperty(FINGER1, M, 80000);
    hand->setProperty(FINGER2, M, 80000);
    hand->setProperty(FINGER3, M, 100000);
    
    usleep(5*1000*1000);
               
    /*
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
    	hand->setProperty(puck, V, 40);
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }
    */

}

void BAD::simpleGrasp(int HSG_value)
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
        
    bool flag = true;
    char response;
    
    while(flag) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->setProperty(puck, ACCEL, 150);
			hand->setProperty(puck, MV, 50);
			//cout << "mikrainw" << endl;
			hand->setProperty(puck, HSG, HSG_value);
			hand->setProperty(puck, CMD, CMD_OPEN);
		}
		
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			waitDoneMoving(puck);
		}

		for (int puck=FINGER1; puck<=FINGER3; puck++)
			hand->setProperty(puck, M, 200000);
		hand->setProperty(SPREAD, HOLD, 1);
		hand->setProperty(SPREAD, M, 0);
	
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			waitDoneMoving(puck);
		}
	
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			cout << "megalwnw" << endl;
			hand->setProperty(puck, HSG, 20000);
			hand->setProperty(puck, LSG, 0);
			hand->setProperty(puck, MODE, MODE_IDLE);	
		}
	
	
		flag = false;
		int pos[3];
		for (int puck=FINGER1; puck<=FINGER3; puck++)
			hand->getProperty(puck, P, &pos[puck-FINGER1]);

		if (pos[0] > 190000 && pos[1] > 190000 && pos[2] > 190000) {
			cout << "I don't think I grabbed anything, should I try again? (y/n)" << endl;
			cin >> response;
			if (response == 'y') {
				flag = true;
			}
			else if (response == 'N')
				flag = false;
		}	
			

	}
}	


void BAD::holdGrasp(int force)
{
	int pos[3], sg[3];
	while(true) {
		cout << "MPIKA" << endl;
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			hand->setProperty(puck, TSTOP, 0);
			hand->setProperty(puck, MODE, MODE_PID);
			sg[puck-FINGER1] = getSG(puck, false);
			hand->getProperty(puck, P, &pos[puck-FINGER1]);
			
			
			if (sg[puck-FINGER1]<force-50)
				hand->setProperty(puck, P, (pos[puck-FINGER1]+100));
			else if (sg[puck-FINGER1]<force+50)
				hand->setProperty(puck, P, (pos[puck-FINGER1]-100));
		}
		
		cout << "\rSG1: " << getSG(FINGER1, false) << " - SG2: " << getSG(FINGER2, false) << " - SG3: " << getSG(FINGER3, false) << std::flush;
	
		//usleep(1*1000);

	}
}


void BAD::release(int topFinger)
{
        
	int init_position;
	int position;
	int sg[3];
	int current_sg;
	

	hand->getProperty(topFinger, SG, &sg[0]);
	hand->getProperty(topFinger, P, &init_position);

	
	while(true) {
		hand->getProperty(topFinger, SG, &current_sg);
		hand->getProperty(topFinger, P, &position);		
		if (current_sg - sg[0] > 150)
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -20);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}	
		if ( (double)(init_position-position) / (double)(init_position) > 0.2 ) 
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -60);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}

		if ( position < 40000 )
			break;
	}
	

	for (int puck=FINGER1; puck<=FINGER3; puck++)
		hand->setProperty(puck, MODE, MODE_IDLE);
}

void BAD::release2(int topFinger)
{
        
	int init_position;
	int position;
	int sg[3];
	int current_sg[4];
	int samples = 20;
	
	cout << "Ksekianw release2" << endl;
	
	int temp;
	
	hand->getProperty(topFinger, HSG, &temp);
	cout << temp << endl;
	
	hand->getProperty(topFinger, SG, &sg[0]);
	hand->getProperty(topFinger, P, &init_position);

	bool flag = true;
	int sg_diff[samples];
	double var;
	
	while(true) {
		for (int i=0; i<samples; i++) {
			current_sg[0] = getSG(topFinger, false);
			usleep(20*1000);
			current_sg[3] = getSG(topFinger, false);
			sg_diff[i] = current_sg[3] - current_sg[0];
		}
	
		
		//usleep(500*1000);
		hand->getProperty(topFinger, SG, &current_sg[1]);
		//usleep(500*1000);
		hand->getProperty(topFinger, SG, &current_sg[2]);
		
		
		
		var = calculateVariance(sg_diff);
		cout.precision(2);
		
		cout << "\rSG1: " << current_sg[0] << " " << current_sg[3] <<  " " << current_sg[1] <<  " " << current_sg[2] << " " << fixed << var << std::flush;
		
		
			
		
		//if (current_sg[1] - current_sg[0] > 40 && current_sg[1] - current_sg[0] < 100 && current_sg[2] - current_sg[1] > 25 && current_sg[2] - current_sg[1] < 40) {
		if (var > 60) {
			usleep(1*1000*1000);
		//if (flag  && current_sg[1] - current_sg[0] > 40 && current_sg[1] - current_sg[0] < 140){// && current_sg[1] - current_sg[0] < 500 && current_sg[2] - current_sg[1] > 40) {
			cout << "\nKOMPLE " << endl;
			
			
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -20);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
				flag = false;
			}
			break;
			
		}
		else if (flag)
			for (int puck=FINGER1; puck<=FINGER3; puck++)
				hand->setProperty(puck, MODE, MODE_IDLE);
		
		
		//if (current_sg[1] < current_sg[0] + 15 && current_sg[1] > current_sg[0] - 15 && current_sg[2] > current_sg[1] - 15 && current_sg[2] < current_sg[1] + 15) {
		/*
		if (current_sg[1] < current_sg[0] + 15 && current_sg[1] > current_sg[0] - 15 ) {
			sg[0] = current_sg[0];
//			cout << "1 " << endl;
		}
		*/
			
	}
	
	while(true) {
		hand->getProperty(topFinger, P, &position);	
		if ( (double)(init_position-position) / (double)(init_position) > 0.2 ) 
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -60);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}

		if ( position < 40000 )
			break;
	}
	

	for (int puck=FINGER1; puck<=FINGER3; puck++)
		hand->setProperty(puck, MODE, MODE_IDLE);
}	



void BAD::initSG(bool pressing)
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        initHand();
    else
        open(HAND);
    
    usleep(2*1000*1000);
    
    cout << "Please push the fingertips slightly opposite to the palm (like you want to open them more)." << endl;
    cout << "Press ENTER when you are done." << endl;
	cin.ignore();
    
    for (int puck=FINGER1; puck<=FINGER3; puck++)
    	min_sg[puck-FINGER1] = getSG(puck, false);
    
    usleep(2*1000*1000);
    
    if (pressing) {
    	for (int puck=FINGER1; puck<=FINGER3; puck++) {
			cout << "Please start to press FINGER" << puck-FINGER1+1 <<" until your hear a sound." << endl;
			cout << "Press ENTER when you HAVE started pressing to read the SG value and keep pressing for 2secs" << endl;
			cin.ignore();
			max_sg[puck-FINGER1] = getSG(puck, false);
			usleep(2*1000*1000);
    	}
    }
    else
    	for (int puck=FINGER1; puck<=FINGER3; puck++)
			max_sg[puck-FINGER1] = 3800;

    
    cout << "Thank you." << endl;
    
    sg_inited = true;
    
    /*
    cout << "MIN: " << min_sg[0] << " " << min_sg[1] << " " << min_sg[2] << endl;\
    cout << "MAX: " << max_sg[0] << " " << max_sg[1] << " " << max_sg[2] << endl;

    double perc[3];    
    cout.precision(2);    
    while(true) {
    	usleep(1*1000);    	
    	for (int puck=FINGER1; puck<=FINGER3; puck++)
			perc[puck-FINGER1] = getSG(puck, true);
    	
    	cout << "\rSG1: " << fixed << perc[0] << "\% - SG2: " << fixed << perc[1] << "\% - SG3: " << fixed << perc[2] << "\%" << std::flush;
    }
    */
    
    
}
/** getSG():
*	Returns an SG value filtering electronics noise of meg numbers.
*	with perc true in returns a percentage considering min and max sg
* 	with perc=false it retuns just clean sg
*/
double BAD::getSG(int finger, bool perc)
{
	int sg;
	int counter = 0;
	hand->getProperty(finger, SG, &sg);
	while (true) {
		if (sg > 50000 || sg < 100) {
			usleep(1*1000);
			hand->getProperty(finger, SG, &sg);
			counter++;
		}
		else break;
		
		if (counter > 300) {
			cout << "[BAD] getSG: ERROR: Cannot read a logical value for sg" << endl;
			return 0;
		}
	}
	
	if (perc && sg_inited)
		return ((double)(sg-min_sg[finger-FINGER1])/(double)(max_sg[finger-FINGER1]-min_sg[finger-FINGER1]))*100;
	else if (perc && !sg_inited) {
		cout << "[BAD] getSG: ERROR: You have not init the SG so you cannot use perc=true. I will return as perc=false." << endl;
		return sg;
	}
	else
		return sg;

}


void BAD::fetchAndRelease2(int topFinger)
{	

	int rc;
/** wait 1 second and check if hand is initialized */
	usleep(1*1000000);
    if (!hand->initiliazed)
        hand->init();
    else
        open(HAND);
    

    
    cout << msg_type << "I will close the fingers in order to grab the bottle." << endl;
    
/** Set velocity of the fingers */
	usleep(500*1000);
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }


/** Start grasping. When every fingers feels the object stop and hold the grasp */
	bool flag[3] = {true, true, true};
	double current_sg;
	
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			current_sg = getSG(puck, true);
			if (current_sg > 25) {
				hand->setProperty(puck, MODE, MODE_IDLE);
				flag[puck-FINGER1] = false;
			}
		}

	}

	hand->setProperty(SPREAD, MODE, MODE_IDLE);

	
	cout << msg_type << "I grabbed the bottle. You can move the KUKA arm towards a person and the the person can receive the bottle" << endl;
	
/** Check if we have a force in the top finger and release the grasp */
	int init_position;
	int position;
	hand->getProperty(topFinger, P, &init_position);
	while(true) {
		current_sg = getSG(topFinger, true);
		hand->getProperty(topFinger, P, &position);		
		if (current_sg > 40)
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -20);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}	
		if ( (double)(init_position-position) / (double)(init_position) > 0.2 ) 
			for (int puck=FINGER1; puck<=FINGER3; puck++) {
				hand->setProperty(puck, V, -60);
				hand->setProperty(puck, TSTOP,0);
				hand->setProperty(puck, MODE, MODE_VELOCITY);
			}

		if ( position < 40000 )
			break;
	}

	for (int puck=FINGER1; puck<=FINGER3; puck++)
		hand->setProperty(puck, MODE, MODE_IDLE);
		
	cout << msg_type << "Always a pleasure!" << endl;
}

void BAD::precisionGrasp2()
{
	usleep(1*1000000);
    if (!hand->initiliazed)
        hand->init();
    else
        open(HAND);
    
    
    
    
/** Move to initial position */
	hand->setProperty(FINGER1, M, 80000);
	hand->setProperty(FINGER2, M, 80000);
	hand->setProperty(FINGER3, M, 80000);
	hand->setProperty(SPREAD, M, 0);
        
/** Start moving to grab the phone. If yoyu feel it stop pushing */    
    cout << "[BAD] PrecisionGrasp: Press ENTER when you want start grasping" << endl;
    cin.ignore();
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 40);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        hand->setProperty(puck, TSTOP, 0);
        hand->setProperty(puck, MODE, MODE_VELOCITY);
    }
    
    bool flag[3] = {true, true, true};
	double current_sg;
	
	
	cout << "mpainw se loop gia na piasw" << endl;
	for (int puck=FINGER1; puck<=FINGER3; puck++)
        hand->setProperty(puck, V, 30);
	while(flag[0] | flag[1] | flag[2]) {
		for (int puck=FINGER1; puck<=FINGER3; puck++) {
			//hand->getProperty(puck,SG, &sg[puck-FINGER1]);
			//usleep(1*1000);
			current_sg = getSG(puck, true);
			if (flag[puck-FINGER1] & current_sg  > 40) {
				cout << "puck" << puck << "flag false" << endl;
				hand->setProperty(puck, MODE, MODE_IDLE);
				hand->setProperty(puck, TSTOP, 150);
				flag[puck-FINGER1] = false;
			}
			else
				hand->setProperty(puck, MODE, MODE_VELOCITY);
		}

	}
	cout << "to piasa" << endl;
	//hand->setProperty(FINGER1, MODE, MODE_IDLE);
	//hand->setProperty(FINGER2, MODE, MODE_IDLE);
	hand->setProperty(HAND, MODE, MODE_IDLE);
	hand->setProperty(SPREAD, MODE, MODE_PID);
	
	double init_sg[3];
	double curr_sg[3];
	
	usleep(1*1000*1000);
	
	for (int puck=FINGER1; puck<=FINGER3; puck++)
			init_sg[puck-FINGER1] = getSG(puck, true);
	
	    cout << "\rINIT SG1: " << fixed << init_sg[0] << "\% - SG2: " << fixed << init_sg[1] << "\% - SG3: " << fixed << init_sg[2] << "\%" << endl;
	    	
	usleep(1*1000*1000);
	
	bool forceFlag[2] = {false, true};
	
	
	/*
	cout << "perimenw talantwsh" << endl;
	while(true) {
		for (int puck=FINGER1; puck<=FINGER3; puck++)
			init_sg[puck-FINGER1] = getSG(puck, true);
		//usleep(1000);
		for (int puck=FINGER1; puck<=FINGER3; puck++)
			curr_sg[puck-FINGER1] = getSG(puck, true);
		
		
		//cout << "\rcurrSG[0]/initSG[0]: " << curr_sg[0]/init_sg[0]  << std::flush;
		
		
		//if (curr_sg[0] - init_sg[0] > 10 && curr_sg[2] - init_sg[2] > 10 && curr_sg[1] - init_sg[1] < -10)
			//forceFlag[0] = true;
		//if (curr_sg[1] > 1.1*init_sg[1] && curr_sg[2] > 1.1*init_sg[2] && curr_sg[0] < 0.1*init_sg[0])
			//forceFlag[1] = true;
		if (curr_sg[1] - init_sg[1] > 70)
			forceFlag[0] = true;
			
		if (forceFlag[0] && forceFlag[1])
			break;
	}
	cout << "phra talantwsi. eleutherwnw" << endl;
	*/
	
	
	double perc[3];    
    cout.precision(2);
        
    while(true) {
    	usleep(1*1000);    	
    	for (int puck=FINGER1; puck<=FINGER3; puck++)
			perc[puck-FINGER1] = getSG(puck, true);
    	
    	
    	cout << "\rSG1: " << fixed << perc[0] << " - SG2: " << fixed << perc[1] << " - SG3: " << fixed << perc[2] << "   " << std::flush;
	}

	
	/*
	cout << "[BAD] PrecisionGrasp: Press ENTER to release the object." << endl;
    cin.ignore();
    */
    
    int p;
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
    	hand->getProperty(puck, P, &p);
    	hand->setProperty(puck, M, p-50000);
    }
    
    hand->setProperty(HAND, MODE, MODE_IDLE);
    hand->setProperty(SPREAD, MODE, MODE_IDLE);
    
}

//set hlsg. with -1 you dont set anything
void BAD::setHLSG(int finger, double HSGperc, double LSGperc)
{	
	
	int _HSG, _LSG;
	
	_HSG = (max_sg[finger-FINGER1]-min_sg[finger-FINGER1])*(HSGperc/100) + min_sg[finger-FINGER1];
	_LSG = (max_sg[finger-FINGER1]-min_sg[finger-FINGER1])*(LSGperc/100) + min_sg[finger-FINGER1];
	
	if (HSGperc != -1)
		hand->setProperty(finger, HSG, _HSG);
	if (LSGperc != -1)
		hand->setProperty(finger, HSG, _LSG);
}



double BAD::calculateVariance(int matrix[])
{
	double average = 0; 
	int size = sizeof(matrix);
	double dev[size], variance;
	
	for (int i=0; i<size; i++) {
			average += (double)matrix[i];
		}
	
	average = average/size;
		
	for (int i=0; i<size; i++)
		dev[i] = ((double)matrix[i] - average)*((double)matrix[i] - average);

		
	for (int i=0; i<size; i++)
		dev[i] = ((double)matrix[i] - average)*((double)matrix[i] - average);
		
	for (int i=0; i<size; i++)
		variance += dev[i];
	
	variance = variance/size;
	
	variance = sqrt(variance);
	
	return variance;
	
}





