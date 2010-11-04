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
#include "SatDb.hpp"
#include "PreciseRange.hpp"
#include "GaussianDistribution.hpp"


using namespace gpstk;
using namespace std;




void SatDb::InputInfToEngine(Kind kind)
{
 switch(kind)
 {
 case PASSIVE : GetPassiveInput();
 break;
 case ACTIVE : GetActiveInput();
 break;
 default: break; // Set the flag as null signifying no input
 }
 

}



void SatDb::GetPassiveInput()
{

minElev = usr_elev;
yumaReader.read(yumaFileName);
}

   
void SatDb::GetActiveInput()
{

satTime = inp_time;
}


void SatDb::InputParamToEngine(Kind kind)
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
 

void SatDb::GetPassiveParam()
{

//satDb_probe = 0;
//model_sat_clock
}

void SatDb::GetActiveParam()
{
	int i=0;
}



void SatDb::VerifyIpToEngine() throw(ExcSatDb)
{
	ExcSatDb e("Verification" , 0 ,gpstk::Exception::recoverable);
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
	catch(ExcSatDb &e)
	{
			SatDb_ExpHandler(e);
	}
	  
   }
   
	

void SatDb::MethodOfEngine()
{
   gpstk::XvtStore<SatID>& almStore = *yumaReader.eph;
   /*if (typeid(yumaReader.eph) == typeid(YumaAlmanacStore))
      {*/
  const YumaAlmanacStore& yums = dynamic_cast<const YumaAlmanacStore&>(*yumaReader.eph);
  DayTime t = satTime;
// Clear the private data container   
// Let's make sure everything is clean
  satDb_pData.resize(0);
	if(!visiblityFlag)
	{

      for (int prn=1; prn <= MAX_PRN; prn++)
      {        
		 getSatParam(SatID(prn, SatID::systemGPS), t , yums);
	
	  }
	}
	else
	{
		int size = vislist_ref->size();
		for (int prn=0; prn < size; prn++)
      {        
		 SatID satid = vislist_ref->at(prn);

		  getSatParam(satid, t , yums);

		}
	
	  }
	

	}

 



void SatDb::getSatParam(SatID sat, DayTime& gpstime ,const YumaAlmanacStore& yums)
     
    {

	 satDb_ostruct temp_sat;
	 Position satR,satV; // can be reduced to ECEF or Triple
	 try
	 {
		
     AlmOrbit a = yums.findAlmanac(sat, gpstime);
	 sv_xvt     = a.svXvt(gpstime);
	 short week = a.getFullWeek();
	 // Make the output data structure
	 DayTime toa = a.getToaTime();
	 double time_elap = gpstime - toa;
	 //double temp_elap = (sv_xvt.dtime - a.getAF0())/a.getAF1(); // debug
	 temp_sat.prn=sat;
	 temp_sat.sathealth=!(a.getSVHealth());
	 temp_sat.satXvt=sv_xvt;
	 satR.setECEF(sv_xvt.x[M_ZERO],sv_xvt.x[M_ONE],sv_xvt.x[M_TWO]);
	 satV.setECEF(sv_xvt.v[M_ZERO],sv_xvt.v[M_ONE],sv_xvt.v[M_TWO]);

    // Relativity Correction
	// Correction = -2*dot(R,V)/(c*c) ,
	// R & V are position and velocity vector 

	 temp_sat.relativity = -2*(satR.X()/C_GPS_M)*(satV.X()/C_GPS_M)
             -2*(satR.Y()/C_GPS_M)*(satV.Y()/C_GPS_M)
             -2*(satR.Z()/C_GPS_M)*(satV.Z()/C_GPS_M);
  
     temp_sat.satclkbias=sv_xvt.dtime*C_GPS_M;
	 
	 // This take care of second roll over and other time adjustment
	 // as addition operator is overloaded with "addLongDeltaTime" function
	 // of gpstk::DayTime class.
	// correction = af0+af1(gpstime - toc of almanac) + relativity 
	 double correction = sv_xvt.dtime + temp_sat.relativity; // correction with relativity , as in Yuma file it doesnt come
	 temp_sat.sattime = gpstime + correction;
	 temp_sat.clkCorrection = correction;//a.getAF1()*C_GPS_M;
	
 // Take care of af0 and af1 of the clock and generate all frequency
	// sat correction is calculated in Satdb engine
	// Orginal Sat clock freq is offset for relativistic correction
	 temp_sat.satOscFreq 	   	= 	RSVCLK + RSVCLK * correction;
	 temp_sat.carrFreqL1 		=	temp_sat.satOscFreq*L1_MULT;
	 temp_sat.carrFreqL2 		=	temp_sat.satOscFreq*L2_MULT;
	 temp_sat.carrFreqL5 		=	temp_sat.satOscFreq*115;
	 temp_sat.chipFreqCA		=   temp_sat.satOscFreq / 10;
	 temp_sat.chipFreqPY		=   temp_sat.satOscFreq;
	 
	 
	 
	 
	 
	 // Almorbit satpos computation doesnt add relativity and ddtime param is empty, using it to do relativistic correction 
	 temp_sat.satXvt.ddtime = correction; // af0+af1(gpstime - toc of almanac) + relativity 
	 satDb_pData.push_back(temp_sat);
}
	  catch(InvalidRequest &e)
	 {
		 temp_sat.prn=sat;
		 temp_sat.sathealth=false;
		 temp_sat.satXvt.x[M_ZERO] = (double)M_ZERO;
		 temp_sat.satXvt.x[M_ONE] = (double)M_ZERO;
		 temp_sat.satXvt.x[M_TWO] = (double)M_ZERO;
	     temp_sat.satXvt.v[M_ZERO] = (double)M_ZERO;
		 temp_sat.satXvt.v[M_ONE] = (double)M_ZERO;
		 temp_sat.satXvt.v[M_TWO] = (double)M_ZERO;
		  temp_sat.satclkbias=M_ZERO;
		  temp_sat.relativity=M_ZERO;
		  temp_sat.clkCorrection=(double)M_ZERO;
		  satDb_pData.push_back(temp_sat);
		  //temp_sat.sattime=0;
	  }


 }
    
  

void SatDb::outputInfFromEngine(std::ostream & 	s )
     
    {
	string  el;
	el = ""; 
	using namespace StringUtils;

	satDb_oData = satDb_pData;

	vector<satDb_ostruct>::iterator i;

	// Take only the healthy satellite
	for (i=satDb_oData.begin(); i!=satDb_oData.end();)
     {    
		if(!(*i).sathealth)	
		{  
			satDb_oData.erase(i);
		} else i++;
		//if(satDb_oData.empty()) break;
	}
		
	
	el = print_header();
	el += leftJustify("\n",3);
	s<<el;
	
	for (int prn=0; prn < satDb_oData.size(); prn++)
      {    
		
		using std::endl;
		using std::setw;
	   s << std::setprecision(4);    // set the precision of probe output
	   s << print_content(satDb_oData[prn]);
		}
		
	el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;
	s << satTime.printf("%Y %03j % 12.6s")<<endl;
	el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;
	  
    }
   
      
void SatDb::SatDb_ExpHandler(ExcSatDb &e)
{
unsigned long eid = error_number[1];


	switch(eid)
	{
	case 2: minElev = MIN_ELV_ANGLE;
		break;
	case 1: e.terminate();
		break;
	case 0: cout<<"over";
		break;
	}

}


std::string SatDb::print_content(satDb_ostruct &ostruct)

{
 string  content;
 content = ""; 
 using namespace StringUtils;
	           content +=  leftJustify(asString(ostruct.prn), 6);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.x[M_ZERO]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.x[M_ONE]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.x[M_TWO]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.v[M_ZERO]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.v[M_ONE]), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.satXvt.v[M_TWO]), 13);
			   content += leftJustify("\t",3);
               content += leftJustify(asString((double)ostruct.clkCorrection),13);
			   content += leftJustify("\n",3);
			  // content += leftJustify(asString(e.sattime),13);
			  // content += leftJustify("\n",3);
			   return(content);
}

std::string SatDb::print_header()
{
 string  header;
 header = ""; 
 using namespace StringUtils;

 header +=  leftJustify("Sys PRN", 6);
			   header += leftJustify("\t",3);
			   header += leftJustify("SatPos X", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("SatPos Y", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("SatPos Z", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("SatVel X", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("SatVel Y", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("Satvel Z", 13);
			   header += leftJustify("\t",3);
               header += leftJustify("Clock Correction",13);
			   header += leftJustify("\n",3);
			  // header += leftJustify(asString(e.sattime),13);
			  // header += leftJustify("\n",3);
			   return(header);
}
           
