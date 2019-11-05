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
// Updated on 15th Aug, 2019 by Junchen Wang


#ifndef OFEC_NCDE_H
#define OFEC_NCDE_H


/*
Paper:B. Y. Qu, P. N. Suganthan, and J. J. Liang, ¡°Differential evolution
with neighborhood mutation for multimodal optimization,¡± IEEE Trans.
Evol. Comput., vol. 16, no. 5, pp. 601¨C614, Oct. 2012.
*/

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"
#include <list>

namespace OFEC {
	class NCDE_pop final : public DE::population<DE::individual>
	{
	public:
		NCDE_pop(size_t size_pop);
		evaluation_tag evolve() override;
	protected:
		void sort_distance(size_t a);
	protected:
		std::vector<std::list<std::pair<real, int>>> m_dis;  //save individuals' distance
		real m_r;                             //size of neighborhood
		size_t m_m;                                //size of neighborhood
	};

	class NCDE final : public algorithm
	{
	public:
		NCDE(param_map &v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		NCDE_pop m_pop;
	};
}


#endif // OFEC_NCDE_H

