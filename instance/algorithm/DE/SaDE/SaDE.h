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
Paper: A. K. Qin, V. L. Huang and P. N. Suganthan, ¡°Differential evolution with 
strategy adaptation for global numerical optimization,¡± IEEE Transactions on 
Evolutionary Computation, 13(2): 398- 417, 2009. 
*/
#ifndef OFEC_SADE_H
#define OFEC_SADE_H

#include <list>
#include "../population.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class SaDE_pop final : public DE::population<DE::individual> {
	public:
		SaDE_pop(size_t size_pop);
		evaluation_tag evolve() override;
	protected:
		void update_F();
		void update_CR();
		void update_memory();
	protected:
		const size_t m_num_strategy = 4;
		std::vector<real> mv_F;
		std::vector<std::vector<real>> mvv_CR;

		std::list<std::vector<std::list<real>>> m_CRsuc;

		std::vector<real> m_CRm, m_probability;
		std::list<std::vector<int>> m_cnt_success, m_cnt_fail;
		size_t m_LP = 50;
		real m_epsilon = 0.01;
		std::vector<int> m_strategy_selection;
		//enum Strategy { DE_rand_1_bin = 0, DE_rand_to_best_2_bin, DE_rand_2_bin, DE_current_to_rand_1 };
		violation_type m_mode = violation_type::Boundary;
	};

	class SaDE final : public algorithm
	{
	public:
		SaDE(param_map& v);
		void initialize() override;
		void record() override;
	protected:
		void run_() override;
	protected:
		SaDE_pop m_pop;
	};
}
#endif // OFEC_SADE_H
