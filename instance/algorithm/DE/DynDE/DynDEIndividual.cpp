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


#include "DynDEIndividual.h"

namespace OFEC {

	DynDEindividual::DynDEindividual(const DynDEindividual & indi) :DEindividual(indi) {
		m_type = indi.m_type;
	}
	
	evaluation_tag DynDEindividual::brownian(const solution_type &best, double sigma) {
		for (size_t i = 0; i < GET_NUM_DIM; ++i) {
			m_var[i] = (best.get_variable()[i]) + global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0, sigma);
		}

		check_boundary_violation();

		return evaluate();
	}
	evaluation_tag DynDEindividual::quantum(const solution_type &best, double rcloud) {

		std::vector<double> x(GET_NUM_DIM, 0);
		double dis = 0;
		for (size_t i = 0; i < GET_NUM_DIM; ++i) {
			x[i] = global::ms_global->m_normal[caller::Algorithm]->next();
			dis += x[i] * x[i];
		}
		dis = sqrt(dis);
		double r = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(0, rcloud);
		for (size_t i = 0; i < GET_NUM_DIM; ++i) {
			m_var[i] = (best.get_variable()[i]) + r*x[i] / dis;
		}
		x.clear();
		check_boundary_violation();
		return evaluate();
	}
	evaluation_tag DynDEindividual::entropy(double sigma) {
		for (size_t i = 0; i < GET_NUM_DIM; ++i) {
			m_var[i] = m_var[i] + global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0, sigma);
		}

		check_boundary_violation();
		return evaluate();
	}
}