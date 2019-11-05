/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created by Junchen Wang on Oct. 29, 2018.

/*Hansen, N. and A.Ostermeier(2001).
Completely Derandomized Self - Adaptation in Evolution Strategies.
Evolutionary Computation, 9(2), pp. 159 - 195;
https://github.com/CMA-ES/c-cmaes */

#ifndef OFEC_CMAES_H
#define OFEC_CMAES_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/solution.h"
#include "cmaes.h"
#include <vector>

namespace OFEC {
	class CMA_ES : public algorithm {
	public:
		CMA_ES(param_map& v);
		void initialize() override;
		void run_() override;
		void record() override;
	private:
		cmaes_t m_evo;
		std::vector<solution<>>  m_inds;
		std::string signalsFilePathName;
		std::string initialsFilePathName;
		double *arFunvals, *const*pop, *xfinal;
		size_t size_pop, size_var;
	};
}

#endif // !OFEC_CMAES_H

