 #include "DayTime.hpp"
 #include "Position.hpp"
#include "Xvt.hpp"


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
double   satclkdrift;
double   relativity;
Xvt      satXvt; 
}satDb_ostruct;

#endif
