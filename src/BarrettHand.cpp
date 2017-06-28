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

#include <bad/BarrettHand.h>

BHand::BHand(CANbus* myBus, string myType, pthread_mutex_t* myMutex)
{
/** Initialize the basic member variables. */
    bus = myBus;
    type = myType;
    mutex = myMutex;
    initiliazed = false;
    msg_type = "[Hand] ";
    err_type = "[Error] BHand::";

/** Check if the BHand type is a legitimate one. */
    if (type.compare("BH8-262") & type.compare("BH8-280") & type.compare("BH8-282"))
        printf("[BHand] Warning: The type of BarrettHand does not seem to be one of the following:\n"
               "        1. BH8-262\n"
               "        2. BH8-280\n"
               "        3. BH8-282\n");
    
}

BHand::~BHand(){};

int BHand::init() {
    
/** Initialize the CAN bus. */
    bus->init();
   
/** Make sure the hand is connected and you can see all the pucks. */
    int r[4] = {-1, -1, -1, -1};
       
    for (int puck=FINGER1; puck <= SPREAD; puck++) {
        getProperty(puck, ROLE, &r[puck-FINGER1]);
    }
    
    if (r[0]<0 || r[1]<0 || r[2]<0 || r[3]<0) {
        cerr << err_type << __FUNCTION__ << ": Some pucks have not been found. You either need to\n" \
             << "connect and turn on the BarrettHand, or to contact the Barrett support." << endl;
        return MD_ERROR_FAIL;
    }
   
/** The first time the hand is initialized, ask for pressing the ENTER, as 
	a safety feature. */
    if (!initiliazed) {
        printf("[BHand] Initializing the hand...\n");
        //bus->reset();
        
        cout << msg_type << "The hand is about to initiliaze." << endl;
        cout << "\033[1;31mPlease be sure the hand is in safe position.\033[0m\n";
        cout << "\033[1;31mPress ENTER when you are ready...\033[0m\n";
        cin.ignore();
    }

/** Set for all fingers the property STAT (5) to READY (2) in order to 
    initialize the pucks. Then set the property CMD (29) to HandInitialize (13)
    in order to open the fingers to their initial position. */
    int puck;
    for (puck=FINGER1; puck<=SPREAD; puck++) {
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

int BHand::setProperty(int node, int property, int value)
{
/** Before you do anything lock the mutex, to be sure that you are the only
    thread accessing the bus, to keep data communication consistant.  */
	int rc;
	rc = pthread_mutex_lock(mutex);
	int to = node;
	CANMessage msg;
	
/** Distinct between a group and a non group message. */
	if (node==15) {

	/** Create a raw CAN message with message id = 5 (group for all pucks)
	    group=true and include the essential info (property, new value). */
        	if (BarrettCANProtocolEncode(&msg, 0x05, property, value, true, true) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	/** Use the CANBuss::write() to write the message on the bus. */
		if (bus->write(msg) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "Bus.write does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else if (node>=11 && node<=14) {
	 
	/** Create a raw CAN message with message id the given and group = 
	    false and include the essential info (property, new value). */
		if (BarrettCANProtocolEncode(&msg, node, property, value, false, true) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
        
	/** Use the CANBuss::write() to write the message on the bus. */
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

	/** After you are done with accessing the bus free the mutex lock. */
	rc = pthread_mutex_unlock(mutex);
		
	return MD_ERROR_OK;	
	
}


int BHand::getProperty(int node, int property, int* value)
{
	int rc;
	int to = node;
	CANMessage msg;

/** Before you do anything lock the mutex, to be sure that you are the only
    	    thread accessing the bus, to keep data communication consistant.  */
	rc = pthread_mutex_lock(mutex);
	
/** Distinct between a group and a non group message. First you need to 
    send a message with no data to ask for the property and then you
    wait for the answer. */
	if (node==15)
		return MD_ERROR_FAIL; //TODO
	else if (node>10) {
 
	/** Create a raw CAN message with message id the given and group = 
	    false and include the essential info (property, new value) and
	    no data. */
		if (BarrettCANProtocolEncode(&msg, node, property, 0, false, false) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	
	/** Use the CANBuss::write() to write the message on the bus. */
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
	
/** Wait 500 μs for the reply. */
	usleep(500);  // wait 0.5 ms
	
/** Read the CANbus for incoming messages. */
	if (bus->read(&msg) != MD_ERROR_OK) {
			printf("[Error] in getProperty. Function read() does not returned\n"	
			       "        successfully.\n");
			return MD_ERROR_FAIL;
	}
	
/** Distinct between packed and no packed incoming messages. */
	if (property == P | property == JP) {
	/** Decode the raw CAN message and return the value of the property
	    within the CAN message, for a packed message of 22-bit value. */	
		if (BarrettCANProtocolDecode(msg, value, 1) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	}
	else {
		
	/** Decode the raw CAN message and return the value of the property
	    within the CAN message, for a regular message (16-bit or 32-bit). */ 
		if (BarrettCANProtocolDecode(msg, value, 0) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
		

	}
	
	/** After you are done with accessing the bus free the mutex lock. */
	rc = pthread_mutex_unlock(mutex);

	return MD_ERROR_OK;	
}


int BHand::BarrettCANProtocolEncode(CANMessage* msg, int8_t to, int8_t property, int32_t value, bool group, bool set)
{
	bool is32bit = false;

/** Distinct between group and non-group. If group, you need to put logical 1
    as the first bit by doing: 100 0000 0000 OR o (from: pc) to (receiver: 
    arg to). If non-group the message id is equal to arg "to".  */	
	if (group)
		msg->MSGID = 0x400 | 0x00 | to;
	else
		msg->MSGID = to;
	
	
/**  prop32 is an array containing the 32-bit properties */
	int8_t prop32[12] = {48, 50, 52, 54, 56, 58, 66, 68, 74, 88, 96, 98};
	
/**  Check if the property exists in prop32 (if it is a 32-bit property). */
	int8_t *temp = std::find(prop32, prop32+12, property);
	if (temp != prop32+12)
	    is32bit = true;
	
/** If we have a 32-bit property set DLC=6 bytes, meaning 1 control byte, 1
    zero byte and 4 bytes of data. If is a 16-bit property set DLC=4, ie
    2 bytes of data. */
	if (is32bit)
	    msg->DLC = 6;
	else
	    msg->DLC = 4; 
	
/** Make sure 16-bit prop is 16-bit by filtering with 0xFFFF and 
    property is 7-bit (the first bit is zero). */
	if (msg->DLC == 4) {
	    value = value & 0xFFFF;
	}
	
	if (property > 127) {
		cout << "Error. Property is over 7-bit" << endl;
	    return MD_ERROR_FAIL;
	} 
	

/** Distinct between set and get property. If it is a set property the first
	bit of data[0] is 1 and the rest are just the 7-bit property. If
	it is a get property command we do not need bytes for data.  */
	if (set) {
	
	/** Set the first bit to 1 */
		msg->DATA[0] = 0x80 | property;
		msg->DATA[1] = 0;
	
	/** Distinct between 16-bit and 32-bit property. */	
		if (!is32bit) {
		/** For a 16-bit prop, filter the value and put the LSB
			first and the MSB second. */
			msg->DATA[2] = value & 0x00FF;
			uint16_t temp = value & 0xFF00;
			temp = temp >> 8;
			msg->DATA[3] = temp;
		}
		else { 
		/** For 32-bit prop, filter the value and put the LSB
			first, the middle LSB second, the middle MSB
			third and the MSB fourth. */
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
	else {
		msg->DLC = 2;  
		msg->DATA[0] = property;
		msg->DATA[1] = 0;
	}
	
	return MD_ERROR_OK;
}


// int packed = 0: no packed, =1:packed P or JP, =2:dual packed P+JP
int BHand::BarrettCANProtocolDecode(CANMessage msg, int* value, int packed)
{
	/** Distinct between non-packed, packed kai dual-packed. The function
		returns only one value so if packed=1 returns the first 22-bit
		value (P) and if packed=2 returns the second 22-bit value (JP).
		To read both values you should dualPackedProperty() (see
		below). */
	if (packed==0) {
	/** Decode the message applying the reverse procedure of 
		BarrettCANProtocolEncode() for a simple message. */
		bool is32bit;
		if (msg.DLC == 4) is32bit = false;
		else is32bit = true;
	
		if (!is32bit) {
			uint16_t temp;
			temp = msg.DATA[3];
			temp = temp << 8;
			*value = msg.DATA[2] | temp;	    
		} 
		else {
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
	/** Decode the 22-bit message. (3 bytes with 1 as first bit). */
		uint32_t temp;
		temp = ((msg.DATA[0] & 0x7F) << 16) | (msg.DATA[1] << 8) | msg.DATA[2];
		*value = temp;	
	}
	else if (packed==2) {
	/** Decode the 22-bit message. (3 bytes with 1 as first bit). */
		uint32_t temp;
		temp = ((msg.DATA[3] & 0x7F) << 16) | (msg.DATA[4] << 8) | msg.DATA[5];
		*value = temp;
	}	

	return MD_ERROR_OK;
}


int BHand::getDualPackedPosition(int node, int* pos, int* jpos)
{	
	int to = node;
	CANMessage msg;
	
/** Distinct between a group and a non-group message. */
	if (node==15)
		return MD_ERROR_FAIL; //TODO
	else if (node>10) {
	/** Encode the information for th P property. */
		if (BarrettCANProtocolEncode(&msg, node, P, 0, false, false) != MD_ERROR_OK) {
			cerr << err_type << __FUNCTION__ << "BarrettCANProtocolEncode does not return properly.\n";
			return MD_ERROR_FAIL;
		}
	/** Write the message to CAN bus to ask for the P property. */
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
	
/** Wait 500 ms or the reply. */
	usleep(500); 

/** Read the message from the CAN bus. */
	if (bus->read(&msg) != MD_ERROR_OK) {
			printf("[Error] in getProperty. Function read() does not returned\n"	
			       "        successfully.\n");
			return MD_ERROR_FAIL;
	}
/** Decode it for packed=1 (it will return the value of P. */	
	if (BarrettCANProtocolDecode(msg, pos, 1) != MD_ERROR_OK) {
		cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
		return MD_ERROR_FAIL;
		}
/** Decode for packed=2 (it will return the value of JP) */
	if (BarrettCANProtocolDecode(msg, jpos, 2) != MD_ERROR_OK) {
		cerr << err_type << __FUNCTION__ << "BarrettCANProtocolDecode does not return properly.\n";
		return MD_ERROR_FAIL;
	}	
	
	return MD_ERROR_OK;
}


int BHand::initPropValues()
{
    usleep(100*1000);
    
    for (int puck=FINGER1; puck<=FINGER3; puck++) {
        setProperty(puck, TSTOP, 50);
        setProperty(puck, MT, 22200);
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


	
int BHand::init2()
{
	bus->init();
	
	int puck;
    for (puck=FINGER1; puck<=SPREAD; puck++)
        setProperty(puck, STAT, STATUS_READY);
}
