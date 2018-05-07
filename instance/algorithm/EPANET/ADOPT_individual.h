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

#ifndef ADOPT_INDIVIDUAL
#define ADOPT_INDIVIDUAL

//#include "../../../core/algorithm/individual.h"
#include "../DE/DE_individual.h"
#include "../../problem/realworld/EPANET/epa_encoding.h"
#include "../../problem/realworld/EPANET/epanet.h"

namespace OFEC {
	class ADOPT_individual : public individual<variable_epanet, real>
	{
	protected:
		solution_type m_pv, m_pu;
	public:
		template<typename ... Args>
		ADOPT_individual(size_t no, Args&& ... args) : individual(no, std::forward<Args>(args)...), m_pv(no, std::forward<Args>(args)...), m_pu(no, std::forward<Args>(args)...) {}
		//ADOPT_individual(const ADOPT_individual & indi) : individual(indi) {}
		
		virtual void mutate__(double F, const std::vector<std::vector<double>> & pro, solution<variable_epanet, real> *r1,
			solution<variable_epanet, real> *r2,
			solution<variable_epanet, real> *r3,
			solution<variable_epanet, real> *r4 = 0,
			solution<variable_epanet, real> *r5 = 0);
		void recombine(double CR);
		evaluation_tag select(bool is_feasible);
		bool same_location(ADOPT_individual & indi);
		void initialize(int id);
		solution_type& trial();
	public:
		float m_distance_fitness;
		float m_pu_distance_fitness;
	};
}

#endif // !ADOPT_INDIVIDUAL

