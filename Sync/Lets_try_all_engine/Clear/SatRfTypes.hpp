 #include "DayTime.hpp"
 #include "Position.hpp"
 #include "satdbtypes.hpp"
 #include "SatMedtypes.hpp"
#include <boost/circular_buffer.hpp>
   #include <assert.h>
#include <vector>

using namespace gpstk;
typedef struct
{
std::vector<satDb_ostruct> *satDb_ref;
std::vector<satMDE_ostruct> *satMde_ref;
DayTime gpsTrueTime;
}satRF_actIstruct;


typedef struct
{
double minElev;
}satRF_psvIstruct;

typedef struct
{
SatID  prn;
int   firstBlock[1023];
int   firstBlockSlew;
std::vector<int> lastblock;
int   totalCompleteCodeBlocks;
double lastBlockPhase;
int    lastBlockSlew;

double   firstCycle[1023];
long   firstCycleCoarsePhase;
std::vector<double> lastCycle;
int   totalCompleteCycles;
double lastCycleFinePhase;
long   lastCycleCoarsePhase;

}satRF_ostruct;

typedef struct
{
//boost::circular_buffer<char> codeArray;
boost::circular_buffer<int> codeArray;
int prvSlew;
double prvSlewPhase;
}satRF_caCodeArray;

typedef struct
{
//boost::circular_buffer<char> codeArray;
boost::circular_buffer<double> carrArray;
int prvCoarsePhase;
double prvCarrierFinePhase;
}satRF_carrierArray;

typedef struct
{
satRF_caCodeArray codeVector;
satRF_carrierArray carrierVector;
bool codeCarrierGenerated;
//bool isVisible;

}codeAndCarrierDB;



