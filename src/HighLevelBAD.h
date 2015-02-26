#include "BarrettHand.h"
#include "kinematics.h"
#include <omp.h> // for benchmarking and timing
//#include "angle_transform.hh"

//#include "extra_library.hh"


class BAD {
    BHand *hand;
    HandState *state;
    string msg_type;
	string err_type;
	
	int min_sg[3];
	int max_sg[3];
	
	bool sg_inited;
	
public:
	BAD(BHand*);
	~BAD();
    
/** Basic functionality of BarretHand */
	int open(int);
	void initHand();
	void terminate();
	bool doneMoving(int);
	void waitDoneMoving(int);
	void goToRelaxedPos();
	void initSG(bool);
	double getSG(int finger, bool);
	void setHLSG(int finger, double HSGperc, double LSGperc);

/** Testing and Benchmarking Tools */
	void testTiming();
	void test();
	void testingGetPosition();
	void logger();
	
/** Applications and uses of BarrettHand */
	void fetchAndRelease(int);
	void fetchAndRelease2(int);
	void handShake(HandState);
	void precisionGrasp();
	void precisionGrasp2();
	void distortionControl(double distortion);
	void simpleGrasp(int);
	void holdGrasp(int);

/** Under development and TODOs */	
	void release(int);
	void release2(int topFinger);
	void detectBreakaway(HandState* state);
	void staple();
	void simpleCylinderGrasp();
	void syringe();
	void touchAndGrab(HandState state);

/** Auxiliary functions */
	double calculateVariance(int matrix[]);
};


