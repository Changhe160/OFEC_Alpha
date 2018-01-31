/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

/*
Paper:B. Y. Qu, P. N. Suganthan, and J. J. Liang, ¡°Differential evolution
with neighborhood mutation for multimodal optimization,¡± IEEE Trans.
Evol. Comput., vol. 16, no. 5, pp. 601¨C614, Oct. 2012.
*/
#ifndef OFEC_NSDE_H
#define OFEC_NSDE_H


#include "../DEindividual.h"
#include "../DEpopulation.h"
#include <list>
#include "../../../../core/measure/measure.h"

namespace OFEC {
	class NSDE :public DEpopulation<DEindividual<>>
	{
	public:
		NSDE(param_map &v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		void selectSubpop();
		void sort();
	protected:
		int m_m;                                //size of neighborhood
		std::vector<std::list<std::pair<double, int>>> m_dis;  //save individuals' distance
		std::vector<int> m_seed;                     //best fittness of neighborhood
		std::vector<int> m_order_list;
	};
}


#endif // NSDE_H
