// ------------------------------------------------------------------------ //
// This source file is part of the 'ESA Advanced Concepts Team's			//
// Space Mechanics Toolbox' software.                                       //
//                                                                          //
// The source files are for research use only,                              //
// and are distributed WITHOUT ANY WARRANTY. Use them on your own risk.     //
//                                                                          //
// Copyright (c) 2004-2007 European Space Agency                            //
// ------------------------------------------------------------------------ //



#ifndef GTOP_TOOLBOX_LAMBERT_H
#define GTOP_TOOLBOX_LAMBERT_H


#include "Astro_Functions.h"

namespace gtop_toolbox {

	void LambertI(const double *, const double *, double, const double, const int,  //INPUT
		double *, double *, double &, double &, double &, int &);//OUTPUT

}

#endif

