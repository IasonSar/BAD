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

/** Properties of BarrettHand**/
const int ACCEL		= 82;
const int CMD		= 29;
const int CT		= 56;
const int OT		= 54;
const int MODE		= 8;
const int VERS		= 0;
const int STAT		= 5;
const int TSTOP		= 78;
const int V		= 44;
const int ROLE		= 1;
const int KP		= 79;
const int KI		= 81;
const int KD		= 80;
const int M		= 58;
const int P		= 48;
const int E		= 52;
const int LSG		= 72;
const int HSG		= 71;
const int SG		= 25;
const int MT		= 43;
const int JP		= 96;
const int HOLD		= 77;
const int MCV		= 46;
const int MOV		= 57;
const int T		= 42;
const int MV		= 45;


/** Other constants **/
const int FINGER1           = 11;
const int FINGER2           = 12;
const int FINGER3           = 13;
const int SPREAD            = 14;
const int HAND              = 15;
const int MAX_TICKS_FINGER  = 199111;
const int MAX_TICKS_SPREAD  = 35840;
const double PI				= 3.14159265359;



// For mode property
const int MODE_IDLE         = 0;
const int MODE_TORQUE       = 2;
const int MODE_PID          = 3;
const int MODE_VELOCITY     = 4;
const int MODE_TRAPEZOID    = 5;

// For STAT property
const int STATUS_RESET      = 0;
const int STATUS_READY      = 2;

// For CMD property
const int CMD_HI            = 13;
const int CMD_IC            = 14;
const int CMD_IO            = 15;
const int CMD_TC            = 16;
const int CMD_TO            = 17;
const int CMD_CLOSE         = 18;
const int CMD_MOVE          = 19;
const int CMD_OPEN          = 20;


