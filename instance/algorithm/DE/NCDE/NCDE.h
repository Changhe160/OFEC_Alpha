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


#ifndef OFEC_NCDE_H
#define OFEC_NCDE_H


/*
Paper:B. Y. Qu, P. N. Suganthan, and J. J. Liang, ¡°Differential evolution
with neighborhood mutation for multimodal optimization,¡± IEEE Trans.
Evol. Comput., vol. 16, no. 5, pp. 601¨C614, Oct. 2012.
*/

#include "../DE_individual.h"
#include "../DE_population.h"
#include "../../../../core/measure/measure.h"
#include <list>
namespace OFEC {
	namespace DE {
		class NCDE :public population<individual>
		{
		public:
			NCDE(param_map &v);
			evaluation_tag run_();
		protected:
			evaluation_tag evolve();
			void sort_distanece(int a);
		protected:
			std::vector<std::list<std::pair<double, int>>> m_dis;  //save individuals' distance
			double m_r;                             //size of neighborhood
			int m_m;                                //size of neighborhood
		};
	}
	using NCDE = DE::NCDE;
}


#endif // OFEC_NCDE_H

