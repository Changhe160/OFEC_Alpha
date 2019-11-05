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
// updated Apr 12, 2019 by Li Zhou

#ifndef OFEC_ESS_INDIVIDUAL_H
#define OFEC_ESS_INDIVIDUAL_H

#include "../../../../../../core/algorithm/individual.h"

#include "../../../../../problem/realworld/EPANET/epa_encoding.h"
#include "../../../../../problem/realworld/EPANET/epanet.h"

#include <cmath>
#include <fstream>
#include <cstring>

namespace OFEC {

	class ESs_individual : public individual<variable_epanet, real>
	{
	public:
	    ESs_individual(size_t num_obj, size_t num_con) : individual(num_obj,num_con) {}
		void initialize(int id);
		void mutate(real delta_user, const std::vector<real> & pro);

	protected:
		template<typename Type>
		void gauss_random(Type & value, real delta) {
			value = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(value, delta);
		}
	public:
		std::vector<real> m_delta_mass;
		real m_delta_node;
		real m_delta_start_time;
		real m_delta_add;
		real m_delta_deletion;
		real m_delta_max_depth; 

		real m_rate_add;
		real m_rate_deletion;
		int m_max_depth;
	};


}

#endif // !OFEC_ESS_INDIVIDUAL_H


