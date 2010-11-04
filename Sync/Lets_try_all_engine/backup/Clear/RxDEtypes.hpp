 #include "DayTime.hpp"
#include "Xvt.hpp"

using namespace gpstk;
typedef struct 
{
DayTime   rx_time;
}rxDE_actIstruct;


typedef struct 
{
Triple    rx_Rxyz;
Triple    rx_Vxyz;
Triple    rx_Axyz;
Triple    rx_Jxyz;
Position    rx_llh;
}rxDE_psvIstruct;
 
typedef struct 
{
DayTime		trueGPStime;
Xvt			rxXvt;
DayTime		rx_time;

}RxDE_ostruct;


