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
	channel.passiveData.alpha[0] = 0.010e-6;
	channel.passiveData.alpha[1] = 0.000e-6;
	channel.passiveData.alpha[2] = -0.060e-6;
	channel.passiveData.alpha[3] = 0.000e-6;
	channel.passiveData.beta[0] =90e3;
channel.passiveData.beta[1] =0e3;
channel.passiveData.beta[2] =-197e3;
channel.passiveData.beta[3] =0e3;
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

visSV= M_ZERO;
timeToCompute = true;
visiblityComputed = false;
visCounter = M_ZERO;
newSV = true;
isThisFirstTime = false;



}

void SatMDE::GetActiveParam()
{
	satMDE_pData.clear();
	
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
DayTime gpsTrueTime = satMDE_ai.gpsTrueTime;

int N = satMDE_ai.satDb_ref->size();
int i;
Xvt rxXvt = satMDE_ai.rxXvt;

WGS84Geoid geoid;
double tempPR, tempIono , tempTrop ,tempRangeRate;

prange.resize(0);

visSV = M_ZERO;


if(timeToCompute)

{
timeToCompute = false;
rejectedSV.resize(0);
visibleSV.resize(0);

for (int i=0; i<N; i++)
	{
	Xvt satXvt= satMDE_ai.satDb_ref->at(i).satXvt ;
	SatID satid = satMDE_ai.satDb_ref->at(i).prn ;
	
	updateLocals(satXvt,rxXvt);
	
	if(elevation < satMDE_pi.minElev)
	{
	rejectedSV.push_back(satid);
	continue;
	}
	else
	{
	visSV ++;
	visibleSV.push_back(satid);
	visiblityComputed = true;
	
	// Find Ranges when complete set is visible

	current.prn=satMDE_ai.satDb_ref->at(i).prn;
	current.range = ComputeCorrectedRange(satXvt,rxXvt);
	prange.push_back(current);
	
	int loc = findSvPrvRange(satid); // this gives the location of previous range of the if SV was visblible in previous run
	if(newSV)
	prevsecond.range = (double)M_ZERO;
	else
	prevsecond.range = prevRange.at(loc).range;
	prevsecond.prn = satid;
	tempRangeRate = current.range - prevsecond.range;
	prevsecond.range =current.range ; // update the SV range to current range
	
	ComputePhaseAndDoppler(i,tempRangeRate);
	temp_mdpdata.phaseWindUp = ComputePhaseWindUpDelay(i,gpsTrueTime);

	temp_mdpdata.prn =current.prn;
	temp_mdpdata.elevation = elevation;
	temp_mdpdata.azimuth = azimuth;
	temp_mdpdata.corrRange =current.range;
	temp_mdpdata.rawRange = rawrange;
	temp_mdpdata.ionoDelayL1 =ionoL1;
	temp_mdpdata.carrierDopplerL1 =carrierDopplerL1;
	temp_mdpdata.codeDopplerCaL1 =codeDopplerCaL1;
	temp_mdpdata.ionoDelayL2 =ionoL2;
	temp_mdpdata.carrierDopplerL2 =carrierDopplerL2;
	temp_mdpdata.codeDopplerCaL2 =codeDopplerCaL2;
	temp_mdpdata.ionoDelayL5 =ionoL5;
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
	for (int i=0; i<N; i++)
	{
	Xvt satXvt= satMDE_ai.satDb_ref->at(i).satXvt ;
	SatID satid = satMDE_ai.satDb_ref->at(i).prn ;
	
	updateLocals(satXvt,rxXvt);
	// Compute range with only visible sat 
	current.prn=satMDE_ai.satDb_ref->at(i).prn;
	
	//PseudoRange Calculation
	current.range = ComputeCorrectedRange(satXvt,rxXvt);
	prange.push_back(current);
	
	
	//RangeRate Calculation
	
	tempRangeRate = current.range - prevRange.at(i).range;
	
	//Doppler, IONO, Phase
	ComputePhaseAndDoppler(i,tempRangeRate);
	
	
	temp_mdpdata.phaseWindUp = ComputePhaseWindUpDelay(i,gpsTrueTime);
	
	
    
	temp_mdpdata.prn =current.prn;
	temp_mdpdata.elevation = elevationGeodetic;
	temp_mdpdata.azimuth = azimuthGeodetic;
	temp_mdpdata.corrRange =current.range;
	temp_mdpdata.rawRange = rawrange;
	temp_mdpdata.rangeRate =tempRangeRate;
	temp_mdpdata.ionoDelayL1 =ionoL1;
	temp_mdpdata.carrierDopplerL1 =carrierDopplerL1;
	temp_mdpdata.codeDopplerCaL1 =codeDopplerCaL1;
	temp_mdpdata.ionoDelayL2 =ionoL2;
	temp_mdpdata.carrierDopplerL2 =carrierDopplerL2;
	temp_mdpdata.codeDopplerCaL2 =codeDopplerCaL2;
	temp_mdpdata.ionoDelayL5 =ionoL5;
	temp_mdpdata.carrierDopplerL5 =carrierDopplerL5;
	temp_mdpdata.codeDopplerPyL5 =codeDopplerPyL5;
	
	
	
	satMDE_pData.push_back(temp_mdpdata);
	}
	prevRange.resize(0);
	prevRange = prange;


	visCounter++;
	timeToCompute = false;
	
	if(visCounter == TIME_FOR_ELV_COMP)
	{
	//reset counter
	visiblityComputed = false;
	visCounter = M_ZERO;
	timeToCompute = true;
	}

	}

	}
	
	
double SatMDE::ComputeCorrectedRange(Xvt& satXvt , Xvt& rxXvt)
{
	WGS84Geoid geoid;
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
  


 

void SatMDE::ComputePhaseAndDoppler(int sat_index,double rangeRate)
{
	
	//Get the IONO Correction w.r.t to current satellite frequency
	//Remember Satellite Frequency L1 or L2 or L5 is not fixed as they
	//suffer from the satellite clock bias and jitter
	Xvt rxXvt = satMDE_ai.rxXvt;
	DayTime gpsTrueTime = satMDE_ai.gpsTrueTime;
	
	/* Calculate Param for L1 */
	
	double gpsCarrFreq   = satMDE_ai.satDb_ref->at(sat_index).carrFreqL1 ;
	double gpsCodeFreqCA = satMDE_ai.satDb_ref->at(sat_index).chipFreqCA ;
	double lambda = C_GPS_M/gpsCarrFreq;
	double gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;

	//gpstk::IonoModel::Frequency  freq = L1;
	
	ionoL1 =  getIonoCorrections(alpha,beta,gpsTrueTime,
						rxXvt,
						elevation,
						azimuth,gpstk::IonoModel::L1);
						
	carrierDopplerL1 = rangeRate / lambda;
	codeDopplerCaL1  = carrierDopplerL1/gpsCar2CodeRatio;
	
	/* Calculate Param for L2 */
	
	 gpsCarrFreq   = satMDE_ai.satDb_ref->at(sat_index).carrFreqL2 ;
	 gpsCodeFreqCA = satMDE_ai.satDb_ref->at(sat_index).chipFreqCA ;
	 lambda = C_GPS_M/gpsCarrFreq;
	 gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;
	 
	 ionoL2 =  getIonoCorrections(alpha,beta,gpsTrueTime,
						rxXvt,
						elevation,
						azimuth,gpstk::IonoModel::L2);
	
	 carrierDopplerL2 = rangeRate / lambda;
	 codeDopplerCaL2  = carrierDopplerL2/gpsCar2CodeRatio;
	 
	 /* Calculate Param for L2 */
	
	 gpsCarrFreq   = satMDE_ai.satDb_ref->at(sat_index).carrFreqL5 ;
	 gpsCodeFreqCA = satMDE_ai.satDb_ref->at(sat_index).chipFreqPY ; //L5 chip rate = 10.23e6 same as P(Y) in L1
	 lambda = C_GPS_M/gpsCarrFreq;
	 gpsCar2CodeRatio = gpsCarrFreq / gpsCodeFreqCA;
	 
	 ionoL5 =  getIonoCorrections(alpha,beta,gpsTrueTime,
						rxXvt,
						elevation,
						azimuth,gpstk::IonoModel::L2);
	
	 carrierDopplerL5 = rangeRate / lambda;
	 codeDopplerPyL5  = carrierDopplerL5/gpsCar2CodeRatio;
	
	}
	
	
	
	


double SatMDE::getIonoCorrections(double alpha[],double beta[],DayTime Tr,
                                                   Xvt &rxXvt,
                                                   double elevation,
                                                   double azimuth,gpstk::IonoModel::Frequency )
    {
		Position rxPos(rxXvt);   // Convert Position rxPos to Geodetic rxGeo
		Geodetic rxGeo( rxPos.getGeodeticLatitude(),
					rxPos.getLongitude(),
					rxPos.getAltitude() );


       IonoModel tempIono(alpha,beta);


		
		double ionoCorr(0.0);
 
       try
       {
		   ionoCorr = tempIono.getCorrection(Tr, rxGeo, elevation, azimuth,gpstk::IonoModel::L1);
       }
       catch(IonoModel::InvalidIonoModel& e)
       {
          ionoCorr = 0.0;
       }
 
       return ionoCorr;
 
    }  // End of method 'ModeledReferencePR::getIonoCorrections()'

    
  

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
           
double SatMDE::ComputePhaseWindUpDelay(int sat_index,DayTime time)
{	

SunPosition sunPosition;
Triple sunPos(sunPosition.getPosition(time));
Xvt satXvt= satMDE_ai.satDb_ref->at(sat_index).satXvt ;
SatID satid =satMDE_ai.satDb_ref->at(sat_index).prn ;
Triple svPos = satXvt.x;
Xvt rxXvt = satMDE_ai.rxXvt;
Position posRx(rxXvt);

double error =getWindUp(satid, time, svPos, sunPos ,posRx);

return(error);
}
		
double SatMDE::getWindUp( const SatID& satid,
                                    const DayTime& time,
                                    const Triple& sat,
                                    const Triple& sunPosition,
									const Position& posRx)
   {

         // Unitary vector from satellite to Earth mass center
      Triple rk( ( (-1.0)*(sat.unitVector()) ) );

         // Unitary vector from Earth mass center to Sun
      Triple ri( sunPosition.unitVector() );

         // rj = rk x ri
      Triple rj(rk.cross(ri));

         // ri = rj x rk
      ri = rj.cross(rk);

         // Let's convert ri, rj to unitary vectors.
         // Now ri, rj, rk form a base in the ECEF reference frame
      ri = ri.unitVector();
      rj = rj.unitVector();


         // Get satellite rotation angle

         // Get vector from Earth mass center to receiver
      Triple rxPos(posRx.X(), posRx.Y(), posRx.Z());

         // Compute unitary vector vector from satellite to RECEIVER
      Triple rrho( (rxPos-sat).unitVector() );

         // Vector from SV to Sun center of mass
      Triple gps_sun( sunPosition-sat );

         // Redefine rk: Unitary vector from SV to Earth mass center
      rk = (-1.0)*(sat.unitVector());

         // Redefine rj: rj = rk x gps_sun, then make sure it is unitary
      rj = (rk.cross(gps_sun)).unitVector();

         // Redefine ri: ri = rj x rk, then make sure it is unitary
         // Now, ri, rj, rk form a base in the satellite body reference
         // frame, expressed in the ECEF reference frame
      ri = (rj.cross(rk)).unitVector();


         // Projection of "rk" vector to line of sight vector (rrho)
      double zk(rrho.dot(rk));

         // Get a vector without components on rk (i.e., belonging
         // to ri, rj plane)
      Triple dpp(rrho-zk*rk);

         // Compute dpp components in ri, rj plane
      double xk(dpp.dot(ri));
      double yk(dpp.dot(rj));

         // Compute satellite rotation angle, in radians
      double alpha1(std::atan2(yk,xk));


         // Get receiver rotation angle

         // Redefine rk: Unitary vector from Receiver to Earth mass center
      rk = (-1.0)*(rxPos.unitVector());

         // Let's define a NORTH unitary vector in the Up, East, North
         // (UEN) topocentric reference frame
      Triple delta(0.0, 0.0, 1.0);

         // Rotate delta to XYZ reference frame
      delta =
         (delta.R2(tempPos.geodeticLatitude())).R3(-tempPos.longitude());


         // Computation of reference trame unitary vectors for receiver
         // rj = rk x delta, and make it unitary
      rj = (rk.cross(delta)).unitVector();

         // ri = rj x rk, and make it unitary
      ri = (rj.cross(rk)).unitVector();

         // Projection of "rk" vector to line of sight vector (rrho)
      zk = rrho.dot(rk);

         // Get a vector without components on rk (i.e., belonging
         // to ri, rj plane)
      dpp = rrho-zk*rk;

         // Compute dpp components in ri, rj plane
      xk = dpp.dot(ri);
      yk = dpp.dot(rj);

         // Compute receiver rotation angle, in radians
      double alpha2(std::atan2(yk,xk));

      double wind_up(0.0);

         // Find out if satellite belongs to block "IIR", because
         // satellites of block IIR have a 180 phase shift
int IIR_Sat[12]= {2,11,13,14,16,18,19,20,21,22,23,28};

 for(int i=0;i<12;i++)
 {
	 IIR = false;
	 if(satid.id == IIR_Sat[i])
	 {
		IIR = true;
		break;
	 }
 }
	 
 
      if(IIR)
      {
         wind_up = PI;
      }

      alpha1 = alpha1 + wind_up;

      double da1(alpha1-phase_satellite[satid].previousPhase);

      double da2(alpha2-phase_station[satid].previousPhase);

         // Let's avoid problems when passing from 359 to 0 degrees.
      phase_satellite[satid].previousPhase += std::atan2( std::sin(da1),
                                                          std::cos(da1) );

      phase_station[satid].previousPhase += std::atan2( std::sin(da2),
                                                        std::cos(da2) );

         // Compute wind up effect in radians
      wind_up = phase_satellite[satid].previousPhase -
                phase_station[satid].previousPhase;

      return wind_up;

   }  // End of method 'ComputeWindUp::getWindUp()'

