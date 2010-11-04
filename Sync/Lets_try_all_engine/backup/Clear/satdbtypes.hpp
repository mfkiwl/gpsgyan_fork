 #include "DayTime.hpp"
 #include "Position.hpp"
# include "Xvt.hpp"
using namespace gpstk;
#ifndef SATDBOSTRUCT
#define SATDBOSTRUCT

typedef struct 
{
public :
SatID    prn;
bool	 sathealth;
DayTime  sattime;
double   satclkbias;
double   clkCorrection;
double   relativity;
double	carrFreqL1; 		
double	carrFreqL2;
double	carrFreqL5;
double  satOscFreq;
double	chipFreqCA;
double	chipFreqPY;

Xvt      satXvt; 
}satDb_ostruct;
#endif