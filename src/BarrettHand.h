/**
	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 28/02/2015
	
	This file is part of BAD (Barretthand AUTh Driver). BAD is a 
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

#ifndef __BARRETTHAND_HH_INCLUDED__
#define __BARRETTHAND_HH_INCLUDED__

#include "CANbus.h"
#include "barrett_properties.h"

using namespace std;

/**
* A BarrettHand class. It implements the basic functionality of BarrettHand. It 
* uses the proprietary BarrettHand protocol for CAN messages to translate
* high level commands to raw CAN messages.
*/
class BHand {
    CANbus *bus; /**< Pointer object to the CAN bus. */
    string type; /**< Type of of hand. Tested with BH8-282. */
       
    pthread_mutex_t  *mutex; /**< A mutex that is being used by the BAD logger thread. */
    
    string msg_type; /**< Type of printed messages. */
	string err_type; /**< Type of printed errors. */
public:
    bool initiliazed; /**< Flag that shows if the hand has been initialized already. */
    
    /**
	* The contructor of BarrettHand that initialize the member variables.
	*/
	BHand(CANbus*, string, pthread_mutex_t*);
	
	/**
	* The destructor of BarrettHand.
	*/
	~BHand();
	
	/**
	* The function for initialize the BHand. It has to be called before any 
	* other call to BHand and after the CANBus::init().
	* @return An error code. Returning 0 means we have no error.
	* @see CANBus::init()
	*/
	int init();
	
	/**
	* The function for sending a SET PROPERTY command to BarrettHand and changes
	* the value of a puck's property.
	* @param node An int that points to the puck's ID. 11-14 is for FINGER1 to 
	* FINGER3 and SPREAD and 15 is for the group of all the pucks of the hand.
	* @param property An int that contains the property ID, given by the 
	* Barrett's documantation tables.
	* @param value The new value of the property.
	* @return An error code. Returning 0 means we have no error.
	* @see BarrettCANProtocolEncode()
	*/
	int setProperty(int node, int property, int value);
	
	/**
	* The function for sending a GET PROPERTY command to BarrettHand and reads
	* the value of a puck's property.
	* @param node An int that points to the puck's ID. 11-14 is for FINGER1 to 
	* FINGER3 and SPREAD and 15 is for the group of all the pucks of the hand.
	* @param property An int that contains the property ID, given by the 
	* Barrett's documantation tables.
	* @param value Pointer that will contain the current value of the property.
	* @return An error code. Returning 0 means we have no error.
	* @see BarrettCANProtocolEncode()
	* @see BarrettCANProtocolDecode()
	*/
	int getProperty(int node, int property, int* value);
	
	/**
	* The function encoding a the information for a command towards the hand to
	* a raw CAN message (CANmessage object). This function is being used by 
	* setProperty and getProperty and it implements the Barrett CAN Protocol.
	* @param msg A pointer to a CAN Message object that will contain the final
	* raw CAN message.
	* @param to The receiver of the message as a puck ID.
	* @param An int that contains the property ID, given by the 
	* Barrett's documantation tables. 
	* @param value The value of the property.
	* @param Flag that informs the function if the receiver ID is one puck or
	* a group of pucks. If true we have a group message, if false a single 
	* receiver.
	* @param set Flag that informs the function if we have a SET PROPERTY or a
	* GET PROPERTY command.
	* @return An error code. Returning 0 means we have no error.
	*/
	int BarrettCANProtocolEncode(CANMessage* msg, int8_t to, int8_t property, int32_t value, bool group, bool set);
	
	/**
	* The function decodes a a raw CAN message (CANmessage object) to the 
	* high level information understood by the user and the hand.
	* setProperty and getProperty and it implements the Barrett CAN Protocol.
	* @param msg The raw CAN Message to be decoded.
	* @param value Pointer that will contain the data value within the CAN message.
	* @param packed Flag that contains the information about the type of the 
	* incoming message. If 0 it is a regular message, if 1 is a 22-bit packed
	* message that contains the value of JP property and if 2 is a dual packed
	* message containing two 22-bit values of P and JP properties, respectively.
	* @return An error code. Returning 0 means we have no error.
	*/
	int BarrettCANProtocolDecode(CANMessage msg, int* value, int packed);

	/**
	* The function that decodes a dual packed message (two 22-bit values), 
	* where the getProperty API is unable to.
	* @param node An int that points to the puck's ID. 11-14 is for FINGER1 to 
	* FINGER3 and SPREAD and 15 is for the group of all the pucks of the hand.
	* @param pos Pointer that will contain the current value of the P property.
	* @param jpos Pointer that will contain the current value of the JP property.
	* @return An error code. Returning 0 means we have no error.
	*/
	int getDualPackedPosition(int node, int* pos, int* jpos);
	
	/**
	* Function that initialized the basic properties' values to their default
	* value. It should be called after the init() function.
	* @return An error code. Returning 0 means we have no error.
	*/
	int initPropValues();
	
	/**
	* Function that closes all the fingers together.
	* @return An error code. Returning 0 means we have no error.
	*/
	int close();
	
	/**
	* The function for initialize the BHand without open the fingers to initial
	* position. It is recommended to be used under exceptional circumstances and
	* to use the init() function.
	* @return An error code. Returning 0 means we have no error.
	* @see CANBus::init()
	*/
	int init2();
};

#endif 
