// ------------------------------------------------------------------------ //
// This source file is part of the 'ESA Advanced Concepts Team's			//
// Space Mechanics Toolbox' software.                                       //
//                                                                          //
// The source files are for research use only,                              //
// and are distributed WITHOUT ANY WARRANTY. Use them on your own risk.     //
//                                                                          //
// Copyright (c) 2004-2007 European Space Agency                            //
// ------------------------------------------------------------------------ //

#ifndef GTOP_TOOLBOX_MGA_DSM_H
#define GTOP_TOOLBOX_MGA_DSM_H

#include <vector>
#include <cmath>
#include "Pl_Eph_An.h"
#include "Astro_Functions.h"
#include "propagateKEP.h"
#include "Lambert.h"
#include "time2distance.h"
#include "mga.h"

namespace gtop_toolbox {

	using namespace std;

	struct mgadsmproblem {
		int type;							//problem type
		vector<int> sequence;				//fly-by sequence (ex: 3,2,3,3,5,is Earth-Venus-Earth-Earth-Jupiter)
		double e;							//insertion e (only in case total_DV_orbit_insertion)
		double rp;							//insertion rp in km (only in case total_DV_orbit_insertion)
		customobject asteroid;			//asteroid data (in case fly-by sequence has a final number = 10)
		double AUdist;						//Distance to reach in AUs (only in case of time2AUs)
		double DVtotal;						//Total DV allowed in km/s (only in case of time2AUs)
		double DVonboard;					//Total DV on the spacecraft in km/s (only in case of time2AUs)

		//Pre-allocated memory, in order to remove allocation of heap space in MGA_DSM calls
		//The DV vector serves also as output containing all the values for the impulsive DV 
		std::vector<double *> r;// = std::vector<double*>(n);
		std::vector<double *> v;// = std::vector<double*>(n);
		std::vector<double> DV;// = std::vector<double>(n+1);
	};


	int MGA_DSM(
		/* INPUT values: */
		vector<double> x,	// it is the decision vector
		mgadsmproblem &mgadsm,  // contains the problem specific data, passed as reference as mgadsm.DV is an output 

		/* OUTPUT values: */
		double &J    // J output
	);

}

#endif
