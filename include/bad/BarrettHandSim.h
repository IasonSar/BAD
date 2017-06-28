#include <bad/CANbus.h>
#include <bad/barrett_properties.h>

using namespace std;

class BHandSim {
    CANbus *bus;
    string type;
    
    string msg_type;
	string err_type;
	
	int puckEncoder[4];
	int strain[3];
	
public:
    bool initiliazed;
	BHand(CANbus*, string);
	~BHand();
	
	int init();
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
