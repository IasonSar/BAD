#include <cmath>

/**
    enc = encoder counts of the pucks
    rad = radians
    
    The following 2 methods is about the 2 transformations:
    enc -> rad
    rad -> enc
*/

float myRound(double f,float prec);
double enc2rad(int enc);
int rad2enc(int rad);
double enc2deg(int enc);
int deg2enc(float deg);
