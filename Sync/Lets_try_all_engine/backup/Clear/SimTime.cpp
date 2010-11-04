// System
//#include <stdio.h>

// gpstk

#include "SimTime.hpp"
//#include "timer.hpp"

//template<class X>
using namespace gpstk;
using namespace std;
void SimTimeEngine::InputInfToEngine(DayTime &time )
{

simStartTime = time;
//simStartTime.setSystemTime();
simEndTime = simStartTime;
//simEndTime.addSeconds((double)3600);
startSimTime = true;
stopSimTime= false;
pauseSimTime=false;
}




void SimTimeEngine::InputParamToEngine() throw(InvalidParameter)
{
simtimeIsSystemTime = true;
simtimeIsTimer=false;

//enableSatClockModel= false;



//debugLocLevel = 1;
//verboseLocLevel= 1;
}

//void SimTimeEngine:: prepareIpToEngine()



void SimTimeEngine:: EngineControl(Kind kind)

{
	switch(kind)
	{ case PASSIVE :
		isThisFirstTick = false; 
	  // Initialize GPS true time to system time then increment the second based on timer expiration
    	
	  break;

	case ACTIVE :

		break;
	default :
		isThisFirstTick = false; 
		break;
		
	}
	// set debug level etc
}


void SimTimeEngine::VerifyIpToEngine()  throw(InvalidParameter) 

{

	if(simStartTime.second() < 1)
simTimerExpired = false;
}



void SimTimeEngine::MethodOfEngine() throw(ExcSimTimeEngine)

{
	
if(!isThisFirstTick)
		{
		simTimebase = simStartTime;
		isThisFirstTick = true;
		}

	// Addtion of 1 seconds 

    if(simTimerExpired)
	{
	simTimerExpired = false;
	simTimebase.addSeconds((double)1);
	gpsTrueTime = simTimebase ;
		
	
    // Model Receiver clock
	rxModelledTime = simTimebase;

	}
   }
	
  
		
		
		

// ----------- Part  4: Probe functions: dump -----------------------------
void SimTimeEngine::dump(std::ostream& s)
    {
       using std::endl;
       using std::setw;


       s << std::setprecision(4);    // set the precision of probe output
       s.setf(std::ios::scientific); // set the mode of probe output

	   s << " Detail of gpstime passed to object of class :GPSYumaDb  "<<endl;
	   s << "------------------------------------------------------------------------- "<<endl;
       //almStore.dump(s);
	   s << "------------------------------------------------------------------------- "<<endl;
	   s << " Detail of Almanac for Satellite ID passed to object of class :GPSYumaDb  "<<endl;
	   s << "------------------------------------------------------------------------- "<<endl;
	  // gps_yumadb_amlptr.dump(s);

    }





void SimTimeEngine::PrepareOpOfEngine()
{
	cout<<simTimebase.printf("%S");
	
}


void SimTimeEngine:: OutputDumpOfEngine()
{
	ofstream s("try.txt");
	dump(s);
	s.close();
}

//void CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#if 0
void main()
{
   SimTimeEngine crap("good1");

   //if (!crap.initialize(argc, argv))
     // exit(0);
   DayTime time1;
   //Timer_class t1;
   

//static int hero=1000;
   crap.inputInfToEngine(time1);
crap.inputParamToEngine();
  // crap.runEngine();
  // m_timerID = SetTimer(NULL, 1, (UINT)2500, (TIMERPROC)OnTimer);
/*while(1)
{
 time1.setSystemTime();
 crap.inputInfToEngine(time1);
 crap.loopEngine();
crap.outputInfFromEngine();
   cout<<endl;
if(kbhit())
break;
}*/
//crap.outputDumpOfEngine();

}
#endif
/*void CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{static int hero;
       hero++;
}*/













































































































































































































































