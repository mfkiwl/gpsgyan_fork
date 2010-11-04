/* * File name		 : SimTimeEngine.hpp
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


#include "DayTime.hpp"
#include "EngineFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"
#include <string>




using namespace gpstk;

class SimTimeEngine: public EngineFramework
{
public:
	SimTimeEngine(const string& applName) throw()
   : EngineFramework(
      applName,
      "this computes satellite Position , Velocity and time for the specified receiever position and time")
   {};

	  //bool initialize(int argc, char *argv[]) throw();
	//template <class X>
void inputInfToEngine(DayTime &time); //throw(); // think of argv and argc implementation
//virtual void runEngine();
void inputParamToEngine();
//bool runEngine() throw();
//void outputInfFromEngine();
void outputDumpOfEngine();

protected :


          void verifyIpToEngine()  ;
          void prepareIpToEngine() ;

          void methodOfEngine()    ;

         // void validateOpOfEngine();
          void prepareOpOfEngine()	;

         virtual void dump(std::ostream &s);



private :
    DayTime simStartTime, simEndTime,simTimebase;
	bool simtimeIsSystemTime, startSimTime ,stopSimTime,pauseSimTime,isThisFirstTick,isThisFirstTimerTick;
	bool simtimeIsTimer,simTimerExpired,simTimerReset;
//	static int timeCount=100;
};


