/* * File name		 : RxDE.hpp
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
#include "YumaAlmanacStore.hpp"
#include "AlmOrbit.hpp"
#include "DayTime.hpp"
#include "SatID.hpp"
#include "EphReader.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"
#include "FFIdentifier.hpp"
#include "Position.hpp"
#include "RxDEMac.hpp"
#include "RxDEtypes.hpp"


/*C++ Systems Include files*/
#include <iostream>
#include <string>
#include <map>
#include <vector>

/*GPSGyan Include files*/
#include "EngineFramework.hpp"



using namespace gpstk;

class RxDE: public EngineFramework
{
public:	


	 NEW_EXCEPTION_CLASS(ExcRxDE , gpstk::Exception);


	RxDE(const string& applName) throw()
   : EngineFramework(
      applName,
      "this computes satellite Position , Velocity and time for the specified receiever position and time")
   {};

	 // bool initialize(int argc, char *argv[]) throw();
	 // virtual void process();
	 
void GetPassiveInput(rxDE_psvIstruct &ref_psv);
void GetActiveInput(rxDE_actIstruct &ref_act);
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
engineHealthMonitor RxDE_probe;

RxDE_ostruct RxDE_oData;

rxDE_actIstruct ref_act;
rxDE_psvIstruct ref_psv;


DayTime inp_time ;
 
double usr_elev;
std::vector<int> error_number;
//std::string op_inf;
protected : 
			//void  EngineControl(Kind kind);
            void VerifyIpToEngine() throw(ExcRxDE);
 //         void prepareIpToEngine() ;

			void MethodOfEngine() throw(ExcRxDE);
			void getSatParam(SatID sat, DayTime& t ,const YumaAlmanacStore& yums);
			void RxDE_ExpHandler(ExcRxDE &e);
			double GenerateRxClkErr(double& update_interval);

			// Fromatted File Printing 
			std::string print_content(RxDE_ostruct &ostruct);
		    std::string print_header();


         // void validateOpOfEngine();
		//	void PrepareOpOfEngine()	;

			//virtual void dump(std::ostream &s);




private :

rxDE_actIstruct rxDE_ai;
rxDE_psvIstruct rxDE_pi;

double rxClkerr_time[M_TWO] ,rxClkerr_phase[M_TWO] ;
double norm1,norm2;

//DayTime  trueGPStime;
	SatID    gps_sv;
RxDE_ostruct RxDE_pData;
	
	// Error Value

	double minElev;

};


