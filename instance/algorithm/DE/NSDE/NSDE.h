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
Paper:B. Y. Qu, P. N. Suganthan, and J. J. Liang, ��Differential evolution
with neighborhood mutation for multimodal optimization,�� IEEE Trans.
Evol. Comput., vol. 16, no. 5, pp. 601�C614, Oct. 2012.
*/
#ifndef OFEC_NSDE_H
#define OFEC_NSDE_H


#include "../individual.h"
#include "../population.h"
#include <list>
#include "../../../../core/measure/measure.h"

namespace OFEC {
	namespace DE {
		class NSDE :public population<individual>
		{
		public:
			NSDE(param_map &v);
			evaluation_tag run_();
		protected:
			evaluation_tag evolve();
			void select_subpopulation();
			void sort();
		protected:
			int m_m;                                //size of neighborhood
			std::vector<std::list<std::pair<double, int>>> m_dis;  //save individuals' distance
			std::vector<int> m_seed;                     //best fittness of neighborhood
			std::vector<int> m_order_list;
		};
	}
	using NSDE = DE::NSDE;
}


#endif // NSDE_H
