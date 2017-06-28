#include <bad/extra_library.h>

float myRound(double f,float prec)
{
    return (float) (floor(f*(1.0f/prec) + 0.5)/(1.0f/prec));
}

double enc2rad(int enc) 
{
    double pi = 3.141592653589;
    return enc * (140*pi/180)/195000.0;
}

int rad2enc(int rad)
{
    double pi = 3.141592653589;
    double enc = rad / ((140*pi/180)/195000.0);
    return (int)enc;
}

double enc2deg(int enc)
{
    double deg = enc * 140/195000;
    return myRound(deg, 2);
}

int deg2enc(float deg)
{
    double enc = deg * 195000/140;
    return (int)enc;
    
}
