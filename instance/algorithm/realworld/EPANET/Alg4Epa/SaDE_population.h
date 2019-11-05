/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
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
// updated Jan 19, 2019 by Li Zhou

#ifndef OFEC_SADE_POPULATION
#define OFEC_SADE_POPULATION

#include "epanet_individual.h"
#include "../../../../../core/algorithm/population.h"
#include <list>
#include <algorithm>

namespace OFEC {

	class SaDE_population : public population<epanet_individual>
	{
		enum DE_mutation_stratgy { rand_1, best_1, target_to_best_1, best_2, rand_2, rand_to_best_1, target_to_rand_1 };
		DE_mutation_stratgy m_mutation_strategy;
	protected:
		const int m_num_strategy = 4;
		std::vector<real> m_F;
		std::vector<std::vector<real>> mvv_CR;

		std::list<std::vector<std::list<real>>> m_CRsuc;

		std::vector<real> m_CRm, m_pro_strategy;
		std::list<std::vector<int>> m_cnt_success, m_cnt_fail;
		int m_LP = 5;
		real m_epsilon = 0.01;
		std::vector<int> m_strategy_selection;
		violation_type m_mode = violation_type::Boundary;

		//for ST and duration
		std::vector<std::vector<real>> m_probability;
		std::vector<std::pair<real, size_t>> m_ST_data_obj;
		std::vector<std::pair<real, size_t>> m_duration_data_obj;
	public:
		SaDE_population(size_t size_pop);
		SaDE_population & operator=(SaDE_population & pop) {
			population::operator=(pop);

			m_F = pop.m_F;
			mvv_CR = pop.mvv_CR;
			m_CRsuc = pop.m_CRsuc;
			m_CRm = pop.m_CRm;
			m_pro_strategy = pop.m_pro_strategy;
			m_cnt_success = pop.m_cnt_success;
			m_cnt_fail = pop.m_cnt_fail;
			m_strategy_selection = pop.m_strategy_selection;
			m_mode = pop.m_mode;
			m_probability = pop.m_probability;
			m_ST_data_obj = pop.m_ST_data_obj;
			m_duration_data_obj = pop.m_duration_data_obj;

			return *this;
		}
		SaDE_population & operator=(SaDE_population && pop) {
			population::operator=(std::move(pop));

			m_F = std::move(pop.m_F);
			mvv_CR = std::move(pop.mvv_CR);
			m_CRsuc = std::move(pop.m_CRsuc);
			m_CRm = std::move(pop.m_CRm);
			m_pro_strategy = std::move(pop.m_pro_strategy);
			m_cnt_success = std::move(pop.m_cnt_success);
			m_cnt_fail = std::move(pop.m_cnt_fail);
			m_strategy_selection = std::move(pop.m_strategy_selection);
			m_mode = pop.m_mode;
			m_probability = std::move(pop.m_probability);
			m_ST_data_obj = std::move(pop.m_ST_data_obj);
			m_duration_data_obj = std::move(pop.m_duration_data_obj);

			return *this;
		}

		void mutate();
		void mutate_(const int idx, real F, const std::vector<std::vector<real>> & pro);
		void recombine();

		void select(int base, int number, std::vector<int>& result);
		evaluation_tag select(bool is_stable);
		void set_mutation_strategy(DE_mutation_stratgy rS);

		void update_F();
		void update_CR();
		void update_pro_strategy();
		void update_probability();

		void fill_solution(const epanet_individual & indi);
		bool is_feasible_population(const real tar);
	};

}

#endif // !OFEC_SADE_POPULATION

