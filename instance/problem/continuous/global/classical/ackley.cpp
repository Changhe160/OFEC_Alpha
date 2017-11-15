
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

#include "ackley.h"
namespace OFEC {
	
	ackley::ackley(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-32.768, 32.768);
		set_init_range(-32.768, 32.768);
		initialize();
	}
	ackley::ackley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		set_range(-32.768, 32.768);
		set_init_range(-32.768, 32.768);
		initialize();
	}

	void ackley::initialize() {

		set_original_global_opt();

		set_global_opt();
	}

	void ackley::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		real s1 = 0, s2 = 0;

		for (int i = 0; i < m_variable_size; i++) {
			s1 += x[i] * x[i];
			s2 += cos(2 * OFEC_PI*x[i]);
		}

		obj[0] = -20 * exp(-0.2*sqrt(s1 / m_variable_size)) - exp(s2 / m_variable_size) + 20 + OFEC_E + m_bias;

	}
	
}