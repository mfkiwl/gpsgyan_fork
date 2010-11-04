 #include "DayTime.hpp"
 #include "Position.hpp"
 #include "satdbtypes.hpp"
#include "EngineFramework.hpp"
#include "SatDgen.hpp"
#include "SatDb.hpp"

#ifndef SATMDEOSTRUCT
#define SATMDEOSTRUCT

using namespace gpstk;
typedef struct
{
//std::vector<satDb_ostruct> *satDb_ref;
SatDgen  *satDgen;
SatDb	 *satDb;
Position    rxXvt;
DayTime measTime;
}satMDE_actIstruct;


typedef struct
{
double minElev;
}satMDE_psvIstruct;

typedef struct
{
SatID  prn;
satDgen_opDataPort dataNmeas;
double rangeRate;

double carrierDopplerL1 ;
double codeDopplerCaL1;

double carrierDopplerL2;
double codeDopplerCaL2;

double carrierDopplerL5 ;
double codeDopplerPyL5;
bool   measurementHealth;


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


