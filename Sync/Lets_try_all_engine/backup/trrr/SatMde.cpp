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
#include "SatMde.hpp"
#include "WGS84Geoid.hpp"
#include "PreciseRange.hpp"


using namespace gpstk;
using namespace std;




void SatMDE::InputInfToEngine(Kind kind )
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



void SatMDE::GetPassiveInput(satMDE_psvIstruct &ref_psv)
{
satMDE_pi = ref_psv;
visSV = 0;

}


void SatMDE::GetActiveInput(satMDE_actIstruct &ref_act)
{

satMDE_ai = ref_act;


}



void SatMDE::InputParamToEngine(Kind kind)
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


void SatMDE::GetPassiveParam()
{




}

void SatMDE::GetActiveParam()
{


}



void SatMDE:: EngineControl(Kind kind)

{
	switch(kind)
	{ case PASSIVE :

	SetInitialStateofEngine();
	  // Initialize GPS true time to system time then increment the second based on timer expiration

	  break;

	case ACTIVE :
	 SetActiveStateofEngine();

		break;
	default :
		//isThisFirstTick = false;
		break;

	}
	// set debug level etc
}




void SatMDE::SetInitialStateofEngine()
{
	// Clear and set initial state variable;
	alpha[0] = 0.010e-6;
	alpha[1] = 0.000e-6;
	alpha[2] = -0.060e-6;
	alpha[3] = 0.000e-6;
	beta[0] =90e3;
	beta[1] =0e3;
	beta[2] =-197e3;
	beta[3] =0e3;
	visSV= M_ZERO;
	timeToCompute = true;
	visiblityComputed = false;
	visCounter = M_ZERO;
	newSV = true;
	isThisFirstTime = false;

/* Clear or initialize anydata which is feed back to other Engine in their active state*/

/* Since initial Estimate of transit time is fed back to SatDb engine which is called earlier*/
	for(int i=1; i <= gpstk::MAX_PRN ; i++)
	{
		SatID temp_satid(i,gpstk::SatID::systemGPS);
		pseudoRange[temp_satid] = M_ZERO;
	}



}


void SatMDE::SetActiveStateofEngine()
{
	// clear all the old value of the private data structure;
    	satMDE_pData.clear();
    	prange.resize(0); // clear the prange
		//pseudoRange.clear();
   	    visSV = M_ZERO;  // reset the visible sv = 0 for visblity computation

   // set the global var for current active inputs that would be used by Engine Algo
   	  totalSat = satMDE_ai.satDb_ref->size();
	  currentRxXvt = satMDE_ai.rxXvt;


// Take care of any counter and flag that are dependedent on Active input
// or Engine independent counters and flag

 if(timeToCompute) // Time to Compute Visiblity

{

rejectedSV.resize(0);
visibleSV.resize(0);
}
else
{	visCounter++;


}




}




void SatMDE::VerifyIpToEngine() throw(ExcSatMDE)
{
#if 0
	ExcSatMDE e("Verification" , 0 ,gpstk::Exception::recoverable);
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
	catch(ExcSatMDE &e)
	{
			SatMDE_ExpHandler(e);
	}
	 #endif
   }


void SatMDE::MethodOfEngine()
{
satMDE_range prevsecond,current;
std::vector<satMDE_range> vTemprange;
satMDE_ostruct temp_mdpdata;
DayTime measTime = satMDE_ai.measTime;
	int i;

double tempPR, tempIono , tempTrop ,tempRangeRate;




if(timeToCompute)

{
	timeToCompute = false;

for(int i=1; i <= gpstk::MAX_PRN ; i++)
	{
	SatID satid(i,gpstk::SatID::systemGPS);
	getMeasurement(satid,satMDE_ai.rxXvt,measTime,satMDE_ai.satDb,satMDE_ai.satDgen,0,true,false);


	//	Xvt satXvt= satMDE_ai.satDb_ref->at(i).satXvt ;
	//SatID satid = satMDE_ai.satDb_ref->at(i).prn ;
	//updateLocals(satXvt,currentRxXvt);

/* Routine only when visiblity is computed*/
	if(satMDE_ai.satDgen->satDgen_oData.elevation < satMDE_pi.minElev)
	{
	rejectedSV.push_back(satid);
	pseudoRange[satid]=M_ZERO;
	continue;
	}
	else
	{
	visSV ++;
	visibleSV.push_back(satid);
	visiblityComputed = true;

	// Find Ranges when complete set is visible

	current.prn=satid;
	current.range = satMDE_ai.satDgen->satDgen_oData.pseudoRange;
	prange.push_back(current);

	pseudoRange[satid]=current.range;

	int loc = findSvPrvRange(satid); // this gives the location of previous range of the if SV was visblible in previous run
	if(newSV)
	{
	prevsecond.range = (double)M_ZERO;
	temp_mdpdata.measurementHealth = false;
	}

	else
	prevsecond.range = prevRange.at(loc).range;
	prevsecond.prn = satid;
	tempRangeRate = current.range - prevsecond.range;
	prevsecond.range =current.range ; // update the SV range to current range

	/* visiblity routine Ends*/

	ComputePhaseAndDoppler(tempRangeRate,satMDE_ai.satDb->satoData.clkCorrection);
	temp_mdpdata.prn =satid;
	temp_mdpdata.dataNmeas = satMDE_ai.satDgen->satDgen_oData;
	
	temp_mdpdata.carrierDopplerL1 =carrierDopplerL1;
	temp_mdpdata.codeDopplerCaL1 =codeDopplerCaL1;
	temp_mdpdata.carrierDopplerL2 =carrierDopplerL2;
	temp_mdpdata.codeDopplerCaL2 =codeDopplerCaL2;
	temp_mdpdata.carrierDopplerL5 =carrierDopplerL5;
	temp_mdpdata.codeDopplerPyL5 =codeDopplerPyL5;
	temp_mdpdata.rangeRate =tempRangeRate;

	vTemprange.push_back(prevsecond);
	satMDE_pData.push_back(temp_mdpdata);
	}

	}
	prevRange.resize(0);
	prevRange = vTemprange;
}

else
	{
		totalSat = visibleSV->size();
	
		for (int i=0; i<totalSat; i++)
	{
	Xvt satXvt= satMDE_ai.satDb_ref->at(i).satXvt ;
	SatID satid = visibleSV.at(i) ;

     getMeasurement(satid,satMDE_ai.rxXvt,measTime,satMDE_ai.satDb,satMDE_ai.satDgen,0,true,false);
	// Compute range with only visible sat
	current.prn=satid;
	//PseudoRange Calculation
	current.range = satMDE_ai.satDgen->satDgen_oData.pseudoRange;
	prange.push_back(current);

	pseudoRange[satid]=current.range;

	//RangeRate Calculation
	tempRangeRate = current.range - prevRange.at(i).range;
	//Doppler, IONO, Phase
	ComputePhaseAndDoppler(tempRangeRate,satMDE_ai.satDb->satoData.clkCorrection);

    
	temp_mdpdata.prn =satid;
	temp_mdpdata.prn =satid;
	temp_mdpdata.dataNmeas = satMDE_ai.satDgen->satDgen_oData;
	
	temp_mdpdata.carrierDopplerL1 =carrierDopplerL1;
	temp_mdpdata.codeDopplerCaL1 =codeDopplerCaL1;
	temp_mdpdata.carrierDopplerL2 =carrierDopplerL2;
	temp_mdpdata.codeDopplerCaL2 =codeDopplerCaL2;
	temp_mdpdata.carrierDopplerL5 =carrierDopplerL5;
	temp_mdpdata.codeDopplerPyL5 =codeDopplerPyL5;
	temp_mdpdata.rangeRate =tempRangeRate;
	satMDE_pData.push_back(temp_mdpdata);

	if(visCounter == TIME_FOR_ELV_COMP)
		{
		//reset counter
		visiblityComputed = false;
		visCounter = M_ZERO;
		timeToCompute = true;
	}


	}
	prevRange.resize(0);
	prevRange = prange;
	}

	}


double SatMDE::ComputeCorrectedRange(Xvt& satXvt , Xvt& rxXvt)
{
//	WGS84Geoid geoid;
double relativity = (satXvt.ddtime - satXvt.dtime)* geoid.c(); // get relativistic correction
double corr_range = satXvt.preciseRho(rxXvt.x, geoid, relativity) ;
return(corr_range);
}



int SatMDE::findSvPrvRange(SatID vis_svid)
{
 newSV= true;

	for(int i=0; i < prevRange.size() ; i++)
	{
		SatID tempID = prevRange.at(i).prn;
		if(tempID.id == vis_svid.id)
		{
		newSV = false;
		return(i);
		}

		}
		return(0);
		}



#if 0
void SatMDE::updateLocals(const Xvt& satXvt ,const Xvt& rxXvt)
{
       Position Rx(rxXvt);

	   //Rx.setECEF(rxXvt.x[0],rxXvt.x[1],rxXvt.x[2]);
	   rawrange = RSS(satXvt.x[0]-Rx.X(),
                           satXvt.x[1]-Rx.Y(),
                           satXvt.x[2]-Rx.Z());


       cosines[0] = (Rx.X()-satXvt.x[0])/rawrange;
       cosines[1] = (Rx.Y()-satXvt.x[1])/rawrange;
       cosines[2] = (Rx.Z()-satXvt.x[2])/rawrange;

      Position SV(satXvt);
      elevation = Rx.elevation(SV);
      azimuth = Rx.azimuth(SV);
      elevationGeodetic = Rx.elevationGeodetic(SV);
      azimuthGeodetic = Rx.azimuthGeodetic(SV);
    }


#endif


void SatMDE::ComputePhaseAndDoppler(const double &rangeRate,const double &clkCorrection)
{

	//Get the IONO Correction w.r.t to current satellite frequency
	//Remember Satellite Frequency L1 or L2 or L5 is not fixed as they
	//suffer from the satellite clock bias and jitter
	Xvt rxXvt = satMDE_ai.rxXvt;
	DayTime measTime = satMDE_ai.measTime;

	/* Calculate Param for L1 */
	double satOscFreq = gpstk::OSC_FREQ;
	//satMDE_ai.satDgen->satDgen_oData
    
	satOscFreq =  satOscFreq*(1 + clkCorrection);


	double gpsCarrFreq   = satOscFreq * gpstk::L1_MULT;
	double gpsCodeFreqCA = gpsCarrFreq*CODECLKMULT_L1;
	double lambda = C_GPS_M/gpsCarrFreq;
	double gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;

	//gpstk::IonoModel::Frequency  freq = L1;

	carrierDopplerL1 = rangeRate / lambda;
	codeDopplerCaL1  = carrierDopplerL1/gpsCar2CodeRatio;

	/* Calculate Param for L2 */

	double gpsCarrFreq   = satOscFreq * gpstk::L2_MULT;
	double gpsCodeFreqCA = gpsCarrFreq*CODECLKMULT_L2;
	double lambda = C_GPS_M/gpsCarrFreq;
	double gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;

	 carrierDopplerL2 = rangeRate / lambda;
	 codeDopplerCaL2  = carrierDopplerL2/gpsCar2CodeRatio;

	 /* Calculate Param for L5 */

	 double gpsCarrFreq   = satOscFreq * L5_MULT;
	double gpsCodeFreqCA = gpsCarrFreq*CODECLKMULT_L5;
	double lambda = C_GPS_M/gpsCarrFreq;
	double gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;
	 carrierDopplerL5 = rangeRate / lambda;
	 codeDopplerPyL5  = carrierDopplerL5/gpsCar2CodeRatio;

	}







void SatMDE::outputInfFromEngine(std::ostream & 	s )

    {
	string  el;
	el = "";
	using namespace StringUtils;

	satMDE_oData = satMDE_pData;
	el = print_header();
	el += leftJustify("\n",3);
	s<<el;

	for (int prn=0; prn <satMDE_oData.size(); prn++)
      {

		using std::endl;
		using std::setw;
       s << std::setprecision(4);    // set the precision of probe output
	   s << print_content(satMDE_oData[prn]);
	}
	/*el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;
	s << satTime.printf("%Y %03j % 12.6s")<<endl;
	el = leftJustify("______________________________",24);
	el += leftJustify("\n",3);
	s<<el;*/



    }


void SatMDE::SatMDE_ExpHandler(ExcSatMDE &e)
{
	int i=0;
/*
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
	*/

}


std::string SatMDE::print_content(satMDE_ostruct &ostruct)

{
 string  content;
 content = "";
 using namespace StringUtils;
	           content +=  leftJustify(asString(ostruct.prn), 6);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.rawRange), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.corrRange), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.rangeRate), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.carrierDopplerL1), 13);
			   content += leftJustify("\t",3);
			   content += leftJustify(asString((double)ostruct.codeDopplerCaL1), 13);
			   content += leftJustify("\t",3);

			   content += leftJustify(asString((double)ostruct.phaseWindUp), 13);
			   content += leftJustify("\n",3);
			  // content += leftJustify(asString(e.sattime),13);
			  // content += leftJustify("\n",3);
			   return(content);
}

std::string SatMDE::print_header()
{
 string  header;
 header = "";
 using namespace StringUtils;

 header +=  leftJustify("Sys PRN", 6);
			   header += leftJustify("\t",3);
			   header += leftJustify("RawRange X", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("Corr_range Y", 13);
			   header += leftJustify("\t",3);
			    header += leftJustify("Delta Range", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("elevation Z", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("azimuth", 13);
			   header += leftJustify("\t",3);
			   header += leftJustify("IONO Delay", 13);

			   header += leftJustify("\n",3);
			  // header += leftJustify(asString(e.sattime),13);
			  // header += leftJustify("\n",3);
			   return(header);
}


 void SatMDE::getMeasurement (SatID &temp_satid,
							 Position &rxXvt,
							 DayTime  &measTime,
							 SatDb    *satDb,
							 SatDgen *satDgen,
							 double &range,
							 bool firstMeas,
							 bool svRange)
 {

satDgen_activeInput activeInput;
satDgen_activeParam activeControl;
activeInput.measTime = measTime;
activeInput.satid = temp_satid;
activeInput.rxPos = rxXvt;
activeInput.calculatedRange = range;
activeInput.satDb = satDb;

activeControl.firstMeas =firstMeas;
activeControl.svRange =svRange;

satDgen->activeControl=activeControl;
satDgen->activeData=activeInput;
satDgen->Engine(SatDgen::RUN_ALL );
satDgen->EngineOutput(SatDgen::DATA_HERE);
 }





