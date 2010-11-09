
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
 
  
  SatMDE		mdp_rxAnt1("satellite manager for 1st Antenna");
  SatMDE		mdp_rxAnt2("satellite manager for 2nd Antenna");
  SatDgen		satDgen_rx1("satellite data and Meas Gen");
  SatDgen		satDgen_rx2("satellite data and Meas Gen");
  CodeGen		codegen_rx1("satellite code gen");
  CodeGen		codegen_rx2("satellite code gen");
  CarrGen		carrgen_rx1("satellite carr gen");
  CarrGen		carrgen_rx2("satellite carr gen");
  
  RxDE			rx1_clock,rx2_clock;

  
  SimTimeEngine crap("time");				// responsible for base time generations
  SatDb			crapdb("satellite database");




  ChannelGPS    channel("channel to add delay");
  
  
  



	ofstream s1("try1.txt");
	ofstream s2("try2.txt");
	ofstream s3("try3.txt");
	ofstream s4("try4.txt");
	ofstream s5("try5.txt");
	ofstream s6("carr.txt");

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
 
 alt = alt+100;
rx_ant2.setGeodetic(lat,lon,alt);
 rx_ant2.transformTo(Position::Cartesian);
 
 
 //rx_ant1.setGeodetic(35.772020,78.67410,437.00);
 //rx_ant1.transformTo(Position::Cartesian);
 //rx1_clock.ref_psv.rx_llh=temp;



/* Passive connection for SatDB*/
crapdb.passiveData.yumaFileName="current.alm";
crapdb.passiveControl.useFullGpsWeek=true;

/* Passive connection for SatDgen*/
satDgen_rx1.passiveData.psvInputChk=false;
satDgen_rx1.passiveControl.debugLevel=2;

satDgen_rx2.passiveData.psvInputChk=false;
satDgen_rx2.passiveControl.debugLevel=2;

/* Passive connection for SatMDE*/
mdp_rxAnt1.ref_psv.minElev=5;
mdp_rxAnt2.ref_psv.minElev=5;
/* Carrgen passive Param*/
carrgen_rx1.passiveControl.IF1mult = 100;
carrgen_rx1.passiveControl.IF2mult = 4;
carrgen_rx1.passiveControl.rxConstClk = 15.05e6;


carrgen_rx2.passiveControl.IF1mult = 100;
carrgen_rx2.passiveControl.IF2mult = 4;
carrgen_rx2.passiveControl.rxConstClk = 15.05e6;

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



channel.Engine(ChannelGPS::INITIALIZE);
crap.Engine(SimTimeEngine::INITIALIZE);


satDgen_rx1.Engine(SatDgen::INITIALIZE);
satDgen_rx2.Engine(SatDgen::INITIALIZE);
carrgen_rx1.Engine(CarrGen::INITIALIZE);
carrgen_rx2.Engine(CarrGen::INITIALIZE);
codegen_rx1.Engine(CodeGen::INITIALIZE);
codegen_rx2.Engine(CodeGen::INITIALIZE);

rx1_clock.Engine(RxDE::INITIALIZE);
rx2_clock.Engine(RxDE::INITIALIZE);

mdp_rxAnt1.GetPassiveParam();
mdp_rxAnt1.Engine(SatMDE::INITIALIZE);

//rx1_clock.GetPassiveInput(rx1_clock.ref_psv);
mdp_rxAnt1.GetPassiveInput(mdp_rxAnt1.ref_psv);


/* Check this*/
mdp_rxAnt2.GetPassiveParam();
mdp_rxAnt2.Engine(SatMDE::INITIALIZE);

//rx1_clock.GetPassiveInput(rx1_clock.ref_psv);
mdp_rxAnt2.GetPassiveInput(mdp_rxAnt1.ref_psv);





  





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
	rx1_clock.activeData.trueGPStime = crap.trueGPStime;
	rx1_clock.Engine(RxDE::RUN_METHOD );
	rx1_clock.EngineOutput(RxDE::DATA_HERE);

	mdp_rxAnt1.ref_act.measTime =rx1_clock.rxDE_oData.rx_time;
	mdp_rxAnt1.ref_act.rxXvt = rx_ant1;
	mdp_rxAnt1.ref_act.satDb = &crapdb;
	mdp_rxAnt1.ref_act.satDgen = &satDgen_rx1;
	mdp_rxAnt1.ref_act.carrgen = &carrgen_rx1;
	mdp_rxAnt1.ref_act.codegen = &codegen_rx1;
	mdp_rxAnt1.ref_act.channel = &channel;
	mdp_rxAnt1.GetActiveParam();

	
	/* RXDE throw the GPS True time i.e T_rx - rx error*/

	


	mdp_rxAnt1.InputInfToEngine(EngineFramework::ACTIVE);
	mdp_rxAnt1.Engine(SatMDE::RUN_METHOD);

	
	/* 2nd Receiver*/
	/*	rx2_clock.activeData.trueGPStime = crap.trueGPStime;
	rx2_clock.Engine(RxDE::RUN_METHOD );
	rx2_clock.EngineOutput(RxDE::DATA_HERE);*/

	mdp_rxAnt2.ref_act.measTime =rx1_clock.rxDE_oData.rx_time;
	mdp_rxAnt2.ref_act.rxXvt = rx_ant2;// same position differnt clock
	mdp_rxAnt2.ref_act.satDb = &crapdb;
	mdp_rxAnt2.ref_act.satDgen = &satDgen_rx2;
	mdp_rxAnt2.ref_act.carrgen = &carrgen_rx2;
	mdp_rxAnt2.ref_act.codegen= &codegen_rx2;
	mdp_rxAnt2.ref_act.channel = &channel;
	mdp_rxAnt2.GetActiveParam();

	
	/* RXDE throw the GPS True time i.e T_rx - rx error*/

	




	mdp_rxAnt2.InputInfToEngine(EngineFramework::ACTIVE);
	mdp_rxAnt2.Engine(SatMDE::RUN_METHOD);
	
	

	cout<<rx1_clock.rxDE_oData.rx_time.printf("%Y %03j % 12.12s") << endl;
	

	if(!f1)
	{   using namespace StringUtils;
		f1=true;
		SatID satid = mdp_rxAnt1.visibleSV[1];
		mdp_rxAnt1.outputInfFromEngine (s1,satid,0);
		satid = mdp_rxAnt2.visibleSV[1];
		mdp_rxAnt2.outputInfFromEngine (s1,satid,0);
		mdp_rxAnt2.outputInfFromEngine (s1,satid,3); // just adds a new line


		satid = mdp_rxAnt1.visibleSV[2];
		mdp_rxAnt1.outputInfFromEngine (s2,satid,0);
		satid = mdp_rxAnt2.visibleSV[2];
		mdp_rxAnt2.outputInfFromEngine (s2,satid,0); //
		mdp_rxAnt2.outputInfFromEngine (s2,satid,3); // just adds a new line

		satid = mdp_rxAnt1.visibleSV[3];
		mdp_rxAnt1.outputInfFromEngine (s3,satid,0);
		satid = mdp_rxAnt2.visibleSV[3];
		mdp_rxAnt2.outputInfFromEngine (s3,satid,0); //
		mdp_rxAnt2.outputInfFromEngine (s3,satid,3); // just adds a new line

		satid = mdp_rxAnt1.visibleSV[3];
		mdp_rxAnt1.outputInfFromEngine (s3,satid,0);
		satid = mdp_rxAnt2.visibleSV[3];
		mdp_rxAnt2.outputInfFromEngine (s3,satid,0); //
		mdp_rxAnt2.outputInfFromEngine (s3,satid,3); // just adds a new line


		el +=rx1_clock.rxDE_oData.opDataHeader;
		el += leftJustify("\n",3);
		s5<<el;
	}
	el = "";

	
	using namespace StringUtils;;
	el += rx1_clock.rxDE_oData.opDataInString;
	el += rx2_clock.rxDE_oData.opDataInString;
	el += leftJustify("\n",3);
	s5<<el;


	SatID satid = mdp_rxAnt1.visibleSV[1];
	mdp_rxAnt1.outputInfFromEngine (s1,satid,1);
	satid = mdp_rxAnt2.visibleSV[1];
	mdp_rxAnt2.outputInfFromEngine (s1,satid,1);
    mdp_rxAnt2.outputInfFromEngine (s1,satid,3); // just adds a new line



   satid = mdp_rxAnt1.visibleSV[2];
	mdp_rxAnt1.outputInfFromEngine (s2,satid,1);
	satid = mdp_rxAnt2.visibleSV[2];
	mdp_rxAnt2.outputInfFromEngine (s2,satid,1);
    mdp_rxAnt2.outputInfFromEngine (s2,satid,3); // just adds a new line


	satid = mdp_rxAnt1.visibleSV[3];
	mdp_rxAnt1.outputInfFromEngine (s3,satid,1);
	satid = mdp_rxAnt2.visibleSV[3];
	mdp_rxAnt2.outputInfFromEngine (s3,satid,1);
    mdp_rxAnt2.outputInfFromEngine (s3,satid,3); // just adds a new line

	satid = mdp_rxAnt1.visibleSV[4];
	mdp_rxAnt1.outputInfFromEngine (s4,satid,1);
	satid = mdp_rxAnt2.visibleSV[4];
	mdp_rxAnt2.outputInfFromEngine (s4,satid,1);
    mdp_rxAnt2.outputInfFromEngine (s4,satid,3); // just adds a new line


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

