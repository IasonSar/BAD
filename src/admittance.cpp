#include <iostream>
#include <fstream> //stream class to read/write on files
#include <boost/array.hpp>

#include <boost/numeric/odeint.hpp>

using namespace std;
using namespace boost::numeric::odeint;

const double M = 100;
const double B = 1;
const double K = 2;
double SG = 2000;
ofstream data;

typedef boost::array< double, 2 > state_type;

void admittance(const state_type &x, state_type &dxdt, double t)
{
	dxdt[0] = x[1];
	dxdt[1] = (SG - B*x[1] - K*x[0])/M;
}

void write_adm(const state_type &x, const double t)
{
	data << t << "," << x[0] << endl;
}

int main(int argc, char **argv)
{

	data.open("data.txt");

	state_type x_init = {0, 0};
	integrate(admittance, x_init, 0.0, 25.0, 0.001, write_adm);

	data.close();
}


