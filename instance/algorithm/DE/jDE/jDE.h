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

#ifndef OFEC_JDE_H
#define OFEC_JDE_H

#include "../DE_individual.h"
#include "../DE_population.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	namespace DE {
		class jDE :public population<individual>
		{
		public:
			jDE(param_map &v);
			evaluation_tag run_();
		protected:
			evaluation_tag evolve();
			void update_F_and_CR();
		protected:
			double m_t1, m_t2, m_Fl, m_Fu;
			std::vector<double> mv_F, mv_CR;
		};
	}
	using jDE = DE::jDE;
}


#endif // OFEC_JDE_H
