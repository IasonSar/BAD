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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <asm/types.h> // lib for __u32 types
#include <unistd.h>
#include <stdint.h> // lib for uint_8, etc types
#include <unistd.h> // lib for usleep
#include <string>   // lib for handling strings
#include <algorithm> // lib for std::find() to find objects within an array
#include <ctime> // time, clock etc
#include <sys/time.h>
#include <cstring> //for bad logger strcat
#include <sstream>
#include <pthread.h>
#include <cmath>

#define DWORD  uint32_t //__u32  // todo check if we can do this __u16 or it depends on
					    // operating system. msg.id, status
#define WORD   uint16_t // unsigned short
#define BYTE   uint8_t  //unsigned char msg.len, msg.data[8]
#define LPSTR  char*

#include <PCANBasic.h>

#define PCAN_DEVICE	PCAN_PCIBUS1
//#define PCAN_DEVICE	PCAN_USBBUS1



#define MD_ERROR_OK 0
#define MD_ERROR_FAIL 100
#define MD_ERROR_ARGS 200
#define MD_ERROR_ELSE 300

using namespace std;


/**
* A CAN Message class. It implements a basic CAN Message given the DLC, MSGID 
* and 8 byte of data. Note that this class is about an 8-bit message identifier
* (standard CAN).
*/
class CANMessage {
public:
	uint8_t DLC; /**< The DLC segment of the CAN Message. It represents the number of data bytes */
	uint8_t MSGID; /**< The message ID segment of the CAN Message. */
	uint8_t DATA[8]; /**< An array for 8 bytes of data. */
};


/**
* The CAN Bus class. It implements the functionality of the CAN bus.
*/
class CANbus {
	bool initialized; /**< Flag that contains the informatios regarding if the CAN bus has been initialized. */
	string msg_type; /**< A string about the messages types in the prints of this class members functions. */
	string err_type; /**< A string about the error messages types in the prints of this class members functions. */
public:
	/**
	* The constructor of CANbus.
	* It initialize the variables
	*/
	CANbus();
	
	/**
	* The destructor of CANbus.
	*/
	~CANbus();
	
	/**
	* The initialization function of the CAN bus. You have to call this function
	* before anything else, to initiate the communication process.
	* @return An error code. Returning 0 means we have no error.
	*/
	int init();
	
	/**
	* The function for reseting the CAN bus. It is strongly recommended to 
	* use the init() function instead, due to glitchy behaviour.
	* @return An error code. Returning 0 means we have no error.
	*/
	int reset();
	
	/**
	* The function for writing a message on the CAN bus.
	* @param msg the CAN message to write on the CAN bus.
	* @return An error code. Returning 0 means we have no error.
	*/
	int write(CANMessage msg);
	
	/**
	* The function for reading a message on the CAN bus.
	* @param msg the pointer of CAN message put the data that are being read from the CAN bus.
	* @return An error code. Returning 0 means we have no error.
	*/
	int read(CANMessage* msg);
};


