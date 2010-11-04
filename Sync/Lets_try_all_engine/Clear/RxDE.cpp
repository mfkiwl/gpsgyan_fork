#pragma ident "$Id$"
//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================
//
// Compute number of stations visible to a set of space vehicles (SV) 
// over a requested period (default 23:56).  Accept FIC 
// almanac format, FIC ephemeris, Rinex nav, Yuma almanac, SEM almanac, 
// or SP3 as input
//
// Assumptions:
//
// System
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>

// gpstk
#include "StringUtils.hpp"
#include "GPSAlmanacStore.hpp"
#include "icd_200_constants.hpp"
#include "gps_constants.hpp"
#include "RxDE.hpp"
#include "PreciseRange.hpp"
#include <ctime>            // std::time

#include <boost/random/linear_congruential.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/additive_combine.hpp>
#include <boost/random.hpp>
#if !defined(__SUNPRO_CC) || (__SUNPRO_CC > 0x530)
#include <boost/generator_iterator.hpp>
#endif


using namespace gpstk;
using namespace std;




void RxDE::InputInfToEngine(Kind kind )
{
 switch(kind)
 {
 case PASSIVE : GetPassiveInput(ref_psv);
 break;
 case ACTIVE : GetActiveInput(ref_act);
 break;
 default: break; // Set the flag as null signifying no input
 }
 

}



void RxDE::GetPassiveInput(rxDE_psvIstruct &ref_psv)
{
rxDE_pi.rx_Rxyz = ref_psv.rx_Rxyz; 
rxDE_pi.rx_Vxyz = ref_psv.rx_Vxyz;
rxDE_pi.rx_Axyz = ref_psv.rx_Axyz; 
rxDE_pi.rx_Jxyz = ref_psv.rx_Jxyz; 
rxDE_pi.rx_llh = ref_psv.rx_llh;
Position temp = rxDE_pi.rx_llh.transformTo(Position::Cartesian);
rxDE_pi.rx_Rxyz[0]=temp.X();
rxDE_pi.rx_Rxyz[1]=temp.Y();
rxDE_pi.rx_Rxyz[2]=temp.Z();
rxDE_pi.rx_Vxyz[0]=(double)M_ZERO;
rxDE_pi.rx_Vxyz[1]=(double)M_ZERO;
rxDE_pi.rx_Vxyz[2]=(double)M_ZERO;

}

   
void RxDE::GetActiveInput(rxDE_actIstruct &ref_act)
{

rxDE_ai.rx_time = ref_act.rx_time;
}


void RxDE::InputParamToEngine(Kind kind)
{
switch(kind)
 {
 case PASSIVE : GetPassiveParam();
 break;
 case ACTIVE : GetActiveParam();
 break;
 default: break; // Set the flag as null signifying no input
 }
}
 

void RxDE::GetPassiveParam()
{
rxClkerr_time[0] = 0;
rxClkerr_time[1] = 0;
rxClkerr_phase[0] = 0;
rxClkerr_phase[1]= 0; 
//model_sat_clock
}

void RxDE::GetActiveParam()
{
	int i=0;
}



void RxDE::VerifyIpToEngine() throw(ExcRxDE)
{
	#if 0
	ExcRxDE e("Verification" , 0 ,gpstk::Exception::recoverable);
	e.setErrorId(1);
	try
	{
	
	/* Verify  the Input Elevation */
	if(minElev < MIN_ELV_ANGLE) 
	{
		e.addText("Elevation Underflow ");
		e.addLocation(FILE_LOCATION);
		error_number.push_back(1);
	}

	if (yumaReader.eph == NULL)
   {
      e.addText("Didn't get any Orbital data from the Yuma files. ");
	  e.addLocation(FILE_LOCATION);
	  error_number.push_back(2);
	     throw e;
	}
 

	}
	catch(ExcRxDE &e)
	{
			RxDE_ExpHandler(e);
	}
	 #endif
   }
   
	

void RxDE::MethodOfEngine()
{
  double update_interval =1;
  double rx_correction =GenerateRxClkErr(update_interval);
  RxDE_pData.trueGPStime = rxDE_ai.rx_time - rx_correction;
  RxDE_pData.rxXvt.x = rxDE_pi.rx_Rxyz;
  RxDE_pData.rxXvt.v = rxDE_pi.rx_Vxyz;
  RxDE_pData.rxXvt.dtime=rx_correction;
  RxDE_pData.rxXvt.ddtime=rxClkerr_phase[M_ONE];
}

double RxDE::GenerateRxClkErr(double& update_interval)
     
    {
		
	// This is a typedef for a random number generator.
	// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
	// Define the generator type
	double dt = update_interval;
	typedef boost::mt19937 base_generator_type1;
	typedef boost::mt11213b base_generator_type2;
	// Using 42u generator typs
	base_generator_type1 generator1(42u);
	base_generator_type2 generator2(48u);
	// Take the seed from the PC clock
	generator1.seed(static_cast<unsigned int>(std::time(0)));
	
	boost::normal_distribution<>  normal(M_ZERO,M_ONE);
	
	// Generate two iid  Normal Random number generator with mean = 0 , variance =1; 
    boost::variate_generator<base_generator_type1&, boost::normal_distribution<> > norm_gen1(generator1, normal);

	generator2.seed(static_cast<unsigned int>(std::time(0)));

	boost::variate_generator<base_generator_type2&, boost::normal_distribution<> > norm_gen2(generator2, normal);
	
	// Define the two integrater for discrete gaussian noise process = Weiner Process
	norm1 = norm_gen1();
	norm2 = norm_gen2();
		

	double gaussian_walk1 = std::sqrt(CLK_SF*dt + CLK_SGB_12*pow(dt,(double)3))*norm1 + std::sqrt( CLK_SGB_4*pow(dt,(double)3))*norm2;
	double gaussian_walk2 = std::sqrt( CLK_SG*dt)*norm2;
	
    rxClkerr_time[M_ONE] = rxClkerr_time[M_ZERO] + dt*rxClkerr_phase[M_ZERO] + gaussian_walk1;
    rxClkerr_phase[M_ONE] = rxClkerr_phase[M_ZERO] + gaussian_walk2;


   
   double ret_rxClk_err = rxClkerr_time[M_ZERO];
   // Update the stochastic Model for next tick 
  
   
   rxClkerr_time[M_ZERO]	=rxClkerr_time[M_ONE];
   rxClkerr_phase[M_ZERO]	=rxClkerr_phase[M_ONE];
   return(ret_rxClk_err);
   
 }
    
  

void RxDE::outputInfFromEngine(std::ostream & 	s )
     
    {
	string  el;
	el = ""; 
	using namespace StringUtils;

	RxDE_oData = RxDE_pData;
	/*el = print_header();
	el += leftJustify("\n",3);
	s<<el;
	*/
		using std::endl;
		using std::setw;
       s << std::setprecision(4);    // set the precision of probe output
	  s << print_content(RxDE_oData);
	
	/*el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;
	s << rxDE_ai.rx_time.printf("%Y %03j % 12.10s")<<endl;
	el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;
	*/
	
      
    }
   
      
void RxDE::RxDE_ExpHandler(ExcRxDE &e)
{
unsigned long eid = error_number[1];

#if 0
	switch(eid)
	{
	case 2: minElev = MIN_ELV_ANGLE;
		break;
	case 1: e.terminate();
		break;
	case 0: cout<<"over";
		break;
	}
#endif
}


std::string RxDE::print_content(RxDE_ostruct &ostruct)

{
 string  content;
 content = ""; 
 using namespace StringUtils;
	           content += leftJustify(asString((double)norm1), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)norm2), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)rxClkerr_time[M_ZERO]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)rxClkerr_phase[M_ZERO]), 13);
			   content += leftJustify("\n",3);
			  // content += leftJustify(asString(e.sattime),13);
			  // content += leftJustify("\n",3);
			   return(content);
}

std::string RxDE::print_header()
{
 string  header;
 header = ""; 
 using namespace StringUtils;

 header +=  leftJustify("Sys PRN", 6);
			   header += leftJustify("\t",3);
			   header += leftJustify("AWGN PROCESS 1 ", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("AWGN PROCESS 2", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("Receiver Time corr", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("Receiver Phase cor", 13);
			   header += leftJustify("\n",3);
			  // header += leftJustify(asString(e.sattime),13);
			  // header += leftJustify("\n",3);
			   return(header);
}
           
