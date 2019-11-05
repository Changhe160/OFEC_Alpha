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

#ifndef OFEC_GL_POPULATION
#define OFEC_GL_POPULATION

#include "epanet_individual.h"
#include "../../../../../core/algorithm/population.h"


namespace OFEC {

	class GL_population : public population<epanet_individual>
	{
	protected:
		std::vector<std::pair<real, size_t>> m_node_data_obj;
		std::vector<real> m_probability;

	public:
		GL_population(size_t size_pop);
		GL_population & operator=(GL_population & pop) {
			population::operator=(pop);	
			m_probability = pop.m_probability;
			m_node_data_obj = pop.m_node_data_obj;

			return *this;
		}
		GL_population & operator=(GL_population && pop) {
			population::operator=(std::move(pop));
			m_probability = std::move(pop.m_probability);
			m_node_data_obj = std::move(pop.m_node_data_obj);
			
			return *this;
		}

		void mutate();
		void update_probability();
		
		evaluation_tag select(bool is_stable);
		void fill_solution(const epanet_individual & indi);
		bool is_feasible_population(const real tar);

	};

}

#endif // !OFEC_GL_POPULATION

