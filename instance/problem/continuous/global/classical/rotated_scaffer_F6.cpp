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

#include "rotated_scaffer_F6.h"
namespace OFEC {
	
	rotated_scaffer_F6::rotated_scaffer_F6(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		scaffer_F6((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-100, 100);
		set_init_range(-100, 100);
		initialize();
	}
	rotated_scaffer_F6::rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		scaffer_F6(name, size_var, size_obj) {

		set_range(-100, 100);
		set_init_range(-100, 100);
		initialize();
	}

	void rotated_scaffer_F6::initialize() {

		load_rotation("instance/problem/continuous/global/CEC2005/data/");
		m_optima.clear();
		set_global_opt();

		m_variable_accuracy = 1.0e-2;
	}

	void rotated_scaffer_F6::evaluate__(real *x, std::vector<real>& obj) {
		scaffer_F6::evaluate__(x, obj);
	}
	
}