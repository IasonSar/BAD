#include "BarrettHandSim.h"


BHandSim::BHandSim(CANbus* myBus, string myType)
{
    bus = myBus;
    type = myType;
    initiliazed = false;
    
    msg_type = "[Hand] ";
    err_type = "[Error] BHand::";
    
    if (type.compare("BH8-262") & type.compare("BH8-280") & type.compare("BH8-282"))
        printf("[BHand] Warning: The type of BarrettHand does not seem to be one of the following:\n"
               "        1. BH8-262\n"
               "        2. BH8-280\n"
               "        3. BH8-282\n");
    
}
BHandSim::~BHand(){};

int BHandSim::setProperty(int node, int property, int value)
{   
	int to = node;
	CANMessage msg;
	
	if (node==15) {
        //cerr << err_type << fun << "Trexw thn write gia node=15\n";
        
        // Run the function to create a raw CAN message based on Barrett protocol
        if (BarrettCANProtocolEncode(&msg, 0x05, property, value, true, true) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
                
		if (bus->write(msg) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "Bus.write does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else if (node>=11 && node<=14) {
        //cerr << err_type << fun << "Trexw thn write gia node=11-14\n";
        if (BarrettCANProtocolEncode(&msg, node, property, value, false, true) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
        
		if (bus->write(msg) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "Bus.write does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else {
		printf("[BHand] Error: Node parameter in setProperty has to be\n"
			   "        between 11 and 15. 11-13 for the fingers, 14 for \n"
			   "        the spread and 15 for the group of the whole hand\n");
		return 	MD_ERROR_FAIL;
	}
	
	usleep(100); //sleep 100 us
		
	return MD_ERROR_OK;	
}

//TODO: how to get property from a group, from all pucks
int BHandSim::getProperty(int node, int property, int* value)
{
	int to = node;
	CANMessage msg;
	//int v[2];
	
	
	if (node==15)
		return MD_ERROR_FAIL; //TODO
	else if (node>10) { 
		if (BarrettCANProtocolEncode(&msg, node, property, 0, false, false) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	
		if (bus->write(msg) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "bus.write does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else {
		printf("[BHand] Error: Node parameter in setProperty has to be\n"
			   "        between 11 and 15. 11-13 for the fingers, 14 for \n"
			   "        the spread and 15 for the group of the whole hand.\n");
		return 	MD_ERROR_FAIL;
	}
	
	usleep(500);  // wait 0.5 ms
	
	if (bus->read(&msg) != MD_ERROR_OK) {
			printf("[Error] in getProperty. Function read() does not returned\n"	
			       "        successfully.\n");
			return MD_ERROR_FAIL;
	}
	
	if (property == P | property == JP) {
		if (BarrettCANProtocolDecode(msg, value, 1) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else {		
		if (BarrettCANProtocolDecode(msg, value, 0) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
		
		//*value = v[0];
	}
	return MD_ERROR_OK;	
}

int BHandSim::init() {
    
    bus->init();
   
    //make sure the hand is connected and you see the pucks   
    int r[4] = {-1, -1, -1, -1};
   
    for (int puck=FINGER1; puck <= SPREAD; puck++) {
        getProperty(puck, ROLE, &r[puck-FINGER1]);
    }
    
    if (r[0]<0 || r[1]<0 || r[2]<0 || r[3]<0) {
        cerr << err_type << __FUNCTION__ << ": Some pucks have not been found. You either need to\n" \
             << "connect and turn on the BarrettHand, or to contact the Barrett support." << endl;
        return MD_ERROR_FAIL;
    }
   
   // The first time the hand is initialized ask for pressing the ENTER
    if (!initiliazed) {
        printf("[BHand] Initializing the hand...\n");
        //bus->reset();
        
        cout << msg_type << "The hand is about to initiliaze." << endl;
        cout << "\033[1;31mPlease be sure the hand is in safe position.\033[0m\n";
        cout << "\033[1;31mPress ENTER when you are ready...\033[0m\n";
        cin.ignore();
    }
    
    int puck;
    for (puck=FINGER1; puck<=SPREAD; puck++) {
        //cerr << err_type << __FUNCTION__ <<  "puck = " << puck << endl;
        setProperty(puck, STAT, STATUS_READY);
        usleep(750*1000); 
        setProperty(puck, CMD, CMD_HI);    
        usleep(3*1000000);
    }
    
    usleep(1*1000000);
    
    setProperty(SPREAD, MODE, MODE_IDLE);   
    
    initiliazed = true;
    return MD_ERROR_OK;
}


// Initialize the values of the most properties to their defaults
int BHandSim::initPropValues()
{
    usleep(100*1000);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        setProperty(puck, TSTOP, 50);
    setProperty(SPREAD, TSTOP, 150);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        setProperty(puck, MT, 2000);
        
    for (int puck=FINGER1; puck<=FINGER3; puck++)
        setProperty(puck, CT, 195000);    
    
    usleep(1*1000000);
    
    return MD_ERROR_OK;
}

int BHandSim::close() 
{
    cout << msg_type << "Closing the whole hand. Please remove obstacles." << endl;
    
    usleep(1*1000000); 
    
    if(init() != MD_ERROR_OK) {
        cerr << err_type << __FUNCTION__ << ": init() failed to return" << endl;
        return MD_ERROR_ELSE;
    }
    
    for (int puck=SPREAD; puck>=FINGER1; puck--) {
        setProperty(puck, CMD, CMD_CLOSE);
        usleep(3*1000000);
    }
    
    cout << msg_type << "Done closing fingers and spread." << endl; //TODO check if actually closed with position checking
    
    setProperty(SPREAD, MODE, MODE_IDLE);
    
    return MD_ERROR_OK;    
}
