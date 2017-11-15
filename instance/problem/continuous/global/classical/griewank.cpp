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

#include "griewank.h"

namespace OFEC {
	
	griewank::griewank(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		set_range(-600, 600);
		set_init_range(-600., 600.);
		initialize();
	}
	griewank::griewank(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-600, 600);
		set_init_range(-600., 600.);
		initialize();
	}

	void griewank::initialize() {
		set_original_global_opt();

		set_global_opt();
		m_variable_accuracy = 1.0e-2;
	}


	void griewank::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		double result = 0;

		double s1 = 0, s2 = 1;
		for (int i = 0; i < m_variable_size; i++) {
			s1 += x[i] * x[i] / 4000.;
			s2 *= cos(x[i] / sqrt((double)(i + 1)));
		}
		result = s1 - s2 + 1. + m_bias;

		obj[0] = result;
	}
	
}