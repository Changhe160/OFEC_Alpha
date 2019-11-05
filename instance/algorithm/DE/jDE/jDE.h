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

/*
Paper: J. Brest, S. Greiner, B. Boskovic, M. Mernik, V. Zumer. Self-Adapting
Control Parameters in Differential Evolution: A Comparative Study on Numerical Benchmark
Problems. IEEE Transactions on Evolutionary Computation, 2006, Vol. 10, Issue 6, pp. 646-657.
*/

#ifndef OFEC_JDE_H
#define OFEC_JDE_H

#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class jDE_pop final : public DE::population<DE::individual>
	{
	public:
		explicit jDE_pop(size_t size_pop);
		evaluation_tag evolve() override;
	protected:
		void update_F_and_CR();
	protected:
		real m_t1=0, m_t2=0, m_Fl=0, m_Fu=0;
		std::vector<real> mv_F, mv_CR;
	};

	class jDE : public algorithm {
	public:
		jDE(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		jDE_pop m_pop;
	};
}


#endif // OFEC_JDE_H
