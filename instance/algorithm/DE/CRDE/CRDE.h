/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
// updated Mar 28, 2018 by Li Zhou

/*
Thomsen R (2004) Multimodal optimization using crowding-based differential evolution. In: Proceedings of
the 2004 IEEE congress on evolutionary computation, pp 1382¨C1389. IEEE Press, Portland, OR, USA
*/
#ifndef OFEC_CRDE_H
#define OFEC_CRDE_H

#include "../individual.h"
#include "../population.h"
#include "../../../../core/measure/measure.h"
namespace OFEC {
	namespace DE {
		class CRDE :public population<individual>
		{
		public:
			CRDE(param_map &v);
			evaluation_tag run_();
			evaluation_tag evolve();
		};
	}
	using CRDE = DE::CRDE;
}
#endif // OFEC_CRDE_H
