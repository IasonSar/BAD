/**
	CANbus.cpp

	Author: Iason Sarantopoulos
	Institute: AUTh (Aristotle University of Thessaloniki)
	Last update: 28/02/2015
	
	CANbus.cpp is part of BAD (Barretthand AUTh Driver). BAD is a 
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

#include <bad/CANbus.h>

CANbus::CANbus() 
{ 
    initialized = false;
    msg_type = "[CAN bus] ";
    err_type = "[Error] CANbus::";
}
CANbus::~CANbus() {}

int CANbus::init()
{
/** Check if the CAN bus is initialized. If it is, do not do anything. If it is
	not, call the CAN_Initialize from PCAN-Basic and change the CANbus::initialized
	value to true.
*/
    if (!initialized) {
	    TPCANStatus Status;
	    cout << msg_type << "Initializing...\n";
	
	    Status = CAN_Initialize(PCAN_DEVICE, PCAN_BAUD_1M, 0, 0, 0);
	    if (Status != PCAN_ERROR_OK) {
		    printf("[CAN bus] Error %d! CAN bus failed to be initiliazed.\n", Status);
		    return MD_ERROR_FAIL;
	    }
	    else {
	        cout << msg_type << "Initialiazation is done.\n";
		    initialized = true;
	    }
	
	    usleep(200*1000); // Sleep for 200 ms
    }
    else {
        printf("[CAN bus] is already initialized!\n");
    }
	
	return MD_ERROR_OK;
}

int CANbus::reset() 
{
    TPCANStatus Status;
    
    Status = CAN_Reset(PCAN_DEVICE);
    
    if (Status != PCAN_ERROR_OK) {
		printf("[CAN bus] Error %d! CAN bus reset failed.\n", Status);
		return MD_ERROR_FAIL;
	}
	
	usleep(2*1000);
	
	cout << msg_type << "Reset Successfully\n";
	
	return MD_ERROR_OK;	
}

int CANbus::write(CANMessage msg)
{	

/** Create types of PCAN-Basic. TPCANMsg is a PCAN Message for PCAN-Basic. */
	TPCANMsg Message;
	TPCANStatus Status;

/** Take the generic CANmessage msg and put it to TPCANMsg. */
	Message.MSGTYPE = PCAN_MESSAGE_STANDARD;	
	Message.LEN = msg.DLC;
	Message.ID = msg.MSGID;
	for (int i = 0; i<msg.DLC; i++ )
		Message.DATA[i] = msg.DATA[i];

/** Check if the CAN bus is initialized. If not return an error. */
	if (!initialized) {
	    cerr << err_type << __FUNCTION__ << "The bus is not initialized. Try to initialize the bus first.\n";
		//printf("Error: The bus is not initialized. Try to initialize the bus first.\n");
		return MD_ERROR_FAIL;
	}
	
/** Write the TPCANMsg message to CAN bus with PCANBasic's CAN_Write. */
	if ((Status=CAN_Write(PCAN_DEVICE, &Message)) != PCAN_ERROR_OK) {
		printf("[CAN bus] Error %d! CAN Message writing has failed.\n", Status);
		return MD_ERROR_FAIL;
	}
	
	usleep(100); // Sleep for 100 us
	
	return MD_ERROR_OK;	
}


int CANbus::read(CANMessage* msg) //todo how to pass the Message as an argument, see get_property
{   
/** Create types of PCAN-Basic. TPCANMsg is a PCAN Message for PCAN-Basic. */
    TPCANMsg Message;
	TPCANStatus Status;
	
/** Define a counter and a boolean for 32-bit or 16 bit properties. */
    int counter = 0;
    bool is32bit = false;
	
/** Begin a loop of reading the CANbus using PCAN-Basic's CAN_Read. The loop
	will run for 500 times every 1 ms checking every time if the CAN bus has data
	to read. If nothing is there the loop will run one more time. After the 500 
	ms of running the function will return an error because probably connection
	lost. Otherwise the loop will not run, and the CAN_Read will just read the
	message. */
	while ((Status=CAN_Read(PCAN_DEVICE, &Message, NULL)) == PCAN_ERROR_QRCVEMPTY) {
		usleep(1*1000);
	    counter++;
	    if (counter > 500) {
	        printf("[CANbus] Error! Connection Timed out. \n"
	               "         Cause: Waiting more than 500ms to read something from CANbus.\n"
	        	   "         1. Check the connection between the bus and the BarrettHand.\n"
	        	   "         2. Check if the BarrettHand is powered and switched on.\n"
	        	   "         3. Check if the BHand led is on and is green.\n");
	        return MD_ERROR_FAIL;
	    }
	}
	
/** If we don't have a PCAN_ERROR_OK, print the error. */
	if (Status != PCAN_ERROR_OK) {
		printf("Error 0x%x\n",(int)Status);
		return MD_ERROR_FAIL;
	}

/** Finally, put the TPCANMsg message to our own generic CANMessage msg. */
	msg->DLC = Message.LEN;
	msg->MSGID = Message.ID;
	for (int i = 0; i<Message.LEN; i++ )
		msg->DATA[i] = Message.DATA[i];
		
	return MD_ERROR_OK;	
}
