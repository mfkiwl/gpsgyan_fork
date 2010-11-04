/* * File name		 : satMDE.hpp
 *
 * Abstract          : This file contains definitions of structures and Global classes
 *                     Struchers  used in the Gpsgyan project.

 *
 * Compiler          : Visual C++ Version 9.0
 *
 * Compiler options  : Default
 *
 * Pragmas           : None used
 *
 * H/W Platform      : IBM PC
 *
 * Portability       : No Operating system call used.
 *                     No machine specific instruction used.
 *                     No compiler specific routines used.
 *                     Hence Portable.
 *
 * Authors			: Priyank Kumar
 *
 *
 * Version history	: Base version
 *	                   Priyank Kumar     November 2009
 *
 * References        : None
 *
 * Functions called  : None
 * Library Used 	 : GPStk
 *  				   Copyright 2009, The University of Texas at Austin
 *
 */
/* GPSTk Library*/

#include "DayTime.hpp"
#include "SatID.hpp"
#include "Position.hpp"
#include "SatMdeMac.hpp"
#include "SatMedtypes.hpp"
//#include "MSCData.hpp"
//#include "MSCStream.hpp"
///#include "FFIdentifier.hpp"
//#include "TropModel.hpp"
//#include "IonoModel.hpp"
#include "Geodetic.hpp"
//#include "SunPosition.hpp"
#include "WGS84Geoid.hpp"



/*C++ Systems Include files*/
#include <iostream>
#include <string>
#include <map>
#include <vector>

/*GPSGyan Include files*/
#include "EngineFramework.hpp"



using namespace gpstk;

class SatMDE: public EngineFramework
{
public:


	 NEW_EXCEPTION_CLASS(ExcSatMDE , gpstk::Exception);


	SatMDE(const string& applName) throw()
   : EngineFramework(
      applName,
      "this computes satellite Position , Velocity and time for the specified receiever position and time")
   {};

	 // bool initialize(int argc, char *argv[]) throw();
	 // virtual void process();

void GetPassiveInput(satMDE_psvIstruct &ref_psv);
void GetActiveInput(satMDE_actIstruct &ref_act);
void GetPassiveParam();
void GetActiveParam();


void InputInfToEngine(Kind kind);  // think of argv and argc implementation
//virtual void runEngine();
void InputParamToEngine(Kind kind);// throw(InvalidParameter);
//bool runEngine() throw();
//void outputInfFromEngine(std::ostream & s ,const std::string & 	data);
void outputInfFromEngine(std::ostream & s );
//void OutputDumpOfEngine();

// Public Data memebers
engineHealth satMDE_probe;

std::vector<satMDE_ostruct> satMDE_oData;
std::vector<SatID> visibleSV;
bool visiblityComputed;

satMDE_actIstruct ref_act;
satMDE_psvIstruct ref_psv;
double alpha[4],beta[4],iono_freq;


DayTime inp_time ;
double usr_elev;
std::vector<int> error_number;

map<SatID, double> pseudoRange;
//std::string op_inf;
protected :
			void EngineControl(Kind kind);
			void SetInitialStateofEngine();
			void SetActiveStateofEngine ();

            void VerifyIpToEngine() throw(ExcSatMDE);
 //         void prepareIpToEngine() ;

			void MethodOfEngine() throw(ExcSatMDE);

			void SatMDE_ExpHandler(ExcSatMDE &e);

			// Fromatted File Printing
			std::string print_content(satMDE_ostruct &ostruct);
		    std::string print_header();
			void updateLocals(const Xvt& satXvt ,const Xvt& rxXvt);
			void ComputePhaseAndDoppler(const double &rangeRate,const double &clkCorrection);
			//double getTropoCorrections( TropModel *pTropModel, double elevation );
			//void ComputeSvVisiblity();
			int findSvPrvRange(SatID vis_svid);
		         // void validateOpOfEngine();
		//	void PrepareOpOfEngine()	;

			//virtual void dump(std::ostream &s);




private :
satMDE_actIstruct satMDE_ai;
satMDE_psvIstruct satMDE_pi;
unsigned int visSV;
std::vector<satMDE_ostruct> satMDE_pData;
std::vector<SatID> rejectedSV;

std::vector<satMDE_range> prevRange,prange;

bool timeToCompute;
bool newSV;
bool isThisFirstTime;
int visCounter;
int totalSat;
WGS84Geoid geoid;

Xvt currentRxXvt;

double rawrange;
double elevation;
double azimuth;
double elevationGeodetic;
double azimuthGeodetic;
Triple cosines;

double ionoL1,carrierDopplerL1,codeDopplerCaL1;
double ionoL2,carrierDopplerL2,codeDopplerCaL2;
double ionoL5,carrierDopplerL5,codeDopplerPyL5;

	// Error Value

	double minElev;
	Position tempPos;
   map<SatID, phaseData> phase_station;
      map<SatID, phaseData> phase_satellite;

	  //int IIR_Sat[12] ;
	  bool IIR;
	  //double phaseWindUp;

void getMeasurement(SatID &temp_satid,
							 Position &rxXvt,
							 DayTime  &measTime,
							 SatDb    *satDb,
							 SatDgen *satDgen,
							 double &range,
							 bool firstMeas,
							 bool svRange);
};


