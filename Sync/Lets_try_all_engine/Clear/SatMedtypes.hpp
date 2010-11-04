 #include "DayTime.hpp"
 #include "Position.hpp"
 #include "satdbtypes.hpp"

#ifndef SATMDEOSTRUCT
#define SATMDEOSTRUCT

using namespace gpstk;
typedef struct 
{
std::vector<satDb_ostruct> *satDb_ref;
Xvt    rxXvt;
DayTime gpsTrueTime;
}satMDE_actIstruct;


typedef struct 
{
double minElev;
}satMDE_psvIstruct;
 
typedef struct 
{
SatID  prn;
double tropo_delay;
double phaseWindUp;
double phaseCenter;
double rawRange;
double corrRange;
double elevation;
double azimuth;
double rangeRate;
double ionoDelayL1;
double carrierDopplerL1 ;
double codeDopplerCaL1;
double ionoDelayL2;
double carrierDopplerL2;
double codeDopplerCaL2;
double ionoDelayL5 ;
double carrierDopplerL5 ;
double codeDopplerPyL5;



}satMDE_ostruct;

typedef struct 
{
SatID  prn;
double range;
}satMDE_range;

struct phaseData
{
  // Default constructor initializing the data in the structure
phaseData() : previousPhase(0.0) {};
double previousPhase;      
};
#endif


