/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li 
* Email: changhe.lw@google.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

/*
Paper: A. K. Qin, V. L. Huang and P. N. Suganthan, ¡°Differential evolution with 
strategy adaptation for global numerical optimization,¡± IEEE Transactions on 
Evolutionary Computation, 13(2): 398- 417, 2009. 
*/
#ifndef OFEC_SADE_H
#define OFEC_SADE_H

#include <list>
#include <algorithm>
#include "../DEindividual.h"
#include "../DEpopulation.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	class SaDE :public DEpopulation<DEindividual<>>   
	{
	public:
		SaDE(param_map &v);
		evaluation_tag run_();
	protected:
		evaluation_tag evolve();
		void update_F();
		void update_CR();
		void update_memory();
	protected:
		const int mc_numStr = 4;
		std::vector<double> mv_F;
		std::vector<std::vector<double>> mv_CR;

		std::list<std::vector<std::list<double>>> mv_CRsuc;

		std::vector<double> mv_CRm, mv_prob;
		std::list<std::vector<int>> mv_cntSuc, mv_cntFail;
		int m_LP = 50;
		double m_epsilon = 0.01;
		std::vector<int> m_strSel;
		//enum Strategy { DE_rand_1_bin = 0, DE_rand_to_best_2_bin, DE_rand_2_bin, DE_current_to_rand_1 };
		violation_type m_mode = violation_type::Boundary;
	};
}
#endif // OFEC_SADE_H
