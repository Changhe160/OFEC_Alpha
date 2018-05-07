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
// updated Apr 4, 2018 by Li Zhou

#ifndef ADOPT_SUB_POPULATION
#define ADOPT_SUB_POPULATION

//#include "../../../core/algorithm/population.h"
#include "../DE/DE_population.h"
#include "ADOPT_individual.h"
#include "../../problem/realworld/EPANET/epanet.h"

namespace OFEC {
	class ADOPT_sub_population : public population<ADOPT_individual>
	{
	protected:
		double  m_F, m_CR;
		enum DE_mutation_stratgy { DE_rand_1, DE_best_1, DE_target_to_best_1, DE_best_2, DE_rand_2, DE_rand_to_best_1, DE_target_to_rand_1 };
		DE_mutation_stratgy m_mutation_strategy;
	public:
		template<typename ... Args>
		ADOPT_sub_population(size_t no, Args&& ... args) : population(no, std::forward<Args>(args)...), m_probability(3), \
			m_node_data_obj(CAST_RP_EPANET->number_node(), std::make_pair(0.0, 0)),\
			m_ST_data_obj(CAST_RP_EPANET->max_start_time_size(), std::make_pair(0.0, 0)),\
			m_duration_data_obj(CAST_RP_EPANET->max_duration_size(), std::make_pair(0.0, 0)) {
			m_probability[0].resize(CAST_RP_EPANET->number_node());
			m_probability[1].resize(CAST_RP_EPANET->max_start_time_size());
			m_probability[2].resize(CAST_RP_EPANET->max_duration_size());
			default_parameter();
			set_mutation_strategy(DE_rand_1); 
			set_parmeter(0.6, 1);
		}

		void mutate_(const int idx, const std::vector<std::vector<double>> & pro);

		bool is_feasible_population(const float tar);
		void mutate();
		void mutate(const int idx){}
		void recombine();
		void update_probability();
		evaluation_tag select(bool flag);

		void set_mutation_strategy(DE_mutation_stratgy rS);
		void default_parameter();
		void set_parmeter(double cr, double f);

	protected:
		virtual void select_in_neighborhood(int number, std::vector<int>&, std::vector<int>&);
		virtual void select(int base, int number, std::vector<int>& result);
	public:
		bool m_is_feasible = false;
		std::vector<std::vector<double>> m_probability;
		std::vector<std::pair<real, size_t>> m_node_data_obj;
		//std::vector<double> m_duration_probability;
		std::vector<std::pair<real, size_t>> m_ST_data_obj;
		std::vector<std::pair<real, size_t>> m_duration_data_obj;
	};
}

#endif // !ADOPT_SUB_POPULATION


