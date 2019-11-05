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
// updated Apr 3, 2018 by Li Zhou
// Updated on 15th Aug, 2019 by Junchen Wang

/*
Mendes, R., Mohais, A.: DynDE: a differential evolution for dynamic optimization problems. In: Proceedings
of the IEEE Congress on Evolutionary Computation (CEC¡¯05), pp. 2808¨C2815. IEEE (2005)
*/

#ifndef OFEC_DYNDE_H
#define OFEC_DYNDE_H

#include "DynDE_subpopulation.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class DynDE : public algorithm { 
	public:
		explicit DynDE(param_map &v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
		void exclusion_check();
		real m_r_excl; // radius of exlusion radius
		multi_population<DynDE_subpopulation> m_multi_pop;
	};
}
#endif // OFEC_DYNDE_H
