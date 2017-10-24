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

#include "F17_hybrid_composition_noisy.h"

namespace OFEC {
	namespace CEC2005 {
		F17_hybrid_composition_noisy::F17_hybrid_composition_noisy(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			F16_rotated_hybrid_composition_F15((v[param_proName]), (v[param_numDim]), 1) {

			initialize();
		}
		F17_hybrid_composition_noisy::F17_hybrid_composition_noisy(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			F16_rotated_hybrid_composition_F15(name, size_var, size_obj) {

			initialize();
		}
		F17_hybrid_composition_noisy::~F17_hybrid_composition_noisy() {
			//dtor
		}

		void F17_hybrid_composition_noisy::initialize() {
			m_noise_flag = true;

			// Set optimal solution
			m_optima.clear();
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			for (int i = 0; i < m_variable_size; ++i) {
				x.get_variable()[i] = m_function[0]->translation()[i];
			}
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
			// end set
			m_variable_accuracy = 1.0e-3;
			m_objective_accuracy = 0.1;
		}

		void F17_hybrid_composition_noisy::evaluate__(real *x, std::vector<real>& obj) {
			F16_rotated_hybrid_composition_F15::evaluate__(x, obj);
			obj[0] -= m_bias;
			obj[0] = obj[0] * noise() + m_bias;

		}
		double F17_hybrid_composition_noisy::noise() {
			return (1 + 0.2*fabs(global::ms_global->m_normal[caller::Problem]->next()));
		}
	}
}


