/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Jun 10, 2019 by Li Zhou

#ifndef OFEC_ADOPT_COMBINATION_H
#define OFEC_ADOPT_COMBINATION_H

#include "ADOPT.h"
#include "../SaDE_population.h"
#include "../GL_population.h"

namespace OFEC {

	class ADOPT_combination : public ADOPT<GL_population, SaDE_population, epanet_individual>
	{
	public:
		ADOPT_combination(param_map & v) : ADOPT(v) {}
	};

}

#endif