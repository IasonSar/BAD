/**
	BarrettHand.cpp

	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 28/02/2015
	
	BarrettHand.cpp is part of BAD (Barretthand AUTh Driver). BAD is a 
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

#include "BarrettHand.h"

BHand::BHand(CANbus* myBus, string myType, pthread_mutex_t* myMutex)
{
    bus = myBus;
    type = myType;
    mutex = myMutex;
    initiliazed = false;
    
    msg_type = "[Hand] ";
    err_type = "[Error] BHand::";
    
    if (type.compare("BH8-262") & type.compare("BH8-280") & type.compare("BH8-282"))
        printf("[BHand] Warning: The type of BarrettHand does not seem to be one of the following:\n"
               "        1. BH8-262\n"
               "        2. BH8-280\n"
               "        3. BH8-282\n");
    
}
BHand::~BHand(){};

int BHand::setProperty(int node, int property, int value)
{   
	int rc;
	rc = pthread_mutex_lock(mutex);
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
	
	rc = pthread_mutex_unlock(mutex);
		
	return MD_ERROR_OK;	
	
}

//TODO: how to get property from a group, from all pucks
int BHand::getProperty(int node, int property, int* value)
{
	int rc;
	int to = node;
	CANMessage msg;
	//int v[2];
	
	rc = pthread_mutex_lock(mutex);
	
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
	
	rc = pthread_mutex_unlock(mutex);

	return MD_ERROR_OK;	
}


int BHand::BarrettCANProtocolEncode(CANMessage* msg, int8_t to, int8_t property, int32_t value, bool group, bool set)
{
	//CANMessage msg;
	
	//TPCANmsg Message;
	bool is32bit = false;
	
	//if group then 100 0000 0000 OR 0 (from:pc) OR to (to:to)
	if (group) {
	    //printf("[DEBUG] CANbus::write: exw group...\n");
		msg->MSGID = 0x400 | 0x00 | to;
	}
	else {
	    //printf("[DEBUG] CANbus::write: den exw group, id = to...\n");
		msg->MSGID = to;
	}
	// If it is a set property command then first bit of data[0] is 1
	// The rest of them are just the 7-bit property. If it is a get property
	// we need only one byte for data, not 4.
	
	
	// prop32 is an array containing the 32-bit properties
	int8_t prop32[12] = {48, 50, 52, 54, 56, 58, 66, 68, 74, 88, 96, 98};
	
	// Check if property exists in prop32 (if it is a 32-bit property)
	int8_t *temp = std::find(prop32, prop32+12, property);
	if (temp != prop32+12){
	    //cerr << err_type << __FUNCTION__ << "property is 32-bit with index: " \
	         << (temp - prop32) << endl;
	    is32bit = true;
	}
	
	if (is32bit)
	    msg->DLC = 6; // len = 6 meaning data = 4 bytes, meaning 32-bit properties
	else
	    msg->DLC = 4; // len = 4 meaning data = 2 bytes, meaning 16-bit properties
	
	// make sure 16-bit prop is 16-bit by filtering with 0xFFFF
	if (msg->DLC == 4) {
	    value = value & 0xFFFF;
	}
	
	// Be sure property is 7-bit (the first is zero)
	if (property > 127) {
		cout << "Error. Property is over 7-bit" << endl;
	    return MD_ERROR_FAIL;
	}
	 
	
	if (set) { // if set property
	    msg->DATA[0] = 0x80 | property;
        msg->DATA[1] = 0;
	    if (!is32bit) { //if is 16-bit
		    msg->DATA[2] = value & 0x00FF;
		    uint16_t temp = value & 0xFF00;
		    temp = temp >> 8;
		   msg->DATA[3] = temp;
		}
		else { //if is 32-bit
		    int32_t  temp;
		    msg->DLC = 6;
		    temp =  value & 0x000000FF;
            msg->DATA[2] = temp;
            
            temp = value & 0x0000FF00;
            temp = temp >> 8;
            msg->DATA[3] = temp;
            
            temp = value & 0x00FF0000;
            temp = temp >> 16;
            msg->DATA[4] = temp;

            
            temp = value & 0xFF000000;
            temp = temp >> 24;
            msg->DATA[5] = temp;        
		}
	}
	else { //if get property
	    msg->DLC = 2;  
		msg->DATA[0] = property;
		msg->DATA[1] = 0;
	}
	
	return MD_ERROR_OK;
	
	//usleep(100); // Sleep for 100 us

}


// int packed = 0: no packed, =1:packed P or JP, =2:dual packed P+JP
int BHand::BarrettCANProtocolDecode(CANMessage msg, int* value, int packed)
{
	
	if (packed==0) {
		bool is32bit;
		if (msg.DLC == 4) is32bit = false;
		else is32bit = true;
	
		if (!is32bit) { //if 16-bit
			uint16_t temp;
			temp = msg.DATA[3];
			temp = temp << 8;
			*value = msg.DATA[2] | temp;	    
		} 
		else { //if 32-bit
			uint32_t temp[3];
			temp[3] = msg.DATA[5];
			temp[3] = temp[3] << 24;
			
			temp[2] = msg.DATA[4];
			temp[2] = temp[2] << 16;
			
			temp[1] = msg.DATA[3];
			temp[1] = temp[1] << 8;
			
			temp[0] = msg.DATA[2];
			
			*value = temp[0] | temp[1] | temp[2] | temp[3];    
		}
	}
	else if (packed==1) {
		uint32_t temp;
		temp = ((msg.DATA[0] & 0x7F) << 16) | (msg.DATA[1] << 8) | msg.DATA[2];
		*value = temp;	
	}
	else if (packed==2) {
		uint32_t temp;
		//TODO pass both arguments P & JP, temp[2] sth like that
		//temp = ((msg.DATA[0] & 0x7F) << 16) | (msg.DATA[1] << 8) | msg.DATA[2];
		temp = ((msg.DATA[3] & 0x7F) << 16) | (msg.DATA[4] << 8) | msg.DATA[5];
		*value = temp;
	}
	
	//value[1] = 0;
	return MD_ERROR_OK;
}


int BHand::getDualPackedPosition(int node, int* pos, int* jpos)
{	
	
	//TODO check somewhere if the hand/puck/finger has secondary encoder, only then
	// with the getProperty(P) it would work. Just be sure.
	
	int to = node;
	CANMessage msg;
	
	if (node==15)
		return MD_ERROR_FAIL; //TODO
	else if (node>10) {
		if (BarrettCANProtocolEncode(&msg, node, P, 0, false, false) != MD_ERROR_OK) {
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
	
	
	if (BarrettCANProtocolDecode(msg, pos, 1) != MD_ERROR_OK) {
		cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
		return MD_ERROR_FAIL;
		}
	
	if (BarrettCANProtocolDecode(msg, jpos, 2) != MD_ERROR_OK) {
		cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
		return MD_ERROR_FAIL;
	}	
	
	return MD_ERROR_OK;
} 






















/*
int BHand::getPosition(int node, int *value1, *value2)
{
	
}
*/


int BHand::init() {
    
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

int BHand::init2()
{
	bus->init();
	
	int puck;
    for (puck=FINGER1; puck<=SPREAD; puck++)
        setProperty(puck, STAT, STATUS_READY);
}


// Initialize the values of the most properties to their defaults
int BHand::initPropValues()
{
    usleep(100*1000);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        setProperty(puck, TSTOP, 50);
        setProperty(puck, MT, 2700);
        setProperty(puck, CT, 195000);
        setProperty(puck, OT, 0);
        setProperty(puck, MV, 400);
        setProperty(puck, ACCEL, 200);
        setProperty(puck, HSG, 20000);
        setProperty(puck, LSG, 0);
        setProperty(puck, KD, 2500);
        setProperty(puck, KI, 0);
        setProperty(puck, KP, 200);
	}
	
    setProperty(SPREAD, TSTOP, 150); 
    
    usleep(1*1000000);
    
    return MD_ERROR_OK;
}

int BHand::close() 
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
