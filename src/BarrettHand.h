#ifndef __BARRETTHAND_HH_INCLUDED__
#define __BARRETTHAND_HH_INCLUDED__

#include "CANbus.h"
#include "barrett_properties.h"


using namespace std;

class BHand {
    CANbus *bus;
    string type;
       
    pthread_mutex_t  *mutex;
    
    string msg_type;
	string err_type;
public:
    bool initiliazed;
	BHand(CANbus*, string, pthread_mutex_t*);
	~BHand();
	
	int init();
	int init2();
	int reset();
	int setProperty(int, int, int);
	int getProperty(int, int, int*);
	int BarrettCANProtocolEncode(CANMessage*, int8_t, int8_t, int32_t, bool, bool);
	int BarrettCANProtocolDecode(CANMessage, int*, int);
	int getPosition(int, int*);
	int status(int id);
	int initPropValues();
	int close();
	int getDualPackedPosition(int node, int* pos, int* jpos);
};

#endif 
