//#include "SatDb.hpp"
//#include <stdafx.h>


#include <windows.h>
//#define _MSC_VER  1000
#include "SimTime.hpp"
#include "SatDb.hpp"
#include "RxDE.hpp"
#include "SatMde.hpp"
#include "timer.hpp"
#include "SatRf.hpp"

#include <iostream>
#include <conio.h>
 #include "EngineFramework.hpp"


bool bStillBusy = false;
bool bStilltimer = 0;
using namespace std;
using namespace gpstk;

void CALLBACK TimerProc1(HWND, UINT, UINT, DWORD);
//void mynum();

int main()
{
 UINT id;
 MSG msg;


 double lat = 35.772020;
 double lon = 78.67410;
 double alt = 437.00;

 Position temp;
 temp.setGeodetic(35.772020,78.67410,437.00);
 SatRF crapSatrf("haku");


bStillBusy  = true;
 
 id = SetTimer(NULL, 0, 1000, (TIMERPROC) TimerProc1);

 SimTimeEngine crap("good1");
 SatDb crapdb("good2");
 RxDE  craprxde("good3");
SatMDE  crapsatmde("good4");
 ofstream s1("try1.txt");
 ofstream s2("try2.txt");
  ofstream s3("try3.txt");
 ofstream s4("try4.txt");

crapsatmde.ref_psv.minElev=5;

crapdb.yumaFileName = "current.alm";

crapdb.InputInfToEngine(EngineFramework::PASSIVE);


craprxde.ref_psv.rx_llh=temp;
craprxde.GetPassiveParam();
crapsatmde.GetPassiveParam();
craprxde.GetPassiveInput(craprxde.ref_psv);
crapsatmde.GetPassiveInput(crapsatmde.ref_psv);
crapSatrf.Engine(SimTimeEngine::INITIALIZE);

// SimTimeEngine::Mode engineMode;
//  crap.engMode  = runAll;

   DayTime time1;
   time1.setSystemTime();
   //engineMode = 0;
   crap.InputInfToEngine(time1);
   crap.InputParamToEngine();
   crap.Engine(SimTimeEngine::INITIALIZE);
   //time2.time_expired = false;
 //time2.timerset(2000);
  

 while(1) 
 {
	 


	GetMessage(&msg, NULL, 0, 0);
	DispatchMessage(&msg);
	if(bStilltimer)
	{
	bStilltimer = false;
	crap.simTimerExpired=true;
	
	// Run the RX time and Postion Generator
	//Input
	craprxde.ref_act.rx_time=crap.gpsTrueTime;
	craprxde.InputInfToEngine(EngineFramework::ACTIVE);
	craprxde.Engine(RxDE::RUN_METHOD);
	craprxde.outputInfFromEngine (s2);

	crapdb.inp_time = craprxde.RxDE_oData.trueGPStime;
	crapdb.visiblityFlag = crapsatmde.visiblityComputed;
	crapdb.vislist_ref = &crapsatmde.visibleSV;
	crapdb.InputInfToEngine(EngineFramework::ACTIVE);
    crapdb.Engine(SatDb::RUN_ALL);
	crapdb.outputInfFromEngine (s1);
	
	crapsatmde.GetActiveParam();
	

	crapsatmde.ref_act.satDb_ref= &crapdb.satDb_oData;
	//crapsatmde.ref_act.satXvt= crapdb.satDb_oData[prn].satXvt;
	crapsatmde.ref_act.gpsTrueTime=craprxde.RxDE_oData.trueGPStime;
    crapsatmde.ref_act.rxXvt=craprxde.RxDE_oData.rxXvt;
	crapsatmde.InputInfToEngine(EngineFramework::ACTIVE);
	crapsatmde.Engine(RxDE::RUN_METHOD);



	crapSatrf.ref_act.satDb_ref= &crapdb.satDb_oData;
	crapSatrf.ref_act.satMde_ref= &crapsatmde.satMDE_oData;
	//crapsatmde.ref_act.satXvt= crapdb.satDb_oData[prn].satXvt;
	crapSatrf.ref_act.gpsTrueTime=craprxde.RxDE_oData.trueGPStime;
    
	crapSatrf.InputInfToEngine(EngineFramework::ACTIVE);
	crapSatrf.Engine(RxDE::RUN_METHOD);
	crapSatrf.outputInfFromEngine (s4);
	

	cout<<craprxde.RxDE_oData.trueGPStime.printf("%Y %03j % 12.12s") << endl;
	cout<<"Timer func got called"<<endl;

	crapsatmde.outputInfFromEngine (s3);
	crap.Engine(SimTimeEngine::RUN_METHOD);
	}


	if(_kbhit())
{
	KillTimer(NULL, id);
	   s1.close();
	   s2.close();
	    s3.close();
		 s4.close();
	break;
}

 }


 
 return 0;
}


void CALLBACK TimerProc1(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime) 
{

 bStilltimer =true;
bStillBusy = false;
}
 
