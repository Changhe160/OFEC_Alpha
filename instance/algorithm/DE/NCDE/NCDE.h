#ifndef OFEC_NCDE_H
#define OFEC_NCDE_H

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

#include "../DEindividual.h"
#include "../DEpopulation.h"
#include "../../../../core/measure/measure.h"
#include <list>
namespace OFEC {
	class NCDE :public DEpopulation<DEindividual<>>
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


#endif // OFEC_NCDE_H

