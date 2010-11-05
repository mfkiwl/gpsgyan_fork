
#include <windows.h>
#include "SimTime.hpp"
#include "SatMde.hpp"
#include "RxDE.hpp"
#include "timer.hpp"


#include <iostream>
#include <conio.h>
 #include "EngineFramework.hpp"


bool bStillBusy = false;
bool bStilltimer = 0;
using namespace std;
using namespace gpstk;

void CALLBACK TimerProc1(HWND, UINT, UINT, DWORD);

int main()
{
 UINT id;
 MSG msg;
 bStillBusy  = true;
 bool f1= false;
  bool f2= false;
   bool f3= false;
    bool f4= false;

 id = SetTimer(NULL, 0, 1000, (TIMERPROC) TimerProc1);



 // Instantiate the Engines
// SatRF crapSatrf("haku");
  SimTimeEngine crap("time");				// responsible for base time generations
  SatDb			crapdb("satellite database");
  SatMDE		crapsatmde("satellite manager");
  SatDgen		crapsatdgen("satellite data and Meas Gen");
  CodeGen		codegen("satellite code gen");
  CarrGen		carrgen("satellite carr gen");
  ChannelGPS    channel("channel to add delay");
  RxDE			craprxDE;


	ofstream s1("try1.txt");
	ofstream s2("try2.txt");
	ofstream s3("try3.txt");
	ofstream s4("try4.txt");
	ofstream s5("try5.txt");

 /* Connection of engines <passive Inputs and Param> mainly from GUI*/
	DayTime time1;
    time1.setSystemTime();
   //engineMode = 0;
    crap.InputInfToEngine(time1);
    crap.InputParamToEngine();
 /* Passive connection for RXDE*/
 double lat = 35.772020;
 double lon = 78.67410;
 double alt = 437.00;
 
 
 Position rx_ant1, rx_ant2, rx_ant3;
 //rx_ant1.setGeodetic(35.772020,78.67410,437.00);
 rx_ant1.setGeodetic(lat,lon,alt);
 rx_ant1.transformTo(Position::Cartesian);
 
 
 //rx_ant1.setGeodetic(35.772020,78.67410,437.00);
 //rx_ant1.transformTo(Position::Cartesian);
 //craprxde.ref_psv.rx_llh=temp;



/* Passive connection for SatDB*/
crapdb.passiveData.yumaFileName="current.alm";
crapdb.passiveControl.useFullGpsWeek=true;

/* Passive connection for SatDgen*/
crapsatdgen.passiveData.psvInputChk=false;
crapsatdgen.passiveControl.debugLevel=2;

/* Passive connection for SatMDE*/
crapsatmde.ref_psv.minElev=5;

/* Carrgen passive Param*/
carrgen.passiveControl.IF1mult = 100;
carrgen.passiveControl.IF2mult = 4;
carrgen.passiveControl.rxConstClk = 15.05e6;

/* Channel passive Param*/
channel.passiveData.alpha[0] = 0.010e-6;
channel.passiveData.alpha[1] = 0.000e-6;
channel.passiveData.alpha[2] = -0.060e-6;
channel.passiveData.alpha[3] = 0.000e-6;
channel.passiveData.beta[0] =90e3;
channel.passiveData.beta[1] =0e3;
channel.passiveData.beta[2] =-197e3;
channel.passiveData.beta[3] =0e3;
channel.passiveData.loopBWpll = 0.7;
channel.passiveData.predetectIT = 2e-3;
channel.passiveData.armSpacing =0.6;
channel.passiveData.loopBWdll = 0.2;
channel.passiveData.c_n0 = 28.25;
channel.passiveData.chipWidth=977.5171065e-9;

string  el;
el = "";






/***********************************************************/
/* Initialization of all engine based on the passive input
/* and param mainly form GUI
/***********************************************************/

//crap.Engine(SimTimeEngine::INITIALIZE);
crapdb.Engine(SatDb::INITIALIZE);
crapsatdgen.Engine(SatDgen::INITIALIZE);
carrgen.Engine(CarrGen::INITIALIZE);
codegen.Engine(CodeGen::INITIALIZE);
channel.Engine(ChannelGPS::INITIALIZE);
craprxDE.Engine(RxDE::INITIALIZE);
crapsatmde.GetPassiveParam();
crapsatmde.Engine(SatMDE::INITIALIZE);

//craprxde.GetPassiveInput(craprxde.ref_psv);
crapsatmde.GetPassiveInput(crapsatmde.ref_psv);
crap.Engine(SimTimeEngine::INITIALIZE);

  //crapdb.Engine(SatDb::INITIALIZE);
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
	craprxDE.activeData.trueGPStime = crap.rxModelledTime;
	craprxDE.Engine(RxDE::RUN_METHOD );
	craprxDE.EngineOutput(RxDE::DATA_HERE);

	crapsatmde.ref_act.measTime =craprxDE.rxDE_oData.rx_time;
	crapsatmde.ref_act.rxXvt = rx_ant1;
	crapsatmde.ref_act.satDb = &crapdb;
	crapsatmde.ref_act.satDgen = &crapsatdgen;
	crapsatmde.ref_act.carrgen = &carrgen;
	crapsatmde.ref_act.codegen = &codegen;
	crapsatmde.ref_act.channel = &channel;
	crapsatmde.GetActiveParam();

	
	/* RXDE throw the GPS True time i.e T_rx - rx error*/

	


	crapsatmde.InputInfToEngine(EngineFramework::ACTIVE);
	crapsatmde.Engine(SatMDE::RUN_METHOD);

	

	cout<<craprxDE.rxDE_oData.rx_time.printf("%Y %03j % 12.12s") << endl;
	

	if(!f1)
	{   using namespace StringUtils;
		f1=true;
		SatID satid = crapsatmde.visibleSV[1];
		crapsatmde.outputInfFromEngine (s1,satid,0);
		satid = crapsatmde.visibleSV[2];
		crapsatmde.outputInfFromEngine (s2,satid,0);
		satid = crapsatmde.visibleSV[3];
		crapsatmde.outputInfFromEngine (s3,satid,0);
		satid = crapsatmde.visibleSV[4];
		crapsatmde.outputInfFromEngine (s4,satid,0);
		el +=craprxDE.rxDE_oData.opDataHeader;
		el += leftJustify("\n",3);
		s5<<el;
	}
	el = "";

	
	using namespace StringUtils;;
	el += craprxDE.rxDE_oData.opDataInString;
	el += leftJustify("\n",3);
	s5<<el;


	SatID satid = crapsatmde.visibleSV[1];
	crapsatmde.outputInfFromEngine (s1,satid,1);

	satid = crapsatmde.visibleSV[2];
	crapsatmde.outputInfFromEngine (s2,satid,1);

	satid = crapsatmde.visibleSV[3];
	crapsatmde.outputInfFromEngine (s3,satid,1);

		satid = crapsatmde.visibleSV[4];
	crapsatmde.outputInfFromEngine (s4,satid,1);


	crap.Engine(SimTimeEngine::RUN_METHOD);
	}


	if(_kbhit())
{
	KillTimer(NULL, id);
	  s1.close();
	  s2.close();
	    s3.close();
		 s4.close();
		 s5.close();
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

