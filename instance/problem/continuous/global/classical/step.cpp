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

#include "step.h"
namespace OFEC {
	
	step::step(param_map &v) :
		step((v.at("proName")), (v.at("numDim")), 1) {

		
	}
	step::step(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void step::initialize() {
		m_variable_monitor = true;
		set_range(-100, 100);
		set_init_range(-100, 100);
		set_original_global_opt();

		set_global_opt();

	}

	void step::evaluate__(real *x, std::vector<real>& obj) {
		double fitness = 0;

		for (size_t i = 0; i < m_variable_size; ++i) {
			fitness += fabs((double)int(x[i] + 0.5)*int(x[i] + 0.5));
		}

		obj[0] = fitness + m_bias;

	}
	
}