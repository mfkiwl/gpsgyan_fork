 #ifndef GPSTK_ENGINEFRAMEWORK_HPP
 #define GPSTK_ENGINEFRAMEWORK_HPP


 //============================================================================
 /* * File name		 : EngineFramework.hpp
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
  * Library Used 	 : GPStk ,Copyright 2009, The University of Texas at Austin
  */
 //============================================================================

 //#include "CommandOptionParser.hpp"


 namespace gpstk
 {


    class EngineFramework
    {
    public:
	enum Mode { INITIALIZE =0,
				RUN_ALL =1,
				RUN_METHOD=2,
				RUN_WITH_VnV=3, 
				RUN_FROM_START =4};
    enum Kind{ PASSIVE =0 ,
			ACTIVE =1};
			

	typedef struct
	{unsigned h_runAll		:1;
	 unsigned h_runMethod	:1;
     unsigned h_runWithVnV	:1;
	 unsigned h_runFromStart:1;
	 unsigned s_engineInitState:1;
	 unsigned s_engine :2;
	 unsigned h_ipEngine :1;
	 unsigned h_prepEngine :1;
	 unsigned h_verEngine :1;
	 unsigned h_moduleEngine:1;
	 unsigned h_valEngine:1;
	 unsigned h_opEngine:1;


	 unsigned :19;
	}engineHealthMonitor ;


			
			
// Default Constructor   
			Mode mode;
			Kind kind;
EngineFramework( const std::string& applName,
                 const std::string& applDesc )
                 throw();
// Default Destructor  
       virtual ~EngineFramework() {};

// Input information stream
   //virtual void inputInfToEngine(){};
   virtual void InputParamToEngine(){};
 
// Engine Running Modes Model

	bool Engine(Mode mode) throw();


// Output Processed stream
	virtual void OutputInfFromEngine();
   	virtual void OutputDumpOfEngine(){};

/***********************************/
    protected:

       int debugLevel;
       int verboseLevel;
       std::string argv0;
       std::string appDesc;


       //CommandOptionNoArg debugOption;
       //CommandOptionNoArg verboseOption;
       //CommandOptionNoArg helpOption;

	   virtual void EngineControl(Kind kind){} ;

       virtual void VerifyIpToEngine() {};
       virtual void PrepareIpToEngine() {};

	   

       virtual void MethodOfEngine() {};

       virtual void ValidateOpOfEngine() {};
       virtual void PrepareOpOfEngine() {};


      	virtual void ShutDownEngine() {};

      	virtual void CompleteProcessing();






    private:


          // Do not allow the use of the default constructor.
       EngineFramework();

    }; // End of class 'EngineFramework'


 }  // End of namespace gpstk
 #endif   // GPSTK_ENGINEFRAMEWORK_HPP
