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
	
	rotated_scaffer_F6::rotated_scaffer_F6(param_map &v) :
		rotated_scaffer_F6((v.at("proName")), (v.at("numDim")), 1) {

	}
	rotated_scaffer_F6::rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		scaffer_F6(name, size_var, size_obj) {

	}

	void rotated_scaffer_F6::initialize() {
		m_variable_monitor = true;
		set_range(-100, 100);
		set_init_range(-100, 100);
		set_original_global_opt();
		set_bias(0);
		
		load_rotation("instance/problem/continuous/global/CEC2005/data/");
		
		set_global_opt();

		m_variable_accuracy = 1.0e-2;
		m_initialized = true;
	}

	void rotated_scaffer_F6::evaluate_objective(real *x, std::vector<real> &obj) {
		scaffer_F6::evaluate_objective(x, obj);
	}
	
}