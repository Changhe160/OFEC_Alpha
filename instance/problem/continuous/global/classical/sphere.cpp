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

#include "sphere.h"
namespace OFEC {
	
	sphere::sphere(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		set_range(-100., 100.);
		set_init_range(-100., 100.);
		initialize();

	}
	sphere::sphere(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		set_range(-100, 100);
		set_init_range(-100., 100.);
		initialize();
	}

	void sphere::initialize() {
		set_original_global_opt();

		set_global_opt();
	}


	void sphere::evaluate__(real *x, std::vector<real>& obj) {
		if (m_translation_flag)
			translate(x);
		if (m_scale_flag)
			scale(x);
		if (m_rotation_flag)
			rotate(x);
		if (m_translation_flag)
			translate_origin(x);

		double fit = 0;

		for (int i = 0; i < m_variable_size; i++) {
			fit += x[i] * x[i];
		}

		obj[0] = fit + m_bias;

	}
	
}