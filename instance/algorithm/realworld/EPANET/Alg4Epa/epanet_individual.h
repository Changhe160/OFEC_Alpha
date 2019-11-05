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

#ifndef EPANET_INDIVIDUAL_H
#define EPANET_INDIVIDUAL_H

#include "../../../../../core/algorithm/individual.h"

#include "../../../../problem/realworld/EPANET/epa_encoding.h"
#include "../../../../problem/realworld/EPANET/epanet.h"

#include <cmath>
#include <fstream>
#include <cstring>

namespace OFEC {

	class epanet_individual : public individual<variable_epanet, real>
	{
	protected:
		solution_type m_pv, m_pu;
	public:
	    epanet_individual(size_t num_obj, size_t num_con) : individual(num_obj,num_con) {}
        epanet_individual(const solution<variable_epanet,real>& sol) : individual(sol), m_pv(sol), m_pu(sol) {}
		void mutate_first_part(const std::vector<real> & pro);
		void mutate_second_part(real F, const std::vector<std::vector<real>> & pro,
			solution<variable_epanet, real> *r1,
			solution<variable_epanet, real> *r2,
			solution<variable_epanet, real> *r3,
			solution<variable_epanet, real> *r4 = 0,
			solution<variable_epanet, real> *r5 = 0);

		void recombine(real CR);
		evaluation_tag select(bool is_stable);
		void cover_first_part(const epanet_individual & indi);
		void cover_second_part(const epanet_individual & indi);
		bool same_location(epanet_individual & indi);
		void initialize(int id);
		
		solution_type& trial();
	protected:
		void mutate_mass(real F, solution<variable_epanet, real> *r1,
			solution<variable_epanet, real> *r2,
			solution<variable_epanet, real> *r3,
			solution<variable_epanet, real> *r4 = 0,
			solution<variable_epanet, real> *r5 = 0);
		void mutate_location(const std::vector<real> & pro);
		void mutate_ST(const std::vector<real> & pro);
		void mutate_duration(const std::vector<real> & pro);

	public:
		real m_distance_fitness;
		real m_pu_distance_fitness;
		
	};


}

#endif // !EPANET_INDIVIDUAL_H


