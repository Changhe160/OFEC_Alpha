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

#include "F20_rotated_hybrid_bound.h"


namespace OFEC {
	F20_rotated_hybrid_bound::F20_rotated_hybrid_bound(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		F18_rotated_hybrid_composition((v[param_proName]), (v[param_numDim]), 1) {

		initialize();
	}
	F20_rotated_hybrid_bound::F20_rotated_hybrid_bound(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		F18_rotated_hybrid_composition(name, size_var, size_obj) {

		initialize();
	}
	F20_rotated_hybrid_bound::~F20_rotated_hybrid_bound() {
		//dtor


	}

	void F20_rotated_hybrid_bound::initialize() {
		
		bool is_load = load_rotation("instance/problem/continuous/global/classical/CEC2005/data/");
		if (!is_load) {
			set_rotation();
		}
		compute_fmax();

		is_load = load_translation("instance/problem/continuous/global/classical/CEC2005/data/");  //data path
		if (!is_load) {
			set_translation();
		}
		for (auto &i : m_function) {
			i->clear_global_opt();
			i->set_global_opt(i->translation().data());
		}
		// Set optimal solution
		clear_global_opt();
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

	void F20_rotated_hybrid_bound::evaluate__(real *x, std::vector<real>& obj) {
		F18_rotated_hybrid_composition::evaluate__(x, obj);
	}

	
	
}


