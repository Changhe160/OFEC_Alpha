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
******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#include "himmenblau.h"

namespace OFEC {
	
	himmenblau::himmenblau(param_map &v) : 
		himmenblau((v.at("proName")), 2, 1) {
		
	}
	himmenblau::himmenblau(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void himmenblau::initialize() {
		set_range(-6, 6);
		set_init_range(-6, 6);

		m_objective_accuracy = 0.5;
		m_variable_accuracy = 1.e-4;
		m_variable_monitor = true;
		// 1 gopt+3 lopt
		std::vector<std::vector<real>> var_data = { { 3.0f, 2.0f },{ 3.58149f, -1.8208f },{ 2.78706f, 3.1282f },{ -3.76343f, -3.26605f } };

		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	void himmenblau::evaluate_objective(real *x, std::vector<real> &obj) {
		real s = 0;

		real t0 = (x[0] * x[0] + x[1] - 11), t1 = (x[1] * x[1] + x[0] - 7);
		s = t0*t0 + t1*t1 + 0.1*(pow(x[0] - 3, 2) + pow(x[1] - 2, 2));
		obj[0] = s + m_bias;
	}
	
}